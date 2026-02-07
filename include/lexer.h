#ifndef LEXER_H
#define LEXER_H

typedef enum {
  TOKEN_TYPE_IDENTIFIER,
  TOKEN_TYPE_LITTERAL,

  TOKEN_TYPE_PLUS,
  TOKEN_TYPE_MINUS,
  TOKEN_TYPE_STAR,
  TOKEN_TYPE_FSLASH,
  TOKEN_TYPE_BSLASH,
  TOKEN_TYPE_LPARENTESIS,
  TOKEN_TYPE_RPARENTESIS,

} token_type_e;

typedef struct {

} token_t;

typedef struct {
  // Must be null terminated
  char *data;
  token_t *tokens;
  char *data_cursor;
  token_t *token_cursor;

} lexer_t;

lexer_t *lexer_create(const char *data);
void lexer_delete(lexer_t *ctx);

int lexer_lex(lexer_t *ctx);

#endif
