#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char peek(lexer_t *ctx);
static char consume(lexer_t *ctx);
static int append_token(lexer_t *ctx, token_type_e type, char *val);

lexer_t *lexer_create(const char *data) {
  lexer_t *lexer = malloc(sizeof *lexer);
  if (!lexer)
    return NULL;

  size_t len = strlen(data);
  lexer->data = malloc(len + 1);
  if (!lexer->data) {
    free(lexer);
    return NULL;
  }

  memcpy(lexer->data, data, len + 1);

  lexer->tokens = NULL;
  lexer->reserved_tokens = 0;
  lexer->last_token = 0;
  lexer->data_cursor = 0;

  lexer->line = 1;
  lexer->column = 1;

  return lexer;
}

void lexer_delete(lexer_t *ctx) {
  if (!ctx)
    return;

  for (uint32_t i = 0; i < ctx->last_token; ++i) {
    free(ctx->tokens[i].val);
  }

  free(ctx->tokens);
  free(ctx->data);
  free(ctx);
}

static char peek(lexer_t *ctx) { return ctx->data[ctx->data_cursor]; }

static char consume(lexer_t *ctx) {
  char c = ctx->data[ctx->data_cursor++];

  if (c == '\n') {
    ctx->line++;
    ctx->column = 1;
  } else {
    ctx->column++;
  }

  return c;
}

static int append_token(lexer_t *ctx, token_type_e type, char *val) {
  if (!ctx->tokens || ctx->last_token == ctx->reserved_tokens) {
    size_t new_cap = ctx->tokens ? ctx->reserved_tokens * 2 : 8;
    token_t *tmp = realloc(ctx->tokens, new_cap * sizeof *ctx->tokens);
    if (!tmp)
      return 1;

    ctx->tokens = tmp;
    ctx->reserved_tokens = new_cap;
  }

  ctx->tokens[ctx->last_token++] = (token_t){
      .type = type,
      .val = val,
      .pos = {ctx->line, ctx->column},
  };

  return 0;
}

int lexer_lex(lexer_t *ctx) {
  if (!ctx)
    return 1;

  while (peek(ctx) != '\0') {
    char c = peek(ctx);

    if (isspace(c)) {
      consume(ctx);
      continue;
    }

    switch (c) {
    case '+':
      consume(ctx);
      append_token(ctx, TOKEN_TYPE_PLUS, strdup("+"));
      break;
    case '-':
      consume(ctx);
      append_token(ctx, TOKEN_TYPE_MINUS, strdup("-"));
      break;
    case '*':
      consume(ctx);
      append_token(ctx, TOKEN_TYPE_STAR, strdup("*"));
      break;
    case '/':
      consume(ctx);
      append_token(ctx, TOKEN_TYPE_FSLASH, strdup("/"));
      break;
    case '(':
      consume(ctx);
      append_token(ctx, TOKEN_TYPE_LPARENTESIS, strdup("("));
      break;
    case ')':
      consume(ctx);
      append_token(ctx, TOKEN_TYPE_RPARENTESIS, strdup(")"));
      break;
    case '{':
      consume(ctx);
      append_token(ctx, TOKEN_TYPE_LBRACE, strdup("{"));
      break;
    case '}':
      consume(ctx);
      append_token(ctx, TOKEN_TYPE_RBRACE, strdup("}"));
      break;
    case ';':
      consume(ctx);
      append_token(ctx, TOKEN_TYPE_SEMICOLON, strdup(";"));
      break;

    default:
      if (isdigit(c)) {
        char *start = ctx->data + ctx->data_cursor;
        while (isdigit(peek(ctx)))
          consume(ctx);

        size_t len = ctx->data + ctx->data_cursor - start;
        char *num = malloc(len + 1);
        memcpy(num, start, len);
        num[len] = '\0';

        append_token(ctx, TOKEN_TYPE_INT_LITERAL, num);
        continue;
      }

      if (isalpha(c) || c == '_') {
        char *start = ctx->data + ctx->data_cursor;
        consume(ctx);
        while (isalnum(peek(ctx)) || peek(ctx) == '_')
          consume(ctx);

        size_t len = ctx->data + ctx->data_cursor - start;
        char *id = malloc(len + 1);
        memcpy(id, start, len);
        id[len] = '\0';

        if (strcmp(id, "return") == 0) {
          free(id);
          append_token(ctx, TOKEN_TYPE_RETURN, strdup("return"));
        } else if (strcmp(id, "int") == 0) {
          free(id);
          append_token(ctx, TOKEN_TYPE_KW_INT, strdup("int"));
        } else {
          append_token(ctx, TOKEN_TYPE_IDENTIFIER, id);
        }

        continue;
      }

      fprintf(stderr, "[LEXER ERROR] Unexpected character '%c' at %u:%u\n", c,
              ctx->line, ctx->column);
      return 1;
    }
  }

  append_token(ctx, TOKEN_TYPE_EOF, strdup("<eof>"));
  return 0;
}
