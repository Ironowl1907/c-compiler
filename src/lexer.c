#include "lexer.h"
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static char peek(lexer_t *ctx);
static char consume(lexer_t *ctx);

static int append_token(lexer_t *ctx, token_type_e type, char *info);

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

  lexer->last_token = 0;
  lexer->data_cursor = 0;
  lexer->tokens = NULL;

  return lexer;
}

void lexer_delete(lexer_t *ctx) {
  if (!ctx)
    return;

  free(ctx->data);
  free(ctx->tokens);
  free(ctx);
}

static char peek(lexer_t *ctx) { return ctx->data[ctx->data_cursor]; }
static char consume(lexer_t *ctx) { return ctx->data[ctx->data_cursor++]; }

static int append_token(lexer_t *ctx, token_type_e type, char *info) {
  if (ctx->last_token == ctx->reserved_tokens || ctx->tokens == NULL) {
    ctx->reserved_tokens = ctx->tokens == NULL ? 8 : ctx->reserved_tokens * 2;

    ctx->tokens = realloc(ctx->tokens, ctx->reserved_tokens);
    if (!ctx->tokens)
      return 1;
  }

  // TODO: Implement token tracking
  ctx->tokens[ctx->last_token].pos = (position_t){0};
  ctx->tokens[ctx->last_token].type = type;
  ctx->tokens[ctx->last_token].val = info;

  ctx->last_token++;
  return 0;
}

int lexer_lex(lexer_t *ctx) {
  if (!ctx)
    return 1;

  while (peek(ctx) != '\0') {
    char c = peek(ctx);
    switch (c) {
    case '+':
      append_token(ctx, TOKEN_TYPE_PLUS, "+");
      break;
    case '-':
      append_token(ctx, TOKEN_TYPE_MINUS, "-");
      break;
    case '*':
      append_token(ctx, TOKEN_TYPE_STAR, "*");
      break;
    case '/':
      append_token(ctx, TOKEN_TYPE_FSLASH, "/");
      break;
    case '(':
      append_token(ctx, TOKEN_TYPE_LPARENTESIS, "(");
      break;
    case ')':
      append_token(ctx, TOKEN_TYPE_RPARENTESIS, ")");
      break;
    default: {
      if (isdigit(c)) {
        char *number_start = ctx->data + ctx->data_cursor;
        consume(ctx);
        while (isdigit(peek(ctx)))
          consume(ctx);
        uint32_t str_size = &ctx->data[ctx->data_cursor] - number_start;

        char *string = malloc(sizeof *string * str_size);
        memcpy(string, number_start, str_size);
        string[str_size] = '\0';

        append_token(ctx, TOKEN_TYPE_INT, string);
        continue;
      }
				else if (isalpha(c)){

				}
    }
    }
    consume(ctx);
  }

  return 0;
}
