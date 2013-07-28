#include <stdio.h>

#include <llvm/BasicBlock.h>
#include <llvm/Constants.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Function.h>
#include <llvm/Instruction.h>
#include <llvm/Instructions.h>
#include <llvm/Pass.h>
#include <llvm/Type.h>

#define VERBOSE
#if defined(VERBOSE)
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

#if defined(__x86_64__)
#define PTR_BITS 64
#else
#define PTR_BITS 32
#endif

using namespace llvm;
using namespace std;

class ClearStack : public FunctionPass {
public:
  ClearStack() : FunctionPass(ID) {}

  virtual bool runOnFunction(Function& func) {
    LOG("func %s\n", func.getName().data());
    vector<AllocaInst*> allocas;
    for (BasicBlock& bb : func) {
      for (Instruction& inst : bb) {
        if (inst.getOpcode() == Instruction::Alloca) {
          AllocaInst& alloca = dynamic_cast<AllocaInst&>(inst);
          if (!alloca.isStaticAlloca())
            continue;

          Type* type = alloca.getAllocatedType();
          LOG("%u %s %d align=%u int=%d arr=%d prim=%u sca=%u %s\n",
              alloca.getOpcode(), alloca.getOpcodeName(),
              alloca.isStaticAlloca(), alloca.getAlignment(),
              type->isIntegerTy(),
              type->isArrayTy(),
              type->getPrimitiveSizeInBits(),
              type->getScalarSizeInBits(),
              alloca.getName().data());
          if ((type->isIntegerTy() &&
               type->getPrimitiveSizeInBits() == PTR_BITS) ||
              type->isPointerTy())
            allocas.push_back(&alloca);
        } else if (inst.getOpcode() == Instruction::Ret) {
          ReturnInst& ret = dynamic_cast<ReturnInst&>(inst);
          LOG("ret: %zu\n", allocas.size());
          for (AllocaInst* alloca : allocas) {
            Constant* zero;
            if (alloca->getAllocatedType()->isIntegerTy())
              zero = ConstantInt::get(alloca->getAllocatedType(), 0);
            else
              zero = ConstantPointerNull::get(
                static_cast<PointerType*>(alloca->getAllocatedType()));
            LOG("fill zero %p => %s\n", zero, alloca->getName().data());
            new StoreInst(zero, alloca, true, &ret);
          }
        }
      }
    }
    return false;
  }

  static char ID;
};

char ClearStack::ID = 'a';
static RegisterPass<ClearStack> g_cs(
  "clearstack", "clear stack", false, false);
