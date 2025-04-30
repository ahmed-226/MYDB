#include "../include/common.h"

void indent(uint32_t level) {
  for (uint32_t i = 0; i < level; i++) {
    printf("  ");
  }
}

void print_constants() {
  printf("ROW_SIZE: %zu\n", (size_t)ROW_SIZE);
  printf("COMMON_NODE_HEADER_SIZE: %zu\n", (size_t)COMMON_NODE_HEADER_SIZE);
  printf("LEAF_NODE_HEADER_SIZE: %zu\n", (size_t)LEAF_NODE_HEADER_SIZE);
  printf("LEAF_NODE_CELL_SIZE: %zu\n", (size_t)LEAF_NODE_CELL_SIZE);
  printf("LEAF_NODE_SPACE_FOR_CELLS: %zu\n", (size_t)LEAF_NODE_SPACE_FOR_CELLS);
  printf("LEAF_NODE_MAX_CELLS: %zu\n", (size_t)LEAF_NODE_MAX_CELLS);
}