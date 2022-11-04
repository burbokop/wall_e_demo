#ifndef KM2_INTERPRETER_BACKEND_INTERPRETER_BACKEND_H
#define KM2_INTERPRETER_BACKEND_INTERPRETER_BACKEND_H

#include "../backend/backend.h"

namespace km2 {
namespace interpreter_backend {

class backend : public km2::backend::backend {
public:

    // backend interface
public:
    virtual std::string name() const override;
    virtual ::km2::backend::abstract_capability::factory_map register_unit_caps() const override;
    virtual ::km2::backend::unit::factory register_unit_factory() const override;
};

}
}

#endif // KM2_INTERPRETER_BACKEND_INTERPRETER_BACKEND_H
