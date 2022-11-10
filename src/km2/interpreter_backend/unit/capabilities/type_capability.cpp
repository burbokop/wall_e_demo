#include "type_capability.h"

#include "../unit.h"
#include "../private.unit.h"
#include "../../entities/type.h"

km2::interpreter_backend::type *km2::interpreter_backend::type_capability::born_type(type::id id, size_t bits) {
    return m_types.make(id, bits);
}

km2::interpreter_backend::type_capability::type_capability(km2::backend::unit *unit)
    : km2::backend::type_capability(unit),
      m_void_type(wall_e::box<type>::make(type::Unit, 0)) {}

km2::backend::type* km2::interpreter_backend::type_capability::int32() {
    return born_type(
                type::Int,
                32
                //static_cast<llvm::Type*>(llvm::IntegerType::getInt32Ty(*unit()->as<class unit>()->p()->context))
                );
}

km2::backend::type* km2::interpreter_backend::type_capability::float32() {
    return born_type(
                type::Float,
                32
                //llvm::Type::getFloatTy(*unit()->as<class unit>()->p()->context)
                );
}

km2::backend::type* km2::interpreter_backend::type_capability::float64() {
    return born_type(
                type::Float,
                64
                //llvm::Type::getDoubleTy(*unit()->as<class unit>()->p()->context)
                );
}

km2::backend::type* km2::interpreter_backend::type_capability::custom_int(std::size_t bits) {
    return born_type(
                type::Int,
                bits
                //llvm::Type::getIntNTy(*unit()->as<class unit>()->p()->context, bits)
                );
}

km2::backend::type* km2::interpreter_backend::type_capability::void_type() {
    return m_void_type.get();
}

km2::backend::type* km2::interpreter_backend::type_capability::cstr() {
    return born_type(
                type::CStr,
                64 // TODO set size to architecture size
                //llvm::PointerType::get(llvm::Type::getInt8Ty(*unit()->as<class unit>()->p()->context), 0)
                );
}

km2::backend::type *km2::interpreter_backend::type_capability::func_type(backend::type *return_type, wall_e::vec<backend::type*> arg_types) {
    return nullptr;
    /*
    wall_e::vec<llvm::Type*> p;
    p.reserve(arg_types.size());
    for(const auto& at : arg_types) {
         p.push_back(at->as<interpreter_backend::type>()->data());
    }
    return born_type(llvm::FunctionType::get(return_type->as<interpreter_backend::type>()->data(), llvm::ArrayRef<llvm::Type*>(p), false));
    */
}

km2::backend::type *km2::interpreter_backend::type_capability::ptr() {
    return born_type(
                type::Ptr,
                64 // TODO set size to architecture size
                );
}

