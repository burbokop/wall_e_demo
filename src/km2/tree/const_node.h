#ifndef KM2_CONST_NODE_H
#define KM2_CONST_NODE_H

#include "abstract/abstract_value_node.h"
#include <wall_e/src/gram.h>
#include "../utility/lvalue.h"

namespace km2 {

class arg_node;

class const_node : public km2::abstract_value_node {
    const std::shared_ptr<arg_node> m_value;
public:
    typedef abstract_value_node super_type;

    const_node(
            const wall_e::index &index,
            const std::shared_ptr<arg_node>& value = nullptr
            );

    static factory create();



    // abstract_value_node interface
public:
    virtual wall_e::either<wall_e::error, backend::value*> generate_backend_value(const std::shared_ptr<backend::unit> &unit) override;

    // abstract_node interface
public:
    virtual std::ostream& short_print(std::ostream &stream) const override;
    virtual wall_e::list<wall_e::error> errors() const override;
    virtual wall_e::list<ast_token> tokens() const override;
    virtual std::ostream &write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const override;
    virtual markup_string hover() const override;
    virtual ast_token_type rvalue_type() const override;
};

} // namespace km2

#endif // KM2_CONST_NODE_H
