#ifndef STR_SLICE_H
#define STR_SLICE_H

#include <stdint.h>

typedef struct {
  char *start;
  uint32_t size;
} str_slice_t;

#endif
