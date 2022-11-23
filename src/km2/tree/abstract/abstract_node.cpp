#include "abstract_node.h"

#include <numeric>
#include <execution>

#include <src/km2/tree/substitution_node.h>


std::vector<km2::backend::context> km2::abstract_node::contexts(const children_t &children) {
    std::vector<backend::context> result;
    transform(children.begin(), children.end(), back_inserter(result), [](const std::shared_ptr<const abstract_node> &n){
        return n ? n->ctx() : backend::context();
    });
    return result;
}

km2::abstract_node::abstract_node(
        const wall_e::gram::environment *env,
        const wall_e::index &index,
        const children_t &children,
        const wall_e::text_segment &segment,
        const wall_e::box_list<node_trait>::factory &traits
        )
    : m_env(env),
      m_index(index),
      m_children(children),
      m_context(backend::context::sum(contexts(children))),
      m_segment(segment),
      m_traits(traits) {
    for(const auto& c : children) {
        if(c) { c->m_parent = this; }
    }
}

wall_e::text_segment km2::abstract_node::segment() const {
    return m_segment;
}

std::map<std::size_t, const km2::abstract_node*> km2::abstract_node::ancestor_cache() const {
    return m_ancestor_cache;
}

wall_e::opt<km2::lvalue> km2::abstract_node::lval() const {
    if(const auto s = parent_as<substitution_node>()) {
        return s->lvalue();
    } else {
        return std::nullopt;
    }
}

wall_e::str_list km2::abstract_node::lval_full_name() const {
    if(const auto s = parent_as<substitution_node>()) {
        return s->full_name();
    } else {
        return {};
    }
}


const km2::backend::context &km2::abstract_node::ctx() const { return m_context; }
const wall_e::index &km2::abstract_node::index() const { return m_index; }
