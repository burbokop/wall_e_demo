#ifndef KM2_INTERPRETER_BACKEND_CONSTANTS_CAPABILITY_H
#define KM2_INTERPRETER_BACKEND_CONSTANTS_CAPABILITY_H

#include "../../../backend/unit/capabilities/constants_capability.h"
#include <wall_e/src/utility/collections.h>
#include <wall_e/src/utility/box.h>


namespace km2 {
namespace interpreter_backend {

class value;
class type;
class dyn_value;
class constant;

class constants_capability : public km2::backend::constants_capability {
    wall_e::box_list<constant> m_constants;
    constant* born_constant(type* data, const std::shared_ptr<dyn_value>& dyn);
public:
    constants_capability(km2::backend::unit* unit);

    virtual km2::backend::value *uint32(std::uint32_t value) override;
    virtual km2::backend::value *uintptr(std::uintptr_t value) override;
    virtual km2::backend::value *uint(std::uint64_t value, std::size_t size) override;
    virtual km2::backend::value *float64(double value) override;
    virtual km2::backend::value *string_cstr(const std::string &name, const std::string &text) override;
};

}
}
#endif // CONSTANTS_CAPABILITY_H
