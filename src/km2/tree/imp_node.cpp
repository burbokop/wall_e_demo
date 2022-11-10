#include "imp_node.h"

#include <iostream>

km2::imp_node::imp_node(
        const wall_e::index &index,
        const children_t &children,
        const wall_e::text_segment &segment
        ) : abstract_value_node(index, children, segment) {}

wall_e::gram::argument km2::imp_node::create(const wall_e::gram::arg_vector &args, const wall_e::index &index) {
    if(debug) std::cout << "km2::imp_node::create: " << args << std::endl;
    return {};
}
