#ifndef KM2_BACKEND_ARGUMENT_H
#define KM2_BACKEND_ARGUMENT_H

#include "../as.h"
#include "value.h"

namespace km2 {
namespace backend {

class type;
class function;

class argument : public value {
public:
    argument() {}
    virtual function* func() const = 0;
    virtual std::size_t index() const = 0;
};

}
}

#endif // KM2_BACKEND_ARGUMENT_H
