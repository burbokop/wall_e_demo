#ifndef NAMESPACE_NODE_H
#define NAMESPACE_NODE_H

#include "block_node.h"
#include "abstract/abstract_value_node.h"
#include <wall_e/src/gram.h>
#include <string>



namespace km2 {
class namespace_node : public abstract_value_node {
    const std::string m_name;
    const std::shared_ptr<block_node> m_block_node;

    km2::context m_context;
public:
    typedef abstract_value_node super_type;

    namespace_node(
            const wall_e::index& index,
            const std::string &name = {},
            const std::shared_ptr<block_node>& block_node = nullptr
            );

    static factory create(const std::string& name_token);

    // node interface
public:
    virtual wall_e::either<
        wall_e::error,
        llvm::Value*
    > generate_llvm(const std::shared_ptr<translation_unit> &unit) override;

    virtual void print(size_t level, std::ostream &stream) override;
    virtual std::list<wall_e::error> errors() const override;
    std::string name() const;
    std::list<std::string> full_name() const;
    const km2::context &context() const;
};
}


#endif // NAMESPACE_NODE_H
