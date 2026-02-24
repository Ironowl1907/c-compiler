#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

ast_t *ast_create(void) {
  ast_t *ast;
  ast = malloc(sizeof *ast);
  if (!ast) {
    perror("Error allocating memory");
    return NULL;
  }
  ast->nodes = NULL;
  ast->capacity = 0;
  ast->size = 0;

  return ast;
}
void ast_destroy(ast_t *ctx) {
  if (ctx) {
    if (ctx->nodes) {
      free(ctx->nodes);
    }
    free(ctx);
  }
}

node_id ast_create_node(ast_t *ctx, node_t node) {
  if (ctx->size == ctx->capacity || ctx->nodes == NULL) {
    ctx->capacity = ctx->capacity ? ctx->capacity * 2 : 8;
    ctx->nodes = realloc(ctx->nodes, ctx->capacity * sizeof *ctx->nodes);
    if (!ctx->nodes) {
      perror("Error allocating memory");
      return 0;
    }
  }
  ctx->nodes[ctx->size] = node;
  return ctx->size++;
}
