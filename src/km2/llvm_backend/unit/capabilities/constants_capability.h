#ifndef KM2_LLVM_BACKEND_CONSTANTS_CAPABILITY_H
#define KM2_LLVM_BACKEND_CONSTANTS_CAPABILITY_H

#include "../../../backend/unit/capabilities/constants_capability.h"
#include <wall_e/src/utility/collections.h>

namespace llvm {
class Value;
}

namespace km2 {
namespace llvm_backend {

class value;

class constants_capability : public backend::constants_capability {
    wall_e::list<std::unique_ptr<value>> m_values;
    llvm_backend::value* born_value(llvm::Value* data);
public:
    constants_capability(backend::unit* unit);

    virtual backend::value *uint32(std::uint32_t value) override;
    virtual backend::value *uintptr(std::uintptr_t value) override;
    virtual backend::value *uint(std::uint64_t value, std::size_t size) override;
    virtual backend::value *float64(double value) override;
    virtual backend::value *string_cstr(const std::string &name, const std::string &text) override;
};

}
}
#endif // CONSTANTS_CAPABILITY_H
