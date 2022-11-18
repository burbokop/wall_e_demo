#ifndef KM2_CALL_NODE_H
#define KM2_CALL_NODE_H

#include "abstract/abstract_value_node.h"
#include <wall_e/src/gram.h>
#include <wall_e/src/utility/lazy.h>

namespace km2 {

wall_e::str_list subtract_namespace_stack(const wall_e::str_list& op0, const wall_e::str_list& op1);

class function_node;
class call_node : public km2::abstract_value_node {
    const wall_e::lex::token_vec m_apeal_namespace_stack;
    const std::string m_name;
    const wall_e::vec<std::shared_ptr<km2::abstract_value_node>> m_args;
    const wall_e::text_segment m_name_segment;

    std::shared_ptr<const function_node> eval_declaration() const;
    const wall_e::lazy<std::shared_ptr<const function_node>> m_declaration = wall_e::lazy(this, &call_node::eval_declaration);
public:
    typedef abstract_value_node super_type;

    wall_e::str_list full_apeal_name() const;
    wall_e::str_list current_nspace_stack() const;

    call_node(
            const wall_e::index& index,
            const wall_e::lex::token_vec& apeal_namespace_stack,
            const std::string& name,
            const wall_e::vec<std::shared_ptr<km2::abstract_value_node>>& args,
            const wall_e::text_segment& name_segment
            );

    const std::shared_ptr<const function_node> declaration() const { return m_declaration.value(); }

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index, const wall_e::gram::environment* env);

    //wall_e::str_list full_name() const;

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

#endif // KM2_CALL_NODE_H
