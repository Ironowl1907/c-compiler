#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <llvm-c/Types.h>
#include <stdint.h>
#define SYMBOL_TABLE_MAX 256;

typedef struct {
  const char *name;
  size_t name_size;
  LLVMValueRef slot;
} symbol_entry_t;

typedef struct {
  symbol_entry_t entries[SYMBOL_TABLE_MAX];
  uint32_t size;
} symbol_table_t;

typedef struct {
  ast_t ast;

  LLVMContextRef context;
  LLVMModuleRef module;
  LLVMBuilderRef builder;

  symbol_table_t symbols;
} codegen_context_t;

////////////////////////////////////////////////////////
//////////////////// Life Cycle /////////////////////////
////////////////////////////////////////////////////////
codegen_context_t *codegen_ctx_create(ast_t *ast, const char *module_name);
void codegen_ctx_destroy(codegen_context_t *ctx);

#endif
