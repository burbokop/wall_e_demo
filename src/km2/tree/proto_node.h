#ifndef KM2_PROTO_NODE_H
#define KM2_PROTO_NODE_H

#include "abstract/abstract_value_node.h"
#include "decl_arg_node.h"
#include <wall_e/src/gram.h>
#include <src/km2/tree/abstract/abstract_type_node.h>


namespace km2 {

class proto_node : public km2::abstract_value_node {
    std::string m_name;
    std::vector<decl_arg_node *> m_args;
    abstract_type_node* m_result_type_node = nullptr;
public:
    typedef abstract_value_node super_type;

    proto_node(const std::string &name, const std::vector<decl_arg_node*> &args, abstract_type_node* result_type_node);

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args);


    // node interface
public:
    virtual wall_e::either<km2::error, llvm::Value*> generate_llvm(module_builder *builder) override;
    virtual void print(size_t level, std::ostream &stream) override;

    // abstract_node interface
public:
    virtual std::list<error> errors() override;
};

} // namespace km2

#endif // KM2_PROTO_NODE_H
