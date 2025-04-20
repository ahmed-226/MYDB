#ifndef TABLE_H
#define TABLE_H

#include "common.h"
#include "pager.h"
#include "row.h"

typedef struct {
  Pager* pager;
  uint32_t root_page_num;
} Table;

Table* db_open(const char* filename);
void db_close(Table* table);
void create_new_root(Table* table, uint32_t right_child_page_num);

#endif 