#ifndef IMP_NODE_H
#define IMP_NODE_H

#include "abstract/abstract_value_node.h"

namespace km2 {

class imp_node : public abstract_value_node {
public:
    imp_node(
            const wall_e::index& index,
            const children_t &children,
            const wall_e::text_segment &segment = {}
            );

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index);

};

}

#endif // IMP_NODE_H
