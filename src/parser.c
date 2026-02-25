#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ===============================================
// ============ Static Declarations ===============
// ===============================================

static token_t consume(parser_t *ctx);
static token_t peek(parser_t *ctx);
static const char *token_type_2_str(token_type_e type);
static inline uint32_t binding_power(token_type_e type);

static void parser_error(parser_t *ctx, const char *message);
static void expect_token(parser_t *ctx, token_type_e expected_type);
static token_t expect_token_consume(parser_t *ctx, token_type_e expected_type);

static node_id parse_primary(parser_t *ctx);
static node_id parse_expresion(parser_t *ctx, uint32_t min_bp);

program_t parser_parse(parser_t *ctx) {
  program_t program;
  program.size = 0;
  program.reserved = 8;
  program.statements = malloc(sizeof program.statements * program.reserved);

  while (peek(ctx).type != TOKEN_TYPE_EOF) {
    // TODO: Global funciton and variable parsing
    // For now only expresion parsing
    node_id statement;
    statement = parse_expresion(ctx, 0);

    program_append_statement(&program, statement);
  }
	return program;
}

static node_id parse_primary(parser_t *ctx) {
  token_t tk = peek(ctx);

  switch (tk.type) {
  case TOKEN_TYPE_INT_LITERAL: {
    consume(ctx);

    char buffer[32];

    if (tk.data.size >= sizeof(buffer))
      parser_error(ctx, "Integer literal too large");

    memcpy(buffer, tk.data.start, tk.data.size);
    buffer[tk.data.size] = '\0';

    int32_t value = (int32_t)strtol(buffer, NULL, 10);

    return ast_make_literal_expr(ctx->ast, value);
  }

  case TOKEN_TYPE_IDENTIFIER: {
    consume(ctx);

    return ast_make_identifier_expr(ctx->ast, tk.data);
  }

  case TOKEN_TYPE_LPARENTESIS: {
    consume(ctx); // eat '('

    node_id expr = parse_expresion(ctx, 0);

    expect_token_consume(ctx, TOKEN_TYPE_RPARENTESIS);

    return expr;
  }

  default:
    parser_error(ctx, "Expected primary expression");
    return 0;
  }
}

static node_id parse_expresion(parser_t *ctx, uint32_t min_bp) {
  token_t tok = peek(ctx);
  node_id lhs = parse_primary(ctx);

  while (binding_power(peek(ctx).type) > min_bp) {
    token_t op = consume(ctx);
    int bp = binding_power(op.type);

    node_id rhs = parse_expresion(ctx, bp);

    switch (op.type) {
    case TOKEN_TYPE_PLUS:
      lhs = ast_make_binary_expr(ctx->ast, OP_ADD, lhs, rhs);
      break;
    case TOKEN_TYPE_MINUS:
      lhs = ast_make_binary_expr(ctx->ast, OP_LESS, lhs, rhs);
      break;
    case TOKEN_TYPE_STAR:
      lhs = ast_make_binary_expr(ctx->ast, OP_MUL, lhs, rhs);
      break;
    case TOKEN_TYPE_FSLASH:
      lhs = ast_make_binary_expr(ctx->ast, OP_DIV, lhs, rhs);
      break;
    default:
      printf("[Parser Error]: Unexpected token at %d, %d\n", tok.pos.column,
             tok.pos.line);
    }
  }
  return lhs;
}

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
    printf("[Parsed Error]: Expected %s in position %d, %d. Instead %s\n",
           token_type_2_str(expected_type), token.pos.column, token.pos.line,
           token_type_2_str(token.type));

    exit(1);
  }
}

static token_t expect_token_consume(parser_t *ctx, token_type_e expected_type) {
  expect_token(ctx, expected_type);
  return consume(ctx);
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

static void parser_error(parser_t *ctx, const char *message) {
  printf("%s, at: %d, %d\n", message, peek(ctx).pos.line, peek(ctx).pos.column);
  exit(1);
}

static inline uint32_t binding_power(token_type_e type) {
  switch (type) {
  case TOKEN_TYPE_INT_LITERAL:
    return 1;
  case TOKEN_TYPE_IDENTIFIER:
    return 1;
  case TOKEN_TYPE_PLUS:
    return 2;
  case TOKEN_TYPE_MINUS:
    return 2;
  case TOKEN_TYPE_STAR:
    return 3;
  case TOKEN_TYPE_FSLASH:
    return 3;
  case TOKEN_TYPE_EOF:
    return 0;
  default:
    printf("[Parser Error]: No binding power for %s\n", token_type_2_str(type));
    exit(1);
  }
  return 0;
}
