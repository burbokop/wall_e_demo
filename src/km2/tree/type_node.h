#ifndef KM2_TYPE_NODE_H
#define KM2_TYPE_NODE_H

#include "abstract/abstract_type_node.h"
#include <wall_e/src/gram.h>

namespace km2 {

class type_node : public km2::abstract_type_node {
public:
    typedef km2::abstract_type_node super_type;

    enum type {
        Unsigned,
        Signed,
        Float,
        Double,
        String,
        Undefined
    };

    static std::string type_string(type  t);

private:
    type m_type = Undefined;
    std::string m_text;
public:

    type_node(type t, const std::string &text = {});

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args);


    // abstract_node interface
public:
    virtual void print(size_t level, std::ostream &stream) override;

    // abstract_type_node interface
public:
    virtual wall_e::either<km2::error, llvm::Type*> generate_llvm(const std::shared_ptr<module> &module) override;

    // abstract_node interface
public:
    virtual std::list<error> errors() override;
};

} // namespace km2

#endif // KM2_TYPE_NODE_H
