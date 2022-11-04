#ifndef KM2_BACKEND_CONSTANTS_CAPABILITY_H
#define KM2_BACKEND_CONSTANTS_CAPABILITY_H

#include "abstract_capability.h"
#include <memory>

namespace km2 {
namespace backend {

class value;

class constants_capability : public abstract_capability {
public:
    constants_capability(class unit* unit);

    virtual value* uint32(std::uint32_t value) = 0;
    virtual value* uintptr(std::uintptr_t value) = 0;
    virtual value* uint(std::uint64_t value, std::size_t size) = 0;
    virtual value* float64(double value) = 0;
    virtual value* string_cstr(const std::string &name, const std::string &text) = 0;
};

}
}
#endif // KM2_BACKEND_CONSTANTS_CAPABILITY_H
