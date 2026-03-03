#include "ast.h"
#include "codegen.h"
#include "lexer.h"
#include "parser.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  // Argument parsing
  if (argc < 2) {
    printf("Expected file path\n");
    return 1;
  }

  FILE *file = fopen(argv[1], "r");
  if (!file) {
    perror("Error opening file");
    return 1;
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);

  char *buffer = (char *)malloc(file_size + 1);
  if (!buffer) {
    perror("Error allocating memory");
    fclose(file);
    return 1;
  }

  fread(buffer, 1, file_size, file);
  buffer[file_size] = '\0';
  fclose(file);

  // Create lexer
  lexer_t *lexer = lexer_create(buffer);

  lexer_lex(lexer);

  // Debug print for lexer output if debug build
#ifndef NDEBUG
  lexer_debug_print_tokens(lexer);
#endif

  // Create ast
  ast_t *ast = ast_create();

  // Craete parser
  parser_t *parser = parser_create(ast, lexer);
  node_id program = parser_parse(parser);

  // Debug print for ast if debug build
#ifndef NDEBUG
  ast_debug_print(ast);
#endif

  // Code generation
  codegen_error_t err;
  codegen_context_t *codegen = codegen_create("ccodegeneration", &err);
  if (err != CODEGEN_OK) {
    printf("[Codegen Error]: Error on creation,err id: %d", err);
  }

  // Cleanup
  lexer_delete(lexer);

  return 0;
}
