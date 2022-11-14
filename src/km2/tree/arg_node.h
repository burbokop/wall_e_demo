#ifndef KM2_ARG_NODE_H
#define KM2_ARG_NODE_H

#include "abstract/abstract_value_node.h"

#include <wall_e/src/gram.h>

#include <src/km2/utility/markup_string.h>

namespace km2 {

class arg_node : public km2::abstract_value_node {
public:
    typedef abstract_value_node super_type;

    wall_e_enum_member(type,
        Id,
        IntLiteral,
        FloatLiteral,
        StringLiteral,
        ValueNode,
        Undefined
    )

private:
    type m_type;
    std::string m_text;
    std::shared_ptr<abstract_value_node> m_value_node;
public:

    arg_node(
            const wall_e::index &index,
            const wall_e::text_segment& segment,
            type t,
            const std::string& text = {},
            std::shared_ptr<abstract_value_node> value_node = nullptr
            );

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index, const wall_e::gram::environment* env);

    ast_token_type token_type() const;
    markup_string hover() const;

    // node interface
public:
    virtual wall_e::either<
        wall_e::error,
        backend::value*
    > generate_backend_value(const std::shared_ptr<backend::unit> &unit) override;
    virtual wall_e::list<wall_e::error> errors() const override;

    // abstract_node interface
public:
    virtual std::ostream &short_print(std::ostream &stream) const override;
    virtual wall_e::list<ast_token> tokens() const override;
    virtual std::ostream &write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context& ctx) const override;
};

} // namespace km2

#endif // KM2_ARG_NODE_H
