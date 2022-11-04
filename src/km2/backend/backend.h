#ifndef KM2_BACKEND_BACKEND_H
#define KM2_BACKEND_BACKEND_H

#include "unit/capabilities/abstract_capability.h"
#include "unit/unit.h"
#include <string>
#include <wall_e/src/utility/collections.h>

namespace km2 {
namespace backend {

class backend {
    friend unit;
    struct unit_factory_cache {
        unit::factory unit_fac;
        abstract_capability::factory_map cap_map;
    };
    mutable wall_e::opt<unit_factory_cache> m_factory_cache;
public:
    virtual std::string name() const = 0;
    virtual abstract_capability::factory_map register_unit_caps() const = 0;
    virtual unit::factory register_unit_factory() const = 0;

    std::shared_ptr<unit> create_unit() const;

    virtual ~backend() {}
};

}
}

#endif // KM2_BACKEND_BACKEND_H
