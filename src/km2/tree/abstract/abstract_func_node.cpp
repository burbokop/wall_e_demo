#include "abstract_func_node.h"



km2::abstract_func_node::abstract_func_node(
        const wall_e::index &index,
        const children_t &children,
        const wall_e::text_segment &segment
        ) : abstract_value_node(index, children, segment) {

}
