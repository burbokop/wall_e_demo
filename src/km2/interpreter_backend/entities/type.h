#ifndef KM2_INTERPRETER_BACKEND_TYPE_H
#define KM2_INTERPRETER_BACKEND_TYPE_H

#include "../../backend/entities/type.h"
#include <memory>
#include <wall_e/src/enums.h>

namespace km2 {
namespace interpreter_backend {


class type : public km2::backend::type {
public:
    wall_e_enum_member(id,
        Unit,
        Int,
        UInt,
        Float,
        Ptr,
        CStr,
        Function
    );
private:
    id m_id;
    std::size_t m_bits;
public:
    type(id id, std::size_t bits) : m_id(id), m_bits(bits) {}
    inline static std::unique_ptr<km2::interpreter_backend::type> make_uniq(id id, std::size_t bits) { return std::make_unique<km2::interpreter_backend::type>(id, bits); }

    inline static km2::backend::type* upcast(type* t) { return dynamic_cast<km2::backend::type*>(t); }
    inline static type* downcast(km2::backend::type* t) { return dynamic_cast<type*>(t); }

    // type interface
public:
    virtual bool eq(const km2::backend::type *other) const override;

    // printable interface
public:
    virtual std::ostream &print(std::ostream &) const override;
};

}
}

#endif // TYPE_H
