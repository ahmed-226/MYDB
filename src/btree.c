#include <stdlib.h>
#include <string.h>
#include "../include/btree.h"
#include "../include/common.h"
#include "../include/node.h"
#include "../include/pager.h"
#include "../include/table.h"

void print_tree(Pager* pager, uint32_t page_num, uint32_t indentation_level) {
  void* node = get_page(pager, page_num);
  uint32_t num_keys, child;

  switch (get_node_type(node)) {
    case (NODE_LEAF):
      num_keys = *leaf_node_num_cells(node);
      indent(indentation_level);
      printf("- leaf (size %d)\n", num_keys);
      for (uint32_t i = 0; i < num_keys; i++) {
        indent(indentation_level + 1);
        printf("- %d\n", *leaf_node_key(node, i));
      }
      break;
    case (NODE_INTERNAL):
      num_keys = *internal_node_num_keys(node);
      indent(indentation_level);
      printf("- internal (size %d)\n", num_keys);
      for (uint32_t i = 0; i < num_keys; i++) {
        child = *internal_node_child(node, i);
        print_tree(pager, child, indentation_level + 1);

        indent(indentation_level + 1);
        printf("- key %d\n", *internal_node_key(node, i));
      }
      child = *internal_node_right_child(node);
      print_tree(pager, child, indentation_level + 1);
      break;
  }
}

uint32_t get_node_max_key(Pager* pager, void* node) {
  if (get_node_type(node) == NODE_LEAF) {
      return *leaf_node_key(node, *leaf_node_num_cells(node) - 1);
  }
  void* right_child = get_page(pager, *internal_node_right_child(node));
  return get_node_max_key(pager, right_child);
}

uint32_t internal_node_find_child(void* node, uint32_t key) {
  uint32_t num_keys = *internal_node_num_keys(node);
  
  uint32_t min_index = 0;
  uint32_t max_index = num_keys; 
  while (min_index != max_index) {
    uint32_t index = (min_index + max_index) / 2;
    uint32_t key_to_right = *internal_node_key(node, index);
    if (key_to_right >= key) {
      max_index = index;
    } else {
      min_index = index + 1;
    }
  }
  return min_index;
}

void update_internal_node_key(void* parent, uint32_t old_key, uint32_t new_key) {
  uint32_t num_keys = *internal_node_num_keys(parent);
  for (uint32_t i = 0; i < num_keys; i++) {
    if (*internal_node_key(parent, i) == old_key) {
      *internal_node_key(parent, i) = new_key;
      return;
    }
  }
  printf("Error: Unable to find key %d in parent to update it to %d\n", old_key, new_key);
  exit(EXIT_FAILURE);
}

void internal_node_insert(Table* table, uint32_t parent_page_num, uint32_t child_page_num) {
  void* parent = get_page(table->pager, parent_page_num);
  void* child = get_page(table->pager, child_page_num);
  uint32_t child_max_key = get_node_max_key(table->pager, child);
  uint32_t index = internal_node_find_child(parent, child_max_key);
  
  uint32_t original_num_keys = *internal_node_num_keys(parent);
  
  if (original_num_keys >= INTERNAL_NODE_MAX_CELLS) {
      internal_node_split_and_insert(table, parent_page_num, child_page_num);
      return;
  }
  
  uint32_t right_child_page_num = *internal_node_right_child(parent);
  if (right_child_page_num == INVALID_PAGE_NUM) {
      *internal_node_right_child(parent) = child_page_num;
      *internal_node_num_keys(parent) = original_num_keys + 1;
      return;
  }
  
  void* right_child = get_page(table->pager, right_child_page_num);
  *internal_node_num_keys(parent) = original_num_keys + 1;
  
  if (child_max_key > get_node_max_key(table->pager, right_child)) {
      *internal_node_child(parent, original_num_keys) = right_child_page_num;
      *internal_node_key(parent, original_num_keys) = get_node_max_key(table->pager, right_child);
      *internal_node_right_child(parent) = child_page_num;
  } else {
      for (uint32_t i = original_num_keys; i > index; i--) {
          void* destination = internal_node_cell(parent, i);
          void* source = internal_node_cell(parent, i - 1);
          memcpy(destination, source, INTERNAL_NODE_CELL_SIZE);
      }
      *internal_node_child(parent, index) = child_page_num;
      *internal_node_key(parent, index) = child_max_key;
  }
}

void internal_node_split_and_insert(Table* table, uint32_t parent_page_num, uint32_t child_page_num) {
  uint32_t old_page_num = parent_page_num;
  void* old_node = get_page(table->pager,parent_page_num);
  uint32_t old_max = get_node_max_key(table->pager, old_node);

  void* child = get_page(table->pager, child_page_num); 
  uint32_t child_max = get_node_max_key(table->pager, child);

  uint32_t new_page_num = get_unused_page_num(table->pager);

  uint32_t splitting_root = is_node_root(old_node);

  void* parent;
  void* new_node;
  if (splitting_root) {
    create_new_root(table, new_page_num);
    parent = get_page(table->pager,table->root_page_num);

    old_page_num = *internal_node_child(parent,0);
    old_node = get_page(table->pager, old_page_num);
  } else {
    parent = get_page(table->pager,*node_parent(old_node));
    new_node = get_page(table->pager, new_page_num);
    initialize_internal_node(new_node);
  }
  
  uint32_t* old_num_keys = internal_node_num_keys(old_node);

  uint32_t cur_page_num = *internal_node_right_child(old_node);
  void* cur = get_page(table->pager, cur_page_num);

  internal_node_insert(table, new_page_num, cur_page_num);
  *node_parent(cur) = new_page_num;
  *internal_node_right_child(old_node) = INVALID_PAGE_NUM;

  for (int i = INTERNAL_NODE_MAX_CELLS - 1; i > INTERNAL_NODE_MAX_CELLS / 2; i--) {
    cur_page_num = *internal_node_child(old_node, i);
    cur = get_page(table->pager, cur_page_num);

    internal_node_insert(table, new_page_num, cur_page_num);
    *node_parent(cur) = new_page_num;

    (*old_num_keys)--;
  }


  *internal_node_right_child(old_node) = *internal_node_child(old_node,*old_num_keys - 1);
  (*old_num_keys)--;


  uint32_t max_after_split = get_node_max_key(table->pager, old_node);

  uint32_t destination_page_num = child_max < max_after_split ? old_page_num : new_page_num;

  internal_node_insert(table, destination_page_num, child_page_num);
  *node_parent(child) = destination_page_num;

  update_internal_node_key(parent, old_max, get_node_max_key(table->pager, old_node));

  if (!splitting_root) {
    internal_node_insert(table,*node_parent(old_node),new_page_num);
    *node_parent(new_node) = *node_parent(old_node);
  }
}