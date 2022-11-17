#ifndef KM2_BLOCK_NODE_H
#define KM2_BLOCK_NODE_H

#include "abstract/abstract_value_node.h"
#include <wall_e/src/gram.h>
#include <src/km2/backend/models/context.h>
#include <src/km2/backend/entities/function.h>

namespace km2 {
class stmt_node;
class namespace_node;
class block_node : public km2::abstract_value_node {
    const wall_e::vec<std::shared_ptr<stmt_node>> m_statements;

    const backend::context m_acc_context;
    const backend::context m_context;
    backend::context m_mutable_context;
public:
    typedef abstract_value_node super_type;

    block_node(
            const wall_e::index &index,
            const wall_e::vec<std::shared_ptr<stmt_node>>& statements
            );

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index, const wall_e::gram::environment* env);

    // node interface
public:
    virtual wall_e::either<
        wall_e::error,
        backend::value*
    > generate_backend_value(const std::shared_ptr<backend::unit> &unit) override;

    std::optional<wall_e::error> add_function(const backend::function_ref &function);

    std::shared_ptr<backend::overload> find_overload_in_block_seq(
            const std::list<std::string> &namespace_stack,
            const std::string &name
            ) const;

    std::shared_ptr<backend::overload> find_overload_in_whole_tree(
            const std::list<std::string> &namespace_stack,
            const std::string &name
            ) const;

    enum search_scope {
        WholeTree,
        Block,
        Namespace
    };

    wall_e::list<std::shared_ptr<abstract_value_node>> find_call_candidate_nodes(
            const wall_e::str_list &namespace_stack,
            const std::string &name,
            search_scope search_scope
            ) const;


    const backend::context &context() const { return m_context; }
    const backend::context &acc_context() const { return m_acc_context; }
    backend::context &mutable_context() { return m_mutable_context; };

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

#endif // KM2_BLOCK_NODE_H
