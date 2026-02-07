#include "lexer.h"
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
	printf("%s", lexer->data);

  // Cleanup
  lexer_delete(lexer);

  return 0;
}
