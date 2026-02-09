#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include <stdint.h>
typedef struct {
  ast_t *ast;
  node_id root_node;

  lexer_t *lexer;
  uint32_t cursor;
} parser_t;

parser_t *parser_create(ast_t *ast, lexer_t *lexer);
void parser_destroy(parser_t *ctx);

int parser_parse(void);

#endif
