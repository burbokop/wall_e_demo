#include "type_capability.h"

#include "../translation_unit.h"

km2::type_capability::type_capability(translation_unit *unit)
    : abstract_capability(unit) {}

llvm::IntegerType* km2::type_capability::int32() const {
    return llvm::IntegerType::getInt32Ty(*unit()->llvm_context());
}

llvm::Type *km2::type_capability::float32() const {
    return llvm::Type::getFloatTy(*unit()->llvm_context());
}

llvm::Type *km2::type_capability::float64() const {
    return llvm::Type::getDoubleTy(*unit()->llvm_context());
}

llvm::IntegerType *km2::type_capability::custom_int(std::size_t bits) const {
    return llvm::Type::getIntNTy(*unit()->llvm_context(), bits);
}

llvm::Type* km2::type_capability::void_type() const {
    return llvm::Type::getVoidTy(*unit()->llvm_context());
}

llvm::PointerType *km2::type_capability::cstr() const {
    return llvm::PointerType::get(llvm::Type::getInt8Ty(*unit()->llvm_context()), 0);
}
