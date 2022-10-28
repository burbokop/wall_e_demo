#ifndef KM2_ASM_NODE_H
#define KM2_ASM_NODE_H

#include "abstract/abstract_value_node.h"
#include <wall_e/src/gram.h>

namespace km2 {

class asm_node : public km2::abstract_value_node {
public:
    typedef abstract_value_node super_type;

    asm_node(const wall_e::index &index, const std::string &text);


    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index);

    // node interface
public:
    virtual wall_e::either<wall_e::error, llvm::Value*> generate_llvm(const std::shared_ptr<translation_unit> &unit) override;
    virtual void print(size_t level, std::ostream &stream) const override;

    // abstract_node interface
public:
    virtual std::list<wall_e::error> errors() const override;
    virtual void short_print(std::ostream &stream) const override;
};

} // namespace km2

#endif // KM2_ASM_NODE_H
