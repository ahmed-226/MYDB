#ifndef BTREE_H
#define BTREE_H

#include "common.h"
#include "pager.h"

void print_tree(Pager* pager, uint32_t page_num, uint32_t indentation_level);

#endif