#ifndef KM2_BACKEND_TYPE_H
#define KM2_BACKEND_TYPE_H

#include <cstdint>
#include <wall_e/src/utility/collections.h>
#include "../as.h"
#include "printable.h"

namespace km2 {
namespace backend {

class type : public printable {
    km2_backend_decl_as
public:
    virtual bool eq(const type* other) const = 0;
    inline bool no_eq(const type* other) const { return !eq(other); }
};

}
}

#endif // TYPE_H
