#ifndef BASE_NODE_H
#define BASE_NODE_H

#include "block_node.h"
#include "abstract/abstract_value_node.h"
#include <wall_e/src/gram.h>
#include <string>

namespace km2 {
class base_node : public abstract_value_node {
    block_node *m_block_node = nullptr;
public:
    typedef abstract_value_node super_type;

    base_node(block_node *block_node = nullptr);

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args);

    // node interface
public:
    virtual llvm::Value *generate_llvm(module_builder *builder) override;
    virtual void print(size_t level, std::ostream &stream) override;
    virtual std::list<error> errors() override;
};
}


#endif // BASE_NODE_H
