#include "constants_capability.h"

#include "../translation_unit.h"

km2::constants_capability::constants_capability(translation_unit *unit)
    : abstract_capability(unit) {}


llvm::ConstantInt *km2::constants_capability::uint32(uint32_t value) {
    return llvm::ConstantInt::getSigned((llvm::Type::getInt32Ty(*unit()->llvm_context())), value);
}

llvm::ConstantInt *km2::constants_capability::uintptr(uintptr_t value) {
    return llvm::ConstantInt::getSigned(unit()->llvm_builder()->getIntPtrTy(unit()->llvm_module()->getDataLayout()), value);
}

llvm::ConstantInt *km2::constants_capability::uint(uint64_t value, size_t size) {
    return llvm::ConstantInt::getSigned(unit()->llvm_builder()->getIntNTy(size), value);
}

llvm::Constant *km2::constants_capability::float64(double value) {
    return llvm::ConstantFP::get(llvm::Type::getDoubleTy(*unit()->llvm_context()), value);
}

llvm::Value *km2::constants_capability::string_cstr(const std::string &name, const std::string &text) {
    const auto str = llvm::ConstantDataArray::getString(*unit()->llvm_context(), text, true);

    const auto global_str = new llvm::GlobalVariable(
                *unit()->llvm_module(),
                str->getType(),
                true,
                llvm::GlobalValue::ExternalLinkage,
                str,
                name
                );

    return unit()->llvm_builder()->CreateGEP(global_str, uintptr(0));
}
