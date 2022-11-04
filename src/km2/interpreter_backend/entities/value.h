#ifndef KM2_INTERPRETER_BACKEND_VALUE_H
#define KM2_INTERPRETER_BACKEND_VALUE_H

#include "../../backend/entities/value.h"
#include <memory>
#include <wall_e/src/utility/collections.h>

namespace km2 {
namespace interpreter_backend {

class type;
class dyn_value;

class value : public km2::backend::value {
    type* m_type;
public:
    value(type* type) : m_type(type) {}
    static value* downcast(km2::backend::value* t);

    virtual std::shared_ptr<dyn_value> dyn() const = 0;

    virtual km2::backend::type* value_type() const override;
};

}
}

#endif // KM2_LLVM_BACKEND_VALUE_H
