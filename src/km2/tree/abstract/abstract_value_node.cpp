#include "abstract_value_node.h"


km2::abstract_value_node::abstract_value_node(const std::vector<std::shared_ptr<abstract_node> > &children, const wall_e::text_segment &segment)
    : abstract_node(
          children,
          segment
          ) {}
