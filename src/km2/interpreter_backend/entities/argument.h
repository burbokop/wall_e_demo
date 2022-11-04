#ifndef KM2_INTERPRETER_BACKEND_ARGUMENT_H
#define KM2_INTERPRETER_BACKEND_ARGUMENT_H

#include "../../backend/entities/argument.h"
#include "value.h"
#include <string>

namespace km2 {
namespace interpreter_backend {

class type;
class function;

class argument : public km2::backend::argument, public km2::interpreter_backend::value {
    function* m_func;
    std::size_t m_index;
public:
    argument(type* type, function* func, std::size_t index)
        : km2::interpreter_backend::value(type),
          m_func(func),
          m_index(index) {};

    inline static km2::backend::argument* upcast(argument* t) { return dynamic_cast<km2::backend::argument*>(t); }
public:
    virtual km2::backend::function *func() const override;
    virtual std::size_t index() const override;

    // value interface
public:
    virtual km2::backend::type *value_type() const override { return km2::interpreter_backend::value::value_type(); }

    // printable interface
public:
    virtual std::ostream &print(std::ostream &) const override;

    // value interface
public:
    virtual std::shared_ptr<dyn_value> dyn() const override;
};

}
}

#endif // KM2_LLVM_BACKEND_ARGUMENT_H
