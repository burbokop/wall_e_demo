#ifndef KM2_CALL_NODE_H
#define KM2_CALL_NODE_H

#include "node.h"
#include <wall_e/src/gram.h>

namespace km2 {

class call_node : public km2::node {
    std::string m_name;
    std::vector<wall_e::lex::token> m_args;
public:
    call_node(const std::string& name, const std::vector<wall_e::lex::token> &args);
    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args);

    // node interface
public:
    virtual llvm::Value *generate_llvm(module_builder *builder) override;
    virtual void print(size_t level, std::ostream &stream) override;
};

} // namespace km2

#endif // KM2_CALL_NODE_H
