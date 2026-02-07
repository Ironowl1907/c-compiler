#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char peek(lexer_t *ctx);
static char consume(lexer_t *ctx);
static int append_token(lexer_t *ctx, token_type_e type, char *start,
                        uint32_t size, position_t pos);

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

static int append_token(lexer_t *ctx, token_type_e type, char *start,
                        uint32_t size, position_t pos) {
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
      .data = {start, size},
      .pos = pos,
  };

  return 0;
}

int lexer_lex(lexer_t *ctx) {
  if (!ctx)
    return 1;

  while (peek(ctx) != '\0') {
    char c = peek(ctx);

    if (isspace((unsigned char)c)) {
      consume(ctx);
      continue;
    }

    position_t start_pos = {ctx->line, ctx->column};
    char *start = ctx->data + ctx->data_cursor;

    switch (c) {
    case '+':
      consume(ctx);
      append_token(ctx, TOKEN_TYPE_PLUS, start, 1, start_pos);
      break;
    case '-':
      consume(ctx);
      append_token(ctx, TOKEN_TYPE_MINUS, start, 1, start_pos);
      break;
    case '*':
      consume(ctx);
      append_token(ctx, TOKEN_TYPE_STAR, start, 1, start_pos);
      break;
    case '/':
      consume(ctx);
      append_token(ctx, TOKEN_TYPE_FSLASH, start, 1, start_pos);
      break;
    case '(':
      consume(ctx);
      append_token(ctx, TOKEN_TYPE_LPARENTESIS, start, 1, start_pos);
      break;
    case ')':
      consume(ctx);
      append_token(ctx, TOKEN_TYPE_RPARENTESIS, start, 1, start_pos);
      break;
    case '{':
      consume(ctx);
      append_token(ctx, TOKEN_TYPE_LBRACE, start, 1, start_pos);
      break;
    case '}':
      consume(ctx);
      append_token(ctx, TOKEN_TYPE_RBRACE, start, 1, start_pos);
      break;
    case ';':
      consume(ctx);
      append_token(ctx, TOKEN_TYPE_SEMICOLON, start, 1, start_pos);
      break;

    default:
      if (isdigit((unsigned char)c)) {
        while (isdigit((unsigned char)peek(ctx)))
          consume(ctx);

        uint32_t size = ctx->data + ctx->data_cursor - start;
        append_token(ctx, TOKEN_TYPE_INT_LITERAL, start, size, start_pos);
        continue;
      }

      if (isalpha((unsigned char)c) || c == '_') {
        consume(ctx);
        while (isalnum((unsigned char)peek(ctx)) || peek(ctx) == '_')
          consume(ctx);

        uint32_t size = ctx->data + ctx->data_cursor - start;

        if (size == 6 && memcmp(start, "return", 6) == 0) {
          append_token(ctx, TOKEN_TYPE_RETURN, start, size, start_pos);
        } else if (size == 3 && memcmp(start, "int", 3) == 0) {
          append_token(ctx, TOKEN_TYPE_KW_INT, start, size, start_pos);
        } else {
          append_token(ctx, TOKEN_TYPE_IDENTIFIER, start, size, start_pos);
        }

        continue;
      }

      fprintf(stderr, "[LEXER ERROR] Unexpected character '%c' at %u:%u\n", c,
              ctx->line, ctx->column);
      return 1;
    }
  }

  append_token(ctx, TOKEN_TYPE_EOF, ctx->data + ctx->data_cursor, 0,
               (position_t){ctx->line, ctx->column});

  return 0;
}
