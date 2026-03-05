#include "codegen.h"
#include <assert.h>
#include <llvm-c/Core.h>
#include <stdlib.h>
#include <string.h>

codegen_context_t *codegen_ctx_create(ast_t *ast, const char *module_name) {
  codegen_context_t *ctx = malloc(sizeof *ctx);
  assert(ctx && "NULL allocating codegen context");

  ctx->ast = ast;
  ctx->context = LLVMContextCreate();
  ctx->module = LLVMModuleCreateWithNameInContext(module_name, ctx->context);
  ctx->builder = LLVMCreateBuilderInContext(ctx->context);

  sym_table_clear(&ctx->symbols);
  return ctx;
}

void codegen_ctx_destroy(codegen_context_t *ctx) {
  if (!ctx)
    return;
  LLVMDisposeBuilder(ctx->builder);
  LLVMDisposeModule(ctx->module);
  LLVMContextDispose(ctx->context);
  free(ctx);
}

void sym_table_clear(symbol_table_t *tbl) { memset(tbl, 0, sizeof(*tbl)); }

void sym_table_set(symbol_table_t *tbl, const char *name, size_t len,
                   LLVMValueRef slot) {}

LLVMValueRef sym_table_get(symbol_table_t *tbl, const char *name, size_t len) {}

void codegen_program(codegen_context_t *ctx, node_id program_node) {}

LLVMValueRef codegen_binary_expr(codegen_context_t *ctx, node_id id) {}

LLVMValueRef codegen_unary_expr(codegen_context_t *ctx, node_id id) {}

LLVMValueRef codegen_literal_expr(codegen_context_t *ctx, node_id id) {}

LLVMValueRef codegen_identifier_expr(codegen_context_t *ctx, node_id id) {}

LLVMValueRef codegen_assign_expr(codegen_context_t *ctx, node_id id) {}

LLVMValueRef codegen_call_expr(codegen_context_t *ctx, node_id id) {}

void codegen_expr_stmt(codegen_context_t *ctx, node_id id) {}

void codegen_var_decl(codegen_context_t *ctx, node_id id) {}

void codegen_block_stmt(codegen_context_t *ctx, node_id id) {}

void codegen_if_stmt(codegen_context_t *ctx, node_id id) {}

void codegen_while_stmt(codegen_context_t *ctx, node_id id) {}

void codegen_return_stmt(codegen_context_t *ctx, node_id id) {}

void codegen_function(codegen_context_t *ctx, node_id id) {}
