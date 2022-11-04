#include "backend.h"



std::shared_ptr<km2::backend::unit> km2::backend::backend::create_unit() const {
    if(!m_factory_cache) {
        m_factory_cache = unit_factory_cache {
            .unit_fac = register_unit_factory(),
            .cap_map = register_unit_caps()
        };
    }
    return m_factory_cache->unit_fac(this);
}
