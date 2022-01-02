#include "abstract_node.h"


km2::abstract_node::abstract_node(const children_t &children, const wall_e::text_segment &segment)
    : m_segment(segment) {
    for(const auto& c : children) {
        if(c) { c->m_parent = this; }
    }
}

km2::abstract_node* km2::abstract_node::parent() const {
    return m_parent;
}

wall_e::text_segment km2::abstract_node::segment() const {
    return m_segment;
}

std::map<std::size_t, km2::abstract_node*> km2::abstract_node::ancestor_cache() const {
    return m_ancestor_cache;
}


std::vector<std::shared_ptr<km2::abstract_node>> km2::abstract_node::children() const {
    return m_children;
}

