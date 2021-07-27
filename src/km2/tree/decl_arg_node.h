#ifndef KM2_DECL_ARG_NODE_H
#define KM2_DECL_ARG_NODE_H

#include <src/km2/tree/abstract/abstract_node.h>
#include <src/km2/tree/abstract/abstract_type_node.h>
#include <wall_e/src/gram.h>

namespace km2 {

class decl_arg_node : public km2::abstract_node {
    std::string m_name;
    abstract_type_node* m_type_node = nullptr;
    bool m_is_variadic = false;
public:
    typedef abstract_node super_type;

    decl_arg_node(const std::string &name, abstract_type_node* type_node, bool is_variadic = false);

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args);

    bool is_variadic() const;
    abstract_type_node *type_node() const;
    const std::string &name() const;

    // abstract_node interface
public:
    virtual void print(size_t level, std::ostream &stream) override;

    // abstract_node interface
public:
    virtual std::list<error> errors() override;
};

} // namespace km2

#endif // KM2_DECL_ARG_NODE_H