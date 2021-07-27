#include "abstract_node.h"


km2::abstract_node::abstract_node(const text_segment &segment) {
    m_segment = segment;
}


km2::text_segment km2::abstract_node::segment() const {
    return m_segment;
}

