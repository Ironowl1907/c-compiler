#ifndef AST_H
#define AST_H

#include "str_slice.h"
#include <stdint.h>

typedef uint32_t node_id;

typedef enum {
  // ---- Expressions ----
  NODE_BINARY_EXPR,
  NODE_UNARY_EXPR,
  NODE_LITERAL_EXPR,
  NODE_IDENTIFIER_EXPR,
  NODE_ASSIGN_EXPR,
  NODE_CALL_EXPR,

  // ---- Statements ----
  NODE_EXPR_STMT,
  NODE_VAR_DECL_STMT,
  NODE_BLOCK_STMT,
  NODE_IF_STMT,
  NODE_WHILE_STMT,
  NODE_RETURN_STMT,

  // ---- Top Level ----
  NODE_FUNCTION,
} node_type_e;

typedef enum {
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_NEG,
  OP_EQUAL,
  OP_NOT_EQUAL,
  OP_LESS,
  OP_GREATER,
} operation_e;

typedef enum {
  TYPE_INT,
  TYPE_VOID,
} value_type_e;

typedef struct {
  operation_e op;
  node_id left;
  node_id right;
} binary_expr_t;

typedef struct {
  operation_e op;
  node_id operand;
} unary_expr_t;

typedef struct {
  int32_t value;
} literal_expr_t;

typedef struct {
  str_slice_t name;
} identifier_expr_t;

typedef struct {
  node_id target; // identifier
  node_id value;  // expression
} assign_expr_t;

typedef struct {
  node_id callee;
  node_id *args;
  uint32_t arg_count;
} call_expr_t;

//
// ============================================================
// ===================== STATEMENTS ============================
// ============================================================
//

typedef struct {
  node_id expression;
} expr_stmt_t;

typedef struct {
  value_type_e type;
  str_slice_t name;
  node_id initializer; // 0 if none
} var_decl_stmt_t;

typedef struct {
  node_id *statements;
  uint32_t stmt_count;
} block_stmt_t;

typedef struct {
  node_id condition;
  node_id then_branch;
  node_id else_branch; // 0 if none
} if_stmt_t;

typedef struct {
  node_id condition;
  node_id body;
} while_stmt_t;

typedef struct {
  node_id expression; // 0 if `return;`
} return_stmt_t;

//
// ============================================================
// ===================== FUNCTION ==============================
// ============================================================
//

typedef struct {
  value_type_e return_type;
  str_slice_t name;

  node_id *params; // var_decl_stmt_t nodes
  uint32_t param_count;

  node_id body; // block_stmt_t
} function_t;

//
// ============================================================
// ===================== GENERIC NODE AND PROGRAM ==============
// ============================================================
//

typedef struct {
  node_type_e type;

  union {

    // Expressions
    binary_expr_t binary_expr;
    unary_expr_t unary_expr;
    literal_expr_t literal_expr;
    identifier_expr_t identifier_expr;
    assign_expr_t assign_expr;
    call_expr_t call_expr;

    // Statements
    expr_stmt_t expr_stmt;
    var_decl_stmt_t var_decl_stmt;
    block_stmt_t block_stmt;
    if_stmt_t if_stmt;
    while_stmt_t while_stmt;
    return_stmt_t return_stmt;

    // Top level
    function_t function;

  } as;

} node_t;

// ============================================================
// ===================== Program ===============================
// ============================================================

typedef struct {
  node_id *statements;
  uint32_t size;
  uint32_t reserved;
} program_t;

void program_append_statement(program_t *program, node_id statement);

// ============================================================
// ===================== AST ARENA =============================
// ============================================================

typedef struct {
  node_t *nodes;
  uint32_t size;
  uint32_t capacity;
} ast_t;

ast_t *ast_create(void);
void ast_destroy(ast_t *ast);

node_id ast_create_node(ast_t *ast, node_t node);

// ===============================================
// =============== Constructors ===================
// ===============================================

node_id ast_make_binary_expr(ast_t *ast, operation_e op, node_id left,
                             node_id right);

node_id ast_make_unary_expr(ast_t *ast, operation_e op, node_id operand);

node_id ast_make_literal_expr(ast_t *ast, int32_t value);

node_id ast_make_identifier_expr(ast_t *ast, str_slice_t name);

node_id ast_make_assign_expr(ast_t *ast, node_id target, node_id value);

node_id ast_make_call_expr(ast_t *ast, node_id callee, node_id *args,
                           uint32_t arg_count);

node_id ast_make_expr_stmt(ast_t *ast, node_id expression);

node_id ast_make_var_decl_stmt(ast_t *ast, value_type_e type, str_slice_t name,
                               node_id initializer);

node_id ast_make_block_stmt(ast_t *ast, node_id *statements,
                            uint32_t stmt_count);

node_id ast_make_if_stmt(ast_t *ast, node_id condition, node_id then_branch,
                         node_id else_branch);

node_id ast_make_while_stmt(ast_t *ast, node_id condition, node_id body);

node_id ast_make_return_stmt(ast_t *ast, node_id expression);

node_id ast_make_function(ast_t *ast, value_type_e return_type,
                          str_slice_t name, node_id *params,
                          uint32_t param_count, node_id body);

// ==================================================
// =================== Debug =========================
// ==================================================

void ast_debug_print(ast_t *ast);

#endif
