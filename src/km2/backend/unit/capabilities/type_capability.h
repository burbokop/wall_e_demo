#ifndef KM2_BACKEND_TYPE_CAPABILITY_H
#define KM2_BACKEND_TYPE_CAPABILITY_H

#include "abstract_capability.h"
#include <memory>
#include <wall_e/src/utility/collections.h>

namespace km2 {
namespace backend {

class type;

class type_capability : public abstract_capability {
public:
    type_capability(class unit* unit);

    virtual type* int32() = 0;
    virtual type* float32() = 0;
    virtual type* float64() = 0;
    virtual type* custom_int(std::size_t bits) = 0;
    virtual type* void_type() = 0;
    virtual type* cstr() = 0;
    virtual type* ptr() = 0;
    virtual type* func_type(type* return_type, wall_e::vec<type*> arg_types) = 0;
};

}
}
#endif // TYPE_CAPABILITY_H
