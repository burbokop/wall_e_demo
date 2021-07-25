#ifndef KM2_FUNCTION_NODE_H
#define KM2_FUNCTION_NODE_H

#include "node.h"

#include <string>
#include <vector>
#include <wall_e/src/gram.h>

namespace km2 {

class function_node : public node {
    std::string m_name;
    std::vector<std::string> m_argTypes;
    node *m_body;
public:
    function_node(const std::string &name, const std::vector<std::string> &typenames, node *body);

    const std::string &name() const;
    const std::vector<std::string> &argTypes() const;


    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args);

    // node interface
public:
    virtual llvm::Value *generate_llvm(module_builder *builder) override;
    virtual void print(size_t level, std::ostream &stream) override;
};

} // namespace km2

#endif // KM2_FUNCTION_NODE_H
