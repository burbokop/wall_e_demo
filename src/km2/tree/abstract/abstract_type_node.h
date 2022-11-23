#ifndef ABSTRACT_TYPE_NODE_H
#define ABSTRACT_TYPE_NODE_H

#include "abstract_value_node.h"
#include <wall_e/src/models/either.h>
#include <memory>


namespace km2 {

namespace backend {
class unit;
}

class abstract_type_node : public abstract_value_node {
protected:
    abstract_type_node(
            const wall_e::gram::environment* env,
            const wall_e::index& index,
            const children_t &children,
            const wall_e::text_segment &segment = {},
            const wall_e::box_list<node_trait>::factory& traits = {}
            );
public:
    typedef abstract_value_node super_type;

    virtual wall_e::either<wall_e::error, backend::type*> generate_backend_type(const std::shared_ptr<backend::unit> &unit) = 0;
};

} // namespace km2

#endif // ABSTRACT_TYPE_NODE_H
