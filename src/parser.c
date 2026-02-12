#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static token_t consume(parser_t *ctx);
static token_t peek(parser_t *ctx);

static node_id parse_identifier(parser_t *ctx);
static node_id parse_litteral(parser_t *ctx);
static node_id parse_return(parser_t *ctx);
static node_id parse_if(parser_t *ctx);
static node_id parse_function_declaration(parser_t *ctx);

parser_t *parser_create(ast_t *ast, lexer_t *lexer) {
  assert(ast);
  assert(lexer);

  parser_t *parser;
  parser = malloc(sizeof *parser);
  if (!parser) {
    perror("Error allocating memory");
    return NULL;
  }
  parser->ast = ast;
  parser->lexer = lexer;
  parser->cursor = 0;
  parser->root_node = 0;

  return parser;
}

void parser_destroy(parser_t *ctx) {
  if (ctx) {
    free(ctx);
  }
}

static token_t consume(parser_t *ctx) {
  assert(ctx);
  return lexer_get_token(ctx->lexer, ctx->cursor++);
}
static token_t peek(parser_t *ctx) {
  assert(ctx);
  return lexer_get_token(ctx->lexer, ctx->cursor);
}

static node_id parse_function_declaration(parser_t *ctx) {

}

static node_id parse_identifier(parser_t *ctx) {
  return ast_create_node(ctx->ast,
                         (node_t){.type = NODE_TYPE_IDENTIFIER,
                                  .as.identifier.type = TYPE_INT,
                                  .as.identifier.name = consume(ctx).data});
}

int parser_parse(void) {}
