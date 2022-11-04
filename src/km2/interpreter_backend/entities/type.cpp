#include "type.h"


bool km2::interpreter_backend::type::eq(const km2::backend::type *other) const {
    //return m_data->getTypeID() == dynamic_cast<const type*>(other)->m_data->getTypeID();

    auto tt = other->as<const type>();

    auto a = m_id == other->as<const type>()->m_id;
    auto b = m_bits == other->as<const type>()->m_bits;

    return a && b;
}
