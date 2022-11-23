#ifndef CLASS_NODE_H
#define CLASS_NODE_H

#include "abstract/abstract_type_node.h"
#include "abstract/abstract_func_node.h"

namespace km2 {

class decl_arg_node;
class block_node;

class class_node : public abstract_type_node {
    const wall_e::lex::token m_keyword_token;
    const wall_e::vec<std::shared_ptr<decl_arg_node>> m_args;
    const std::shared_ptr<block_node> m_body;
public:
    typedef abstract_type_node super_type;

    class_node(
            const wall_e::gram::environment* env,
            const wall_e::index& index,
            const wall_e::lex::token& keyword_token,
            const wall_e::vec<std::shared_ptr<decl_arg_node> > &args,
            std::shared_ptr<block_node> body
            );

    static km2::abstract_node::factory create();


    // abstract_node interface
public:
    virtual std::ostream &write(std::ostream &stream, const wall_e::tree_writer::context &ctx) const override;
    virtual std::ostream &short_print(std::ostream &stream) const override;
    virtual ast_token_list tokens() const override;
    virtual ast_token_type rvalue_type() const override;
    virtual markup_string hover() const override;
    virtual wall_e::list<wall_e::error> errors() const override;

    // abstract_value_node interface
public:
    virtual wall_e::either<wall_e::error, backend::value*> generate_backend_value(const std::shared_ptr<backend::unit> &unit) override;

    // abstract_type_node interface
public:
    virtual wall_e::either<wall_e::error, backend::type*> generate_backend_type(const std::shared_ptr<backend::unit> &unit) override;
};

}

#endif // CLASS_NODE_H
