#include "lexer.h"
#include <stdlib.h>
#include <string.h>

lexer_t *lexer_create(const char *data) {
  lexer_t *lexer = malloc(sizeof *lexer);
  if (!lexer) {
    return NULL;
  }
  lexer->data = malloc(sizeof *lexer->data * strlen(data));
  if (!lexer->data) {
    free(lexer);
    return NULL;
  }
  strcpy(lexer->data, data);

  lexer->tokens = malloc(sizeof *lexer->token_cursor * 8);
  if (!lexer->token_cursor) {
    free(lexer->data);
    free(lexer);
    return NULL;
  }

  lexer->token_cursor = lexer->tokens;
  lexer->data_cursor = lexer->data;

  return lexer;
}

void lexer_delete(lexer_t *ctx) {
  if (!ctx)
    return;

  free(ctx->data);
  free(ctx->tokens);
  free(ctx);
}

int lexer_lex(lexer_t *ctx) {}
