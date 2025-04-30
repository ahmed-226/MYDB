#ifndef BTREE_H
#define BTREE_H

#include "common.h"
#include "pager.h"

struct Table;

uint32_t get_node_max_key(Pager* pager, void* node);
void internal_node_insert(Table* table, uint32_t parent_page_num, uint32_t child_page_num);
void update_internal_node_key(void* parent, uint32_t old_key, uint32_t new_key);
uint32_t internal_node_find_child(void* node, uint32_t key);
void internal_node_split_and_insert(Table* table, uint32_t parent_page_num, uint32_t child_page_num);
void print_tree(Pager* pager, uint32_t page_num, uint32_t indentation_level);

#endif 