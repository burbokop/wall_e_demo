#ifndef KM2_BACKEND_VALUE_H
#define KM2_BACKEND_VALUE_H

#include "../as.h"

#include "printable.h"

namespace km2 {
namespace backend {

class type;

class value : public printable {
    km2_backend_decl_as
public:
    value() {}
    virtual type* value_type() const = 0;
};

}
}

#endif // KM2_BACKEND_VALUE_H
