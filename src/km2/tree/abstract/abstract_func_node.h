#ifndef ABSTRACT_FUNC_NODE_H
#define ABSTRACT_FUNC_NODE_H

#include "abstract_value_node.h"

namespace km2 {

class abstract_func_node : public abstract_value_node {
public:
    typedef abstract_value_node super_type;
    abstract_func_node(
            const wall_e::gram::environment* env,
            const wall_e::index& index,
            const children_t &children,
            const wall_e::text_segment &segment = {},
            const wall_e::box_list<node_trait>::factory& traits = {}
            );

};

}

#endif // ABSTRACT_FUNC_NODE_H
