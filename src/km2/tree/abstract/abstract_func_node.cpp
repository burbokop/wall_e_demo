#include "abstract_func_node.h"



km2::abstract_func_node::abstract_func_node(
        const wall_e::gram::environment *env,
        const wall_e::index &index,
        const children_t &children,
        const wall_e::text_segment &segment,
        const wall_e::box_list<node_trait>::factory &traits
        ) : abstract_value_node(env, index, children, segment, traits) {}
