#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <llvm-c/Types.h>
#include <stdint.h>
#define SYMBOL_TABLE_MAX 256

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
  ast_t *ast;

  LLVMContextRef context;
  LLVMModuleRef module;
  LLVMBuilderRef builder;

  symbol_table_t symbols;
} codegen_context_t;

// ==================================================
// =================== Live Cycle ====================
// ==================================================

codegen_context_t *codegen_ctx_create(ast_t *ast, const char *module_name);

void codegen_ctx_destroy(codegen_context_t *ctx);

// ==================================================
// ================ Sysmbol table ====================
// ==================================================

void sym_table_clear(symbol_table_t *tbl);

void sym_table_set(symbol_table_t *tbl, const char *name, size_t len,
                   LLVMValueRef slot);

LLVMValueRef sym_table_get(symbol_table_t *tbl, const char *name, size_t len);

// ==================================================
// =============== Entry Point =======================
// ==================================================

void codegen_program(codegen_context_t *ctx, node_id program_node);

// ===================================================
// ============== Code Generation =====================
// ===================================================

// Entry generic node codegen
LLVMValueRef coden_node(codegen_context_t *ctx, node_id nodeid);

LLVMValueRef codegen_binary_expr(codegen_context_t *ctx, node_id id);

LLVMValueRef codegen_unary_expr(codegen_context_t *ctx, node_id id);

LLVMValueRef codegen_literal_expr(codegen_context_t *ctx, node_id id);

LLVMValueRef codegen_identifier_expr(codegen_context_t *ctx, node_id id);

LLVMValueRef codegen_assign_expr(codegen_context_t *ctx, node_id id);

LLVMValueRef codegen_call_expr(codegen_context_t *ctx, node_id id);

// Return void, side-effect generate ir

void codegen_expr_stmt(codegen_context_t *ctx, node_id id);

void codegen_var_decl(codegen_context_t *ctx, node_id id);

void codegen_block_stmt(codegen_context_t *ctx, node_id id);

void codegen_if_stmt(codegen_context_t *ctx, node_id id);

void codegen_while_stmt(codegen_context_t *ctx, node_id id);

void codegen_return_stmt(codegen_context_t *ctx, node_id id);

void codegen_function(codegen_context_t *ctx, node_id id);

#endif
