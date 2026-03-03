#ifndef CODEGEN_H
#define CODEGEN_H

#include <stddef.h>

#include <llvm-c/Core.h>
#include <llvm-c/TargetMachine.h>

typedef struct codegen_context {
  LLVMContextRef context;
  LLVMModuleRef module;
  LLVMBuilderRef builder;

  LLVMTargetMachineRef target_machine;
} codegen_context_t;

/*
 * Error handling
 */
typedef enum {
  CODEGEN_OK = 0,
  CODEGEN_ERROR_NULL_ARGUMENT,
  CODEGEN_ERROR_LLVM_INIT_FAILED,
  CODEGEN_ERROR_MODULE_VERIFICATION_FAILED,
  CODEGEN_ERROR_TARGET_INIT_FAILED,
  CODEGEN_ERROR_EMIT_FAILED
} codegen_error_t;

codegen_context_t *codegen_create(const char *module_name,
                                  codegen_error_t *err);

void codegen_destroy(codegen_context_t *ctx);

LLVMContextRef codegen_get_llvm_context(codegen_context_t *ctx);

LLVMModuleRef codegen_get_module(codegen_context_t *ctx);

LLVMBuilderRef codegen_get_builder(codegen_context_t *ctx);

int codegen_verify_module(codegen_context_t *ctx);


#endif
