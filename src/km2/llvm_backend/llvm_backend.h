#ifndef KM2_LLVM_BACKEND_BACKEND_H
#define KM2_LLVM_BACKEND_BACKEND_H

#include "../backend/backend.h"

namespace km2 {

namespace llvm_backend {

class backend : public km2::backend::backend {
public:
    backend() {}

    // backend interface
public:
    virtual std::string name() override;
    virtual km2::backend::abstract_capability::factory_map register_unit_caps() override;
    virtual km2::backend::unit::factory register_unit_factory() override;
};

}

}

#endif // LLVM_LLVM_BACKEND_BACKEND_H
