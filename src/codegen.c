#include "codegen.h"
#include "ast.h"
#include <assert.h>
#include <llvm-c/Core.h>
#include <llvm-c/Types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GET_NODE(ctx, id) (&(ctx)->ast->nodes[(id)])

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

void codegen_program(codegen_context_t *ctx, node_id program_node) {
  node_t *node = GET_NODE(ctx, program_node);
  program_t *program = &node->as.program;

  for (uint32_t i = 0; i < program->size; ++i) {
    node_id stmt_id = program->statements[i];
    node_t *stmt = GET_NODE(ctx, stmt_id);

    if (stmt->type == NODE_FUNCTION) {
      codegen_function(ctx, stmt_id);
    } else {
      printf("[Codegen ERROR]: Top-level node isn't a function, instead: %d\n",
             stmt->type);
    }
  }
}

LLVMValueRef codegen_node(codegen_context_t *ctx, node_id nodeid) {
  assert(ctx);
  assert(nodeid && "Node can't ");

  switch (GET_NODE(ctx, nodeid)->type) {
  case NODE_BINARY_EXPR:
    return codegen_binary_expr(ctx, nodeid);
  case NODE_UNARY_EXPR:
    return codegen_unary_expr(ctx, nodeid);
  case NODE_LITERAL_EXPR:
    return codegen_literal_expr(ctx, nodeid);
  case NODE_IDENTIFIER_EXPR:
    return codegen_identifier_expr(ctx, nodeid);
  case NODE_ASSIGN_EXPR:
    return codegen_assign_expr(ctx, nodeid);
  case NODE_CALL_EXPR:
    return codegen_call_expr(ctx, nodeid);
  case NODE_EXPR_STMT:
    codegen_expr_stmt(ctx, nodeid);
    return NULL;
  case NODE_VAR_DECL_STMT:
    codegen_var_decl(ctx, nodeid);
    return NULL;
  case NODE_BLOCK_STMT:
    codegen_block_stmt(ctx, nodeid);
    return NULL;
  case NODE_IF_STMT:
    codegen_if_stmt(ctx, nodeid);
    return NULL;
  case NODE_WHILE_STMT:
    codegen_if_stmt(ctx, nodeid);
    return NULL;
  case NODE_RETURN_STMT:
    codegen_return_stmt(ctx, nodeid);
    return NULL;
  case NODE_FUNCTION:
    codegen_function(ctx, nodeid);
    return NULL;
  case NODE_PROGRAM:
    codegen_program(ctx, nodeid);
    return NULL;
  default:
    printf("[Codegen ERROR]: Unhandled enum, for codegen: %d\n",
           GET_NODE(ctx, nodeid)->type);
    return NULL;
  }
}

LLVMValueRef codegen_binary_expr(codegen_context_t *ctx, node_id id) {}

LLVMValueRef codegen_unary_expr(codegen_context_t *ctx, node_id id) {}

LLVMValueRef codegen_literal_expr(codegen_context_t *ctx, node_id id) {
  // Get value from node id
  node_t *node = GET_NODE(ctx, id);

  // HACK: WE are only using integers for the moment

  // Create llvmtypedef in context
  LLVMTypeRef type = LLVMInt32TypeInContext(ctx->context);

  // Create llvmvalueref from typedef
  LLVMValueRef value = LLVMConstInt(type, node->as.literal_expr.value, 1);

  // Return
  return value;
}

LLVMValueRef codegen_identifier_expr(codegen_context_t *ctx, node_id id) {}

LLVMValueRef codegen_assign_expr(codegen_context_t *ctx, node_id id) {}

LLVMValueRef codegen_call_expr(codegen_context_t *ctx, node_id id) {}

void codegen_expr_stmt(codegen_context_t *ctx, node_id id) {}

void codegen_var_decl(codegen_context_t *ctx, node_id id) {}

void codegen_block_stmt(codegen_context_t *ctx, node_id id) {}

void codegen_if_stmt(codegen_context_t *ctx, node_id id) {}

void codegen_while_stmt(codegen_context_t *ctx, node_id id) {}

void codegen_return_stmt(codegen_context_t *ctx, node_id id) {
  node_t *node = GET_NODE(ctx, id);

  LLVMValueRef return_value = codegen_node(ctx, id);
  if (!return_value) {
    printf("[Codegen Error]: No return value");
  }

  LLVMBuildRet(ctx->builder, return_value);
}

void codegen_function(codegen_context_t *ctx, node_id id) {}
