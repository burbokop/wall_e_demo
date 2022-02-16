#ifndef TYPE_CAPABILITY_H
#define TYPE_CAPABILITY_H

#include "abstract_capability.h"

#include <llvm/IR/DerivedTypes.h>

namespace km2 {

class type_capability : public abstract_capability {
public:
    type_capability(translation_unit* unit);

    llvm::IntegerType *int32() const;
    llvm::Type *float32() const;
    llvm::Type *float64() const;
    llvm::IntegerType *custom_int(std::size_t bits) const;
    llvm::Type *void_type() const;

    llvm::PointerType *cstr() const;
};

}
#endif // TYPE_CAPABILITY_H
