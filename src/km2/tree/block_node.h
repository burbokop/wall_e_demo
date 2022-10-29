#ifndef KM2_BLOCK_NODE_H
#define KM2_BLOCK_NODE_H

#include "stmt_node.h"
#include "abstract/abstract_value_node.h"
#include <wall_e/src/gram.h>
#include <src/km2/translation_unit/models/context.h>
#include <src/km2/translation_unit/models/function.h>

namespace km2 {

class block_node : public km2::abstract_value_node {
    const std::shared_ptr<stmt_node> m_stmt;
    const std::shared_ptr<block_node> m_next_node;

    const km2::context m_acc_context;
    const km2::context m_context;
    km2::context m_mutable_context;
public:
    typedef abstract_value_node super_type;

    block_node(const wall_e::index &index, const std::shared_ptr<stmt_node> &stmt, const std::shared_ptr<block_node>& next_node = nullptr);

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index);

    // node interface
public:
    virtual wall_e::either<
        wall_e::error,
        llvm::Value*
    > generate_llvm(const std::shared_ptr<translation_unit> &unit) override;

    virtual void print(size_t level, std::ostream &stream) const override;


    std::optional<wall_e::error> add_function(const km2::function &function);

    std::shared_ptr<overload> find_overload_in_block_seq(
            const std::list<std::string> &namespace_stack,
            const std::string &name
            );
    std::shared_ptr<overload> find_overload_in_whole_tree(
            const std::list<std::string> &namespace_stack,
            const std::string &name
            );

    const km2::context &context() const;
    const km2::context &acc_context() const;
    km2::context &mutable_context();

    // abstract_node interface
public:
    virtual wall_e::list<wall_e::error> errors() const override;
    virtual void short_print(std::ostream &stream) const override;
    virtual wall_e::list<ast_token> tokens() const override;
};

} // namespace km2

#endif // KM2_BLOCK_NODE_H
