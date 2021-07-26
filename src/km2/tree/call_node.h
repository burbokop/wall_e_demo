#ifndef KM2_CALL_NODE_H
#define KM2_CALL_NODE_H

#include "abstract/abstract_value_node.h"
#include <wall_e/src/gram.h>

namespace km2 {

class call_node : public km2::abstract_value_node {
    std::string m_name;
    std::vector<km2::abstract_value_node*> m_args;
public:
    typedef abstract_value_node super_type;

    call_node(const std::string& name, const std::vector<km2::abstract_value_node*>& args);
    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args);

    // node interface
public:
    virtual llvm::Value *generate_llvm(module_builder *builder) override;
    virtual void print(size_t level, std::ostream &stream) override;
};

} // namespace km2

#endif // KM2_CALL_NODE_H
