#ifndef KM2_PROTO_NODE_H
#define KM2_PROTO_NODE_H

#include "abstract/abstract_value_node.h"
#include "decl_arg_node.h"
#include <wall_e/src/gram.h>
#include <src/km2/tree/abstract/abstract_type_node.h>
#include <src/km2/utility/lvalue.h>


namespace km2 {

class proto_node : public km2::abstract_value_node {
    const wall_e::vec<std::shared_ptr<decl_arg_node>> m_args;
    const std::shared_ptr<abstract_type_node> m_result_type_node;
public:
    typedef abstract_value_node super_type;

    proto_node(
            const wall_e::index &index,
            const wall_e::vec<std::shared_ptr<decl_arg_node>> &args,
            std::shared_ptr<abstract_type_node> result_type_node
            );

    static factory create();

    wall_e::str_list full_name() const;

    // node interface
public:
    virtual wall_e::either<
        wall_e::error,
        backend::value*
    > generate_backend_value(const std::shared_ptr<backend::unit> &unit) override;

    // abstract_node interface
public:
    virtual wall_e::list<wall_e::error> errors() const override;
    virtual std::ostream &short_print(std::ostream &stream) const override;
    virtual wall_e::list<ast_token> tokens() const override;
    virtual std::ostream &write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context& ctx) const override;
    virtual ast_token_type rvalue_type() const override;
    virtual markup_string hover() const override;
};

} // namespace km2

#endif // KM2_PROTO_NODE_H
