#ifndef CONSTANTS_CAPABILITY_H
#define CONSTANTS_CAPABILITY_H

#include "abstract_capability.h"

#include <llvm/IR/Constants.h>

namespace km2 {


class constants_capability : public abstract_capability {
public:
    constants_capability(translation_unit* unit);

    llvm::ConstantInt *uint32(std::uint32_t value);
    llvm::ConstantInt *uintptr(std::uintptr_t value);
    llvm::ConstantInt *uint(std::uint64_t value, std::size_t size);
    llvm::Constant *float64(double value);

    llvm::Value *string_cstr(const std::string &name, const std::string &text);
};

}
#endif // CONSTANTS_CAPABILITY_H
