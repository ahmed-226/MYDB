#ifndef CURSOR_H
#define CURSOR_H

#include "common.h"
#include "table.h"
#include "row.h"
#include "node.h"

typedef struct {
  Table* table;
  uint32_t page_num;
  uint32_t cell_num;
  bool end_of_table;
} Cursor;

Cursor* table_start(Table* table);
Cursor* table_find(Table* table, uint32_t key);
void* cursor_value(Cursor* cursor);
void cursor_advance(Cursor* cursor);
Cursor* leaf_node_find(Table* table, uint32_t page_num, uint32_t key);
void leaf_node_insert(Cursor* cursor, uint32_t key, Row* value);
void leaf_node_split_and_insert(Cursor* cursor, uint32_t key, Row* value);
uint32_t* node_parent(void* node);

#endif