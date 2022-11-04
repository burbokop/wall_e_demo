#include "constants_capability.h"

#include "../unit.h"
#include "../private.unit.h"
#include "../../entities/value.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>

km2::llvm_backend::value *km2::llvm_backend::constants_capability::born_value(llvm::Value *data) {
    m_values.push_back(llvm_backend::value::make_uniq(data));
    return m_values.back().get();
}

km2::llvm_backend::constants_capability::constants_capability(backend::unit *unit)
    : backend::constants_capability(unit) {}

km2::backend::value *km2::llvm_backend::constants_capability::uint32(uint32_t value) {
    return born_value(llvm::ConstantInt::getSigned((llvm::Type::getInt32Ty(*unit()->as<class unit>()->p()->context)), value));
}

km2::backend::value *km2::llvm_backend::constants_capability::uintptr(uintptr_t value) {
    return born_value(llvm::ConstantInt::getSigned(unit()->as<class unit>()->p()->builder->getIntPtrTy(unit()->as<class unit>()->p()->module->getDataLayout()), value));
}

km2::backend::value *km2::llvm_backend::constants_capability::uint(uint64_t value, size_t size) {
    return born_value(llvm::ConstantInt::getSigned(unit()->as<class unit>()->p()->builder->getIntNTy(size), value));
}

km2::backend::value *km2::llvm_backend::constants_capability::float64(double value) {
    return born_value(llvm::ConstantFP::get(llvm::Type::getDoubleTy(*unit()->as<class unit>()->p()->context), value));
}

km2::backend::value *km2::llvm_backend::constants_capability::string_cstr(const std::string &name, const std::string &text) {
    const auto str = llvm::ConstantDataArray::getString(*unit()->as<class unit>()->p()->context, text, true);
    const auto global_str = new llvm::GlobalVariable(
                *unit()->as<class unit>()->p()->module,
                str->getType(),
                true,
                llvm::GlobalValue::ExternalLinkage,
                str,
                name
                );
    return born_value(unit()->as<class unit>()->p()->builder->CreateGEP(global_str, uintptr(0)->as<class value>()->data()));
}
