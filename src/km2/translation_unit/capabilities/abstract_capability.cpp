#include "abstract_capability.h"



km2::abstract_capability::abstract_capability(translation_unit *unit)
    : m_unit(unit) {}

km2::translation_unit *km2::abstract_capability::unit() const { return m_unit; }

km2::abstract_capability::~abstract_capability() {}
