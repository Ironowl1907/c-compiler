#ifndef LEXER_H
#define LEXER_H

#include "str_slice.h"
#include <stddef.h>
#include <stdint.h>

typedef enum {
  TOKEN_TYPE_IDENTIFIER,
  TOKEN_TYPE_INT_LITERAL,

  TOKEN_TYPE_PLUS,
  TOKEN_TYPE_MINUS,
  TOKEN_TYPE_STAR,
  TOKEN_TYPE_FSLASH,
  TOKEN_TYPE_LPARENTESIS,
  TOKEN_TYPE_RPARENTESIS,
  TOKEN_TYPE_LBRACE,
  TOKEN_TYPE_RBRACE,

  TOKEN_TYPE_RETURN,
  TOKEN_TYPE_KW_INT,
  TOKEN_TYPE_SEMICOLON,

  TOKEN_TYPE_EOF,
} token_type_e;

typedef struct {
  uint32_t line;
  uint32_t column;
} position_t;


typedef struct {
  token_type_e type;
  str_slice_t data;
  position_t pos;
} token_t;

typedef struct {
  char *data; // null-terminated
  token_t *tokens;

  size_t reserved_tokens;
  uint32_t data_cursor;
  uint32_t last_token;

  uint32_t line;
  uint32_t column;
} lexer_t;

lexer_t *lexer_create(const char *data);
void lexer_delete(lexer_t *ctx);

int lexer_lex(lexer_t *ctx);

void lexer_debug_print_tokens(lexer_t * ctx);

#endif
