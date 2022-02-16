#include "abstract_node.h"

#include <numeric>
#include <execution>


std::vector<km2::context> km2::abstract_node::contexts(const children_t &children) {
    std::vector<km2::context> result;
    transform(children.begin(), children.end(), back_inserter(result), [](const std::shared_ptr<abstract_node> &n){
        return n ? n->context() : km2::context();
    });
    return result;
}

km2::abstract_node::abstract_node(const wall_e::index &index, const children_t &children, const wall_e::text_segment &segment)
    : m_index(index),
      m_context(km2::context::sum(contexts(children))),
      m_segment(segment) {
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

const km2::context &km2::abstract_node::context() const { return m_context; }
const wall_e::index &km2::abstract_node::index() const { return m_index; }
