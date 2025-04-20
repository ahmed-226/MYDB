#ifndef STATEMENT_H
#define STATEMENT_H

#include "common.h"
#include "input_buffer.h"
#include "row.h"
#include "table.h"

typedef struct {
  StatementType type;
  Row row_to_insert;
} Statement;

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement);
PrepareResult prepare_insert(InputBuffer* input_buffer, Statement* statement);
ExecuteResult_t execute_statement(Statement* statement, Table* table);
ExecuteResult_t execute_insert(Statement* statement, Table* table);
ExecuteResult_t execute_select(Statement* statement, Table* table);
MetaCommandResult do_meta_command(InputBuffer* input_buffer, Table* table);

#endif 