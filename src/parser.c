#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static token_t consume(parser_t *ctx);
static token_t peek(parser_t *ctx);
static void expect_token(parser_t *ctx, token_type_e expected_type);

static const char *token_type_2_str(token_type_e type);

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

static void expect_token(parser_t *ctx, token_type_e expected_type) {
  token_t token = peek(ctx);
  if (token.type != expected_type) {
    printf("[Parsed Error]: Expected %s in position %d, %d. Instead %s",
           token_type_2_str(expected_type), token.pos.column, token.pos.line,
           token_type_2_str(token.type));

    exit(1);
  }
}

static const char *token_type_2_str(token_type_e type) {
  switch (type) {
  case TOKEN_TYPE_IDENTIFIER:
    return "IDENTIFIER";
  case TOKEN_TYPE_INT_LITERAL:
    return "INT_LITERAL";
  case TOKEN_TYPE_PLUS:
    return "+";
  case TOKEN_TYPE_MINUS:
    return "-";
  case TOKEN_TYPE_STAR:
    return "*";
  case TOKEN_TYPE_FSLASH:
    return "/";
  case TOKEN_TYPE_LPARENTESIS:
    return "(";
  case TOKEN_TYPE_RPARENTESIS:
    return ")";
  case TOKEN_TYPE_LBRACE:
    return "{";
  case TOKEN_TYPE_RBRACE:
    return "}";
  case TOKEN_TYPE_RETURN:
    return "RETURN";
  case TOKEN_TYPE_SEMICOLON:
    return ";";
  case TOKEN_TYPE_EOF:
    return "EOF";
    break;
  case TOKEN_TYPE_KW_INT:
    return "INT";
    break;
  }
  return "<UNDEFINED ENUM>";
}
