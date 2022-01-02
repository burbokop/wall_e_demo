#ifndef KM2_FUNCTION_NODE_H
#define KM2_FUNCTION_NODE_H

#include "abstract/abstract_value_node.h"

#include "decl_arg_node.h"

#include <string>
#include <vector>
#include <wall_e/src/gram.h>

namespace km2 {

class function_node : public abstract_value_node {
    std::string m_name;
    std::vector<std::shared_ptr<decl_arg_node>> m_args;
    std::shared_ptr<abstract_value_node> m_body;
public:
    typedef abstract_value_node super_type;

    function_node(const std::string &name, const std::vector<std::shared_ptr<decl_arg_node>> &args, std::shared_ptr<abstract_value_node> body);

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args);

    // node interface
public:
    virtual wall_e::either<wall_e::error, llvm::Value*> generate_llvm(const std::shared_ptr<module> &module) override;
    virtual void print(size_t level, std::ostream &stream) override;
    virtual std::list<wall_e::error> errors() override;
};

} // namespace km2

#endif // KM2_FUNCTION_NODE_H
