#include "ast.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

ast_t *ast_create(void) {
  ast_t *ast;
  ast = malloc(sizeof *ast);
  if (!ast) {
    perror("Error allocating memory");
    return NULL;
  }
  ast->nodes = NULL;
  ast->capacity = 0;
  ast->size = 1;

  return ast;
}
void ast_destroy(ast_t *ctx) {
  if (ctx) {
    if (ctx->nodes) {
      free(ctx->nodes);
    }
    free(ctx);
  }
}

node_id ast_create_node(ast_t *ctx, node_t node) {
  if (ctx->size == ctx->capacity || ctx->nodes == NULL) {
    ctx->capacity = ctx->capacity ? ctx->capacity * 2 : 8;
    ctx->nodes = realloc(ctx->nodes, ctx->capacity * sizeof *ctx->nodes);
    if (!ctx->nodes) {
      perror("Error allocating memory");
      return 0;
    }
  }
  ctx->nodes[ctx->size] = node;
  return ctx->size++;
}

node_id ast_make_binary_expr(ast_t *ast, operation_e op, node_id left,
                             node_id right) {
  node_t node = {0};
  node.type = NODE_BINARY_EXPR;
  node.as.binary_expr.op = op;
  node.as.binary_expr.left = left;
  node.as.binary_expr.right = right;

  return ast_create_node(ast, node);
}

node_id ast_make_unary_expr(ast_t *ast, operation_e op, node_id operand) {
  node_t node = {0};
  node.type = NODE_UNARY_EXPR;
  node.as.unary_expr.op = op;
  node.as.unary_expr.operand = operand;

  return ast_create_node(ast, node);
}

node_id ast_make_literal_expr(ast_t *ast, int32_t value) {
  node_t node = {0};
  node.type = NODE_LITERAL_EXPR;
  node.as.literal_expr.value = value;

  return ast_create_node(ast, node);
}

node_id ast_make_identifier_expr(ast_t *ast, str_slice_t name) {
  node_t node = {0};
  node.type = NODE_IDENTIFIER_EXPR;
  node.as.identifier_expr.name = name;

  return ast_create_node(ast, node);
}

node_id ast_make_assign_expr(ast_t *ast, node_id target, node_id value) {
  node_t node = {0};
  node.type = NODE_ASSIGN_EXPR;
  node.as.assign_expr.target = target;
  node.as.assign_expr.value = value;

  return ast_create_node(ast, node);
}

node_id ast_make_call_expr(ast_t *ast, node_id callee, node_id *args,
                           uint32_t arg_count) {
  node_t node = {0};
  node.type = NODE_CALL_EXPR;
  node.as.call_expr.callee = callee;
  node.as.call_expr.args = args;
  node.as.call_expr.arg_count = arg_count;

  return ast_create_node(ast, node);
}

node_id ast_make_expr_stmt(ast_t *ast, node_id expression) {
  node_t node = {0};
  node.type = NODE_EXPR_STMT;
  node.as.expr_stmt.expression = expression;

  return ast_create_node(ast, node);
}

node_id ast_make_var_decl_stmt(ast_t *ast, value_type_e type, str_slice_t name,
                               node_id initializer) {
  node_t node = {0};
  node.type = NODE_VAR_DECL_STMT;
  node.as.var_decl_stmt.type = type;
  node.as.var_decl_stmt.name = name;
  node.as.var_decl_stmt.initializer = initializer;

  return ast_create_node(ast, node);
}

node_id ast_make_block_stmt(ast_t *ast, node_id *statements,
                            uint32_t stmt_count) {
  node_t node = {0};
  node.type = NODE_BLOCK_STMT;
  node.as.block_stmt.statements = statements;
  node.as.block_stmt.stmt_count = stmt_count;

  return ast_create_node(ast, node);
}

node_id ast_make_if_stmt(ast_t *ast, node_id condition, node_id then_branch,
                         node_id else_branch) {
  node_t node = {0};
  node.type = NODE_IF_STMT;
  node.as.if_stmt.condition = condition;
  node.as.if_stmt.then_branch = then_branch;
  node.as.if_stmt.else_branch = else_branch;

  return ast_create_node(ast, node);
}

node_id ast_make_while_stmt(ast_t *ast, node_id condition, node_id body) {
  node_t node = {0};
  node.type = NODE_WHILE_STMT;
  node.as.while_stmt.condition = condition;
  node.as.while_stmt.body = body;

  return ast_create_node(ast, node);
}

node_id ast_make_return_stmt(ast_t *ast, node_id expression) {
  node_t node = {0};
  node.type = NODE_RETURN_STMT;
  node.as.return_stmt.expression = expression;

  return ast_create_node(ast, node);
}

node_id ast_make_function(ast_t *ast, value_type_e return_type,
                          str_slice_t name, node_id *params,
                          uint32_t param_count, node_id body) {
  node_t node = {0};
  node.type = NODE_FUNCTION;
  node.as.function.return_type = return_type;
  node.as.function.name = name;
  node.as.function.params = params;
  node.as.function.param_count = param_count;
  node.as.function.body = body;

  return ast_create_node(ast, node);
}

void program_append_statement(program_t *program, node_id statement) {
  assert(program && program->statements);
  if (program->size == program->reserved) {
    program->reserved *= 2;
    program->statements = realloc(program->statements, program->reserved);
  }
  program->statements[program->size++] = statement;
}
