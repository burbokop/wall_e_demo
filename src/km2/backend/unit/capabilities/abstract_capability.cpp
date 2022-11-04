#include "abstract_capability.h"



km2::backend::abstract_capability::abstract_capability(km2::backend::unit *unit)
    : m_unit(unit) {}

km2::backend::unit *km2::backend::abstract_capability::unit() const { return m_unit; }

km2::backend::abstract_capability::~abstract_capability() {}
