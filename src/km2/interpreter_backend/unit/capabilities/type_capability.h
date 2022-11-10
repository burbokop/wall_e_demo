#ifndef KM2_INTERPRETER_BACKEND_TYPE_CAPABILITY_H
#define KM2_INTERPRETER_BACKEND_TYPE_CAPABILITY_H

#include "../../../backend/unit/capabilities/type_capability.h"
#include <wall_e/src/utility/box.h>
#include <wall_e/src/utility/collections.h>
#include "../../entities/type.h"

namespace km2 {
namespace interpreter_backend {

class type_capability : public km2::backend::type_capability {
    wall_e::box_list<type> m_types;
    wall_e::box<type> m_void_type;
    type* born_type(type::id id, size_t bits);
public:
    type_capability(km2::backend::unit* unit);

    virtual km2::backend::type* int32() override;
    virtual km2::backend::type* float32() override;
    virtual km2::backend::type* float64() override;
    virtual km2::backend::type* custom_int(std::size_t bits) override;
    virtual km2::backend::type* void_type() override;
    virtual km2::backend::type* cstr() override;
    virtual km2::backend::type *ptr() override;
    virtual km2::backend::type* func_type(km2::backend::type* return_type, wall_e::vec<km2::backend::type*> arg_types) override;
};

}
}
#endif // TYPE_CAPABILITY_H
