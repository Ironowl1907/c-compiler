#include "codegen.h"
#include <assert.h>
#include <llvm-c/Core.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Simple hash function
static uint32_t sym_hash(const char *name, size_t len);

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

static uint32_t sym_hash(const char *name, size_t len) {
  uint32_t h = 2166136261u;
  for (size_t i = 0; i < len; i++) {
    h ^= (unsigned char)name[i];
    h *= 16777619u;
  }
  return h;
}

void sym_table_clear(symbol_table_t *tbl) { memset(tbl, 0, sizeof(*tbl)); }

void sym_table_set(symbol_table_t *tbl, const char *name, size_t len,
                   LLVMValueRef slot) {
  assert(tbl && "Null parameter");
  assert(tbl->size < SYMBOL_TABLE_MAX - 1 && "Symbol table overflow");

  uint32_t idx = sym_hash(name, len);

  while (tbl->entries[idx].name != NULL) {
    // Overwrite existing key
    if (tbl->entries[idx].name_size == len &&
        memcmp(tbl->entries[idx].name, name, len) == 0) {
      tbl->entries[idx].slot = slot;
      return;
    }
    idx = (idx + 1) % SYMBOL_TABLE_MAX;
  }
  tbl->entries[idx].name = name;
  tbl->entries[idx].name_size = len;
  tbl->entries[idx].slot = slot;
  tbl->size++;
}

LLVMValueRef sym_table_get(symbol_table_t *tbl, const char *name, size_t len) {
  uint32_t idx = sym_hash(name, len) % SYMBOL_TABLE_MAX;
  for (uint32_t i = 0; i < SYMBOL_TABLE_MAX; i++) {
    symbol_entry_t *e = &tbl->entries[idx];
    if (e->name == NULL)
      return NULL;
    if (e->name_size == len && memcmp(e->name, name, len) == 0)
      return e->slot;
    idx = (idx + 1) % SYMBOL_TABLE_MAX;
  }
  return NULL;
}

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
