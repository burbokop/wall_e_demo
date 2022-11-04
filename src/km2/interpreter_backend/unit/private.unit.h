#ifndef PRIVATE_KM2_LLVM_BACKEND_MODULE_H
#define PRIVATE_KM2_LLVM_BACKEND_MODULE_H

#ifdef KM2_LLVM_BACKEND_TARGET

#include <wall_e/src/utility/collections.h>
#include <memory>
#include <stack>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"

namespace km2 {
namespace backend {
class value;
class type;
class argument;
}
}

namespace km2 {
namespace llvm_backend {

class block;
class function;

struct unit_private {
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;

    wall_e::vec<std::unique_ptr<llvm_backend::block>> blocks;
    wall_e::vec<std::unique_ptr<llvm_backend::function>> functions;

    /**
     * @brief born_blocks - create block and store it to `blocks` uniq ptr vec so that its timeline be as `unit_private` obj
     */
    llvm_backend::block* born_block(llvm::BasicBlock* data);
    llvm_backend::function* born_function(llvm::Function* data);

    static llvm::ArrayRef<llvm::Type*> types_vec_data(const wall_e::vec<backend::type*>& types);

    struct ctx {
        llvm_backend::block* block;
        std::map<std::string, backend::value*> args;
    };
    std::stack<ctx> stack;
};


}
}

#endif

#endif // PRIVATE_KM2_LLVM_BACKEND_MODULE_H
