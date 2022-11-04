#ifndef CONSTANT_H
#define CONSTANT_H

#include "value.h"

namespace km2 {
namespace interpreter_backend {

class constant : public value {
    std::shared_ptr<dyn_value> m_dyn;
public:
    constant(type* type, const std::shared_ptr<dyn_value>& dyn)
        : value(type),
          m_dyn(dyn) {}

    // printable interface
public:
    virtual std::ostream &print(std::ostream &) const override;

    // value interface
public:
    virtual std::shared_ptr<dyn_value> dyn() const override { return m_dyn; }
};

}
}

#endif // CONSTANT_H
