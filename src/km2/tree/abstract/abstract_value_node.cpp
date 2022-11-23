#include "abstract_value_node.h"


km2::abstract_value_node::abstract_value_node(
        const wall_e::gram::environment *env,
        const wall_e::index &index,
        const children_t &children,
        const wall_e::text_segment &segment,
        const wall_e::box_list<node_trait>::factory &traits
        ) : abstract_node(
                env,
                index,
                children,
                segment,
                traits
                ) {}
