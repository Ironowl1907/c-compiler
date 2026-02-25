#include "ast.h"
#include <stdio.h>

// ============================================================
// ===================== Helpers ===============================
// ============================================================

static const char *node_type_to_string(node_type_e type) {
  switch (type) {
  case NODE_BINARY_EXPR:
    return "BINARY_EXPR";
  case NODE_UNARY_EXPR:
    return "UNARY_EXPR";
  case NODE_LITERAL_EXPR:
    return "LITERAL_EXPR";
  case NODE_IDENTIFIER_EXPR:
    return "IDENTIFIER_EXPR";
  case NODE_ASSIGN_EXPR:
    return "ASSIGN_EXPR";
  case NODE_CALL_EXPR:
    return "CALL_EXPR";

  case NODE_EXPR_STMT:
    return "EXPR_STMT";
  case NODE_VAR_DECL_STMT:
    return "VAR_DECL_STMT";
  case NODE_BLOCK_STMT:
    return "BLOCK_STMT";
  case NODE_IF_STMT:
    return "IF_STMT";
  case NODE_WHILE_STMT:
    return "WHILE_STMT";
  case NODE_RETURN_STMT:
    return "RETURN_STMT";

  case NODE_FUNCTION:
    return "FUNCTION";
  }

  return "UNKNOWN";
}

static const char *op_to_string(operation_e op) {
  switch (op) {
  case OP_ADD:
    return "+";
  case OP_SUB:
    return "-";
  case OP_MUL:
    return "*";
  case OP_DIV:
    return "/";
  case OP_NEG:
    return "NEG";
  case OP_EQUAL:
    return "==";
  case OP_NOT_EQUAL:
    return "!=";
  case OP_LESS:
    return "<";
  case OP_GREATER:
    return ">";
  }

  return "UNKNOWN_OP";
}

static const char *type_to_string(value_type_e type) {
  switch (type) {
  case TYPE_INT:
    return "int";
  case TYPE_VOID:
    return "void";
  }
  return "unknown";
}

static void print_indent(int indent) {
  for (int i = 0; i < indent; ++i)
    printf("  ");
}

static void print_slice(str_slice_t s) { printf("%.*s", (int)s.size, s.start); }

// ============================================================
// ===================== Recursive Printer =====================
// ============================================================

static void ast_debug_print_node(ast_t *ast, node_id id, int indent) {
  if (id == 0 || id >= ast->size)
    return;

  node_t *node = &ast->nodes[id];

  print_indent(indent);
  printf("%s\n", node_type_to_string(node->type));

  switch (node->type) {

    // ================= Expressions =================

  case NODE_BINARY_EXPR: {
    print_indent(indent + 1);
    printf("op: %s\n", op_to_string(node->as.binary_expr.op));

    ast_debug_print_node(ast, node->as.binary_expr.left, indent + 1);
    ast_debug_print_node(ast, node->as.binary_expr.right, indent + 1);
    break;
  }

  case NODE_UNARY_EXPR: {
    print_indent(indent + 1);
    printf("op: %s\n", op_to_string(node->as.unary_expr.op));

    ast_debug_print_node(ast, node->as.unary_expr.operand, indent + 1);
    break;
  }

  case NODE_LITERAL_EXPR: {
    print_indent(indent + 1);
    printf("value: %d\n", node->as.literal_expr.value);
    break;
  }

  case NODE_IDENTIFIER_EXPR: {
    print_indent(indent + 1);
    printf("name: ");
    print_slice(node->as.identifier_expr.name);
    printf("\n");
    break;
  }

  case NODE_ASSIGN_EXPR: {
    print_indent(indent + 1);
    printf("target:\n");
    ast_debug_print_node(ast, node->as.assign_expr.target, indent + 2);

    print_indent(indent + 1);
    printf("value:\n");
    ast_debug_print_node(ast, node->as.assign_expr.value, indent + 2);
    break;
  }

  case NODE_CALL_EXPR: {
    print_indent(indent + 1);
    printf("callee:\n");
    ast_debug_print_node(ast, node->as.call_expr.callee, indent + 2);

    print_indent(indent + 1);
    printf("args (%u):\n", node->as.call_expr.arg_count);
    for (uint32_t i = 0; i < node->as.call_expr.arg_count; ++i) {
      ast_debug_print_node(ast, node->as.call_expr.args[i], indent + 2);
    }
    break;
  }

    // ================= Statements =================

  case NODE_EXPR_STMT: {
    ast_debug_print_node(ast, node->as.expr_stmt.expression, indent + 1);
    break;
  }

  case NODE_VAR_DECL_STMT: {
    print_indent(indent + 1);
    printf("type: %s\n", type_to_string(node->as.var_decl_stmt.type));

    print_indent(indent + 1);
    printf("name: ");
    print_slice(node->as.var_decl_stmt.name);
    printf("\n");

    if (node->as.var_decl_stmt.initializer) {
      print_indent(indent + 1);
      printf("initializer:\n");
      ast_debug_print_node(ast, node->as.var_decl_stmt.initializer, indent + 2);
    }
    break;
  }

  case NODE_BLOCK_STMT: {
    for (uint32_t i = 0; i < node->as.block_stmt.stmt_count; ++i) {
      ast_debug_print_node(ast, node->as.block_stmt.statements[i], indent + 1);
    }
    break;
  }

  case NODE_IF_STMT: {
    print_indent(indent + 1);
    printf("condition:\n");
    ast_debug_print_node(ast, node->as.if_stmt.condition, indent + 2);

    print_indent(indent + 1);
    printf("then:\n");
    ast_debug_print_node(ast, node->as.if_stmt.then_branch, indent + 2);

    if (node->as.if_stmt.else_branch) {
      print_indent(indent + 1);
      printf("else:\n");
      ast_debug_print_node(ast, node->as.if_stmt.else_branch, indent + 2);
    }
    break;
  }

  case NODE_WHILE_STMT: {
    print_indent(indent + 1);
    printf("condition:\n");
    ast_debug_print_node(ast, node->as.while_stmt.condition, indent + 2);

    print_indent(indent + 1);
    printf("body:\n");
    ast_debug_print_node(ast, node->as.while_stmt.body, indent + 2);
    break;
  }

  case NODE_RETURN_STMT: {
    if (node->as.return_stmt.expression) {
      ast_debug_print_node(ast, node->as.return_stmt.expression, indent + 1);
    }
    break;
  }

    // ================= Function =================

  case NODE_FUNCTION: {
    print_indent(indent + 1);
    printf("return_type: %s\n", type_to_string(node->as.function.return_type));

    print_indent(indent + 1);
    printf("name: ");
    print_slice(node->as.function.name);
    printf("\n");

    print_indent(indent + 1);
    printf("params (%u):\n", node->as.function.param_count);
    for (uint32_t i = 0; i < node->as.function.param_count; ++i) {
      ast_debug_print_node(ast, node->as.function.params[i], indent + 2);
    }

    print_indent(indent + 1);
    printf("body:\n");
    ast_debug_print_node(ast, node->as.function.body, indent + 2);
    break;
  }
  }
}

// ============================================================
// ===================== Public API ============================
// ============================================================

void ast_debug_print(ast_t *ast) {
  if (!ast || ast->size == 0)
    return;

  printf("==== AST DEBUG PRINT ====\n");

  for (uint32_t i = 1; i < ast->size; ++i) {
    ast_debug_print_node(ast, i, 0);
  }

  printf("=========================\n");
}
