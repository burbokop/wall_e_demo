#include "abstract_node.h"


km2::abstract_node::abstract_node(const wall_e::text_segment &segment) {
    m_segment = segment;
}

wall_e::text_segment km2::abstract_node::segment() const {
    return m_segment;
}

