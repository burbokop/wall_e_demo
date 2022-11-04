#include "constants_capability.h"

#include "../unit.h"
#include "../private.unit.h"
#include "../../entities/constant.h"
#include "../../entities/type.h"
#include "type_capability.h"
#include "../../instructions/instruction.h"

km2::interpreter_backend::constant *km2::interpreter_backend::constants_capability::born_constant(
        type* data,
        const std::shared_ptr<dyn_value>& dyn
        ) {
    return m_constants.make(data, dyn);
}

km2::interpreter_backend::constants_capability::constants_capability(backend::unit *unit)
    : backend::constants_capability(unit) {}

km2::backend::value *km2::interpreter_backend::constants_capability::uint32(uint32_t value) {
    return born_constant(
                unit()->cap<km2::backend::type_capability>()->int32()->as<type>(),
                dyn_value::from_variant(value)
                );
}

km2::backend::value *km2::interpreter_backend::constants_capability::uintptr(uintptr_t value) {
    return born_constant(
                unit()->cap<km2::backend::type_capability>()->ptr()->as<type>(),
                dyn_value::from_variant(value)
                );
}

km2::backend::value *km2::interpreter_backend::constants_capability::uint(uint64_t value, size_t size) {
    return born_constant(
                unit()->cap<km2::backend::type_capability>()->custom_int(size)->as<type>(),
                dyn_value::from_variant(value)
                );
}

km2::backend::value *km2::interpreter_backend::constants_capability::float64(double value) {
    return born_constant(
                unit()->cap<km2::backend::type_capability>()->float64()->as<type>(),
                dyn_value::from_variant(value)
                );
}

km2::backend::value *km2::interpreter_backend::constants_capability::string_cstr(const std::string &name, const std::string &text) {
    return born_constant(
                unit()->cap<km2::backend::type_capability>()->cstr()->as<type>(),
                dyn_value::from_variant(text)
                );
}
