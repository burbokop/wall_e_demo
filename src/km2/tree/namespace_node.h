#ifndef NAMESPACE_NODE_H
#define NAMESPACE_NODE_H

#include "block_node.h"
#include "abstract/abstract_value_node.h"
#include <wall_e/src/gram.h>
#include <string>
#include <src/km2/utility/lvalue.h>



namespace km2 {
class namespace_node : public abstract_value_node {
    //const lvalue m_lvalue;
    const wall_e::lex::token m_keyword_token;
    const std::shared_ptr<block_node> m_block_node;

    backend::context m_context;
public:
    typedef abstract_value_node super_type;

    namespace_node(
            const wall_e::gram::environment* env,
            const wall_e::index& index,
            const wall_e::lex::token& keyword_token,
            const std::shared_ptr<block_node>& block_node
            );

    static factory create(const lvalue::factory& lval_factory);

    // node interface
public:
    virtual wall_e::either<
        wall_e::error,
        backend::value*
    > generate_backend_value(const std::shared_ptr<backend::unit> &unit) override;

    virtual wall_e::list<wall_e::error> errors() const override;
    wall_e::opt<std::string> name() const;
    wall_e::str_list full_name() const;
    const backend::context &context() const;

    // abstract_node interface
public:
    virtual std::ostream &short_print(std::ostream &stream) const override;
    virtual wall_e::list<ast_token> tokens() const override;
    virtual std::ostream &write(std::ostream &stream, const wall_e::tree_writer::context& ctx) const override;
    virtual ast_token_type rvalue_type() const override;
    virtual markup_string hover() const override;
};
}


#endif // NAMESPACE_NODE_H
