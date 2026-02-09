#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

ast_t *ast_create(lexer_t *lexer) {
  ast_t *ast;
  ast = malloc(sizeof *ast);
  if (!ast) {
    perror("Error allocating memory");
    return NULL;
  }
  ast->arena = NULL;
  ast->arena_reserved = 0;
  ast->arena_size = 0;

  return ast;
}
void ast_destroy(ast_t *ctx) {
  if (ctx) {
    if (ctx->arena) {
      free(ctx->arena);
    }
    free(ctx);
  }
}

node_id ast_create_node(ast_t *ctx, node_t node) {
  if (ctx->arena_size == ctx->arena_reserved || ctx->arena == NULL) {
    ctx->arena_reserved = ctx->arena_reserved ? ctx->arena_reserved * 2 : 8;
    ctx->arena = realloc(ctx->arena, ctx->arena_reserved * sizeof *ctx->arena);
    if (!ctx->arena) {
      perror("Error allocating memory");
      return 0;
    }
  }
  ctx->arena[ctx->arena_size] = node;
  return ctx->arena_size++;
}
