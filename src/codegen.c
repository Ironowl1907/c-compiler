#include "codegen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Transforms/PassBuilder.h>

static int initialize_llvm_targets(void) {
  if (LLVMInitializeNativeTarget() != 0)
    return 1;

  if (LLVMInitializeNativeAsmPrinter() != 0)
    return 1;

  if (LLVMInitializeNativeAsmParser() != 0)
    return 1;

  return 0;
}

codegen_context_t *codegen_create(const char *module_name,
                                  codegen_error_t *err) {
  if (!module_name) {
    if (err)
      *err = CODEGEN_ERROR_NULL_ARGUMENT;
    return NULL;
  }

  if (!initialize_llvm_targets()) {
    if (err)
      *err = CODEGEN_ERROR_LLVM_INIT_FAILED;
    return NULL;
  }

  codegen_context_t *ctx = calloc(1, sizeof(*ctx));
  if (!ctx)
    return NULL;

  if (LLVMInitializeNativeTarget() || LLVMInitializeNativeAsmPrinter() ||
      LLVMInitializeNativeAsmParser()) {
    *err = CODEGEN_ERROR_LLVM_INIT_FAILED;
    return ctx;
  }

  ctx->context = LLVMContextCreate();
  ctx->module = LLVMModuleCreateWithNameInContext(module_name, ctx->context);
  ctx->builder = LLVMCreateBuilderInContext(ctx->context);

  if (err)
    *err = CODEGEN_OK;
  return ctx;
}

void codegen_destroy(codegen_context_t *ctx) {
  if (!ctx)
    return;

  if (ctx->target_machine)
    LLVMDisposeTargetMachine(ctx->target_machine);

  if (ctx->builder)
    LLVMDisposeBuilder(ctx->builder);

  if (ctx->module)
    LLVMDisposeModule(ctx->module);

  if (ctx->context)
    LLVMContextDispose(ctx->context);

  free(ctx);
}

LLVMContextRef codegen_get_llvm_context(codegen_context_t *ctx) {
  return ctx ? ctx->context : NULL;
}

LLVMModuleRef codegen_get_module(codegen_context_t *ctx) {
  return ctx ? ctx->module : NULL;
}

LLVMBuilderRef codegen_get_builder(codegen_context_t *ctx) {
  return ctx ? ctx->builder : NULL;
}

int codegen_verify_module(codegen_context_t *ctx) {
  if (!ctx)
    return 1;

  char *error = NULL;

  LLVMBool failed =
      LLVMVerifyModule(ctx->module, LLVMReturnStatusAction, &error);

  if (failed) {
    fprintf(stderr, "LLVM Verification Error:\n%s\n", error);
    LLVMDisposeMessage(error);
    return 1;
  }

  return 0;
}
