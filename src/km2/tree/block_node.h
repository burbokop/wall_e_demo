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
    const std::shared_ptr<stmt_node> m_stmt_node;
    const std::shared_ptr<namespace_node> m_namespace_node;

    const std::shared_ptr<block_node> m_next_node;

    const backend::context m_acc_context;
    const backend::context m_context;
    backend::context m_mutable_context;
public:
    typedef abstract_value_node super_type;

    block_node(
            const wall_e::index &index,
            const std::shared_ptr<stmt_node> &stmt_node,
            const std::shared_ptr<namespace_node> &namespace_node,
            const std::shared_ptr<block_node>& next_node = nullptr
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
            );

    std::shared_ptr<backend::overload> find_overload_in_whole_tree(
            const std::list<std::string> &namespace_stack,
            const std::string &name
            );

    const backend::context &context() const { return m_context; }
    const backend::context &acc_context() const { return m_acc_context; }
    backend::context &mutable_context() { return m_mutable_context; };

    // abstract_node interface
public:
    virtual wall_e::list<wall_e::error> errors() const override;
    virtual std::ostream &short_print(std::ostream &stream) const override;
    virtual wall_e::list<ast_token> tokens() const override;
    virtual std::ostream &write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context& ctx) const override;
};

} // namespace km2

#endif // KM2_BLOCK_NODE_H
