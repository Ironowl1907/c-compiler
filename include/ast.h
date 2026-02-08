#ifndef AST_H
#define AST_H

#include "lexer.h"
#include "str_slice.h"
#include <stdint.h>
typedef uint32_t node_id;

typedef enum {
  NODE_TYPE_BINARY,
  NODE_TYPE_URINARY,
  NODE_TYPE_IDENTIFIER,
  NODE_TYPE_LITTERAL,
  NODE_TYPE_ASSIGMENT,
  // TODO: ADD REMAINING NODES
} node_type_e;

typedef enum {
  OPP_SUM,
  OPP_RES,
  OPP_MUL,
  OPP_DIV,
  OPP_NEG,
} opperation_e;

typedef enum { TYPE_INT } ident_type_e;

typedef struct {
  opperation_e opp;
  node_id l, r;
} node_binary;

typedef struct {
  opperation_e opp;
  node_id node;
} node_urinary;

typedef struct {
  ident_type_e type;
  str_slice_t name;

} node_identifier;

typedef struct {
  node_type_e type;
  union {
    int32_t literal;
    node_binary binary;
    node_urinary urinary;
    node_identifier identifier;
  } as;
} node_t;

typedef struct {
  node_t *arena;
  uint32_t arena_size;
  uint32_t arena_reserved;

} ast_t;

ast_t *ast_create(lexer_t *lexer);
void ast_destroy(ast_t *ctx);

node_id ast_create_node(ast_t *ctx, node_t node);

#endif
