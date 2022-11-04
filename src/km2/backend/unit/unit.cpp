#include "unit.h"

#include "../backend.h"

#include <iostream>

km2::backend::abstract_capability *km2::backend::unit::new_capability(std::size_t hash) const {
    const auto& it = m_backend->m_factory_cache->cap_map.find(hash);
    if(it != m_backend->m_factory_cache->cap_map.end()) {
        return it->second(const_cast<unit*>(this));
    } else {
        return nullptr;
    }
}

km2::backend::unit::~unit() {
    for(const auto& c : m_capabilities) {
        delete c.second;
    }
}
