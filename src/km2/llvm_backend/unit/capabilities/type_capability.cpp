#include "type_capability.h"

#include "../unit.h"
#include "../private.unit.h"
#include "../../entities/type.h"
#include <llvm/IR/DerivedTypes.h>

km2::llvm_backend::type *km2::llvm_backend::type_capability::born_type(llvm::Type *data) {
    m_types.push_back(llvm_backend::type::make_uniq(data));
    return m_types.back().get();
}

km2::llvm_backend::type_capability::type_capability(km2::backend::unit *unit)
    : km2::backend::type_capability(unit) {}

km2::backend::type* km2::llvm_backend::type_capability::int32() {
    return born_type(static_cast<llvm::Type*>(llvm::IntegerType::getInt32Ty(*unit()->as<class unit>()->p()->context)));
}

km2::backend::type* km2::llvm_backend::type_capability::float32() {
    return born_type(llvm::Type::getFloatTy(*unit()->as<class unit>()->p()->context));
}

km2::backend::type* km2::llvm_backend::type_capability::float64() {
    return born_type(llvm::Type::getDoubleTy(*unit()->as<class unit>()->p()->context));
}

km2::backend::type* km2::llvm_backend::type_capability::custom_int(std::size_t bits) {
    return born_type(llvm::Type::getIntNTy(*unit()->as<class unit>()->p()->context, bits));
}

km2::backend::type* km2::llvm_backend::type_capability::void_type() {
    return born_type(llvm::Type::getVoidTy(*unit()->as<class unit>()->p()->context));
}

km2::backend::type* km2::llvm_backend::type_capability::cstr() {
    return born_type(llvm::PointerType::get(llvm::Type::getInt8Ty(*unit()->as<class unit>()->p()->context), 0));
}

km2::backend::type *km2::llvm_backend::type_capability::func_type(backend::type *return_type, wall_e::vec<backend::type *> arg_types) {
    wall_e::vec<llvm::Type*> p;
    p.reserve(arg_types.size());
    for(const auto& at : arg_types) {
         p.push_back(at->as<llvm_backend::type>()->data());
    }
    return born_type(llvm::FunctionType::get(return_type->as<llvm_backend::type>()->data(), llvm::ArrayRef<llvm::Type*>(p), false));
}
