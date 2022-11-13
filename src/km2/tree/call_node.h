#ifndef KM2_CALL_NODE_H
#define KM2_CALL_NODE_H

#include "abstract/abstract_value_node.h"
#include <wall_e/src/gram.h>

namespace km2 {

class call_node : public km2::abstract_value_node {
    const wall_e::lex::token_vec m_namespace_stack;
    const std::string m_name;
    const wall_e::vec<std::shared_ptr<km2::abstract_value_node>> m_args;
    const wall_e::text_segment m_name_segment;
public:
    typedef abstract_value_node super_type;

    call_node(
            const wall_e::index& index,
            const wall_e::lex::token_vec& namespace_stack,
            const std::string& name,
            const wall_e::vec<std::shared_ptr<km2::abstract_value_node>>& args,
            const wall_e::text_segment& name_segment
            );

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index);

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
};

} // namespace km2

#endif // KM2_CALL_NODE_H
