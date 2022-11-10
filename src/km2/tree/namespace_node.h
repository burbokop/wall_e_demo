#ifndef NAMESPACE_NODE_H
#define NAMESPACE_NODE_H

#include "block_node.h"
#include "abstract/abstract_value_node.h"
#include <wall_e/src/gram.h>
#include <string>



namespace km2 {
class namespace_node : public abstract_value_node {
    const std::string m_keyword;
    const wall_e::text_segment m_keyword_segment;
    const std::string m_name;
    const wall_e::text_segment m_name_segment;
    const std::shared_ptr<block_node> m_block_node;

    backend::context m_context;
public:
    typedef abstract_value_node super_type;

    namespace_node(const wall_e::index& index,
            const std::string& keyword,
            const wall_e::text_segment& keyword_segment,
            const std::string &name = {},
            const wall_e::text_segment& name_segment = {},
            const std::shared_ptr<block_node>& block_node = nullptr,
            const bool exp = false
            );

    static factory create(const std::string& name_token);

    // node interface
public:
    virtual wall_e::either<
        wall_e::error,
        backend::value*
    > generate_backend_value(const std::shared_ptr<backend::unit> &unit) override;

    virtual void print(size_t level, std::ostream &stream) const override;
    virtual wall_e::list<wall_e::error> errors() const override;
    std::string name() const;
    wall_e::str_list full_name() const;
    const backend::context &context() const;

    // abstract_node interface
public:
    virtual void short_print(std::ostream &stream) const override;
    virtual wall_e::list<ast_token> tokens() const override;
};
}


#endif // NAMESPACE_NODE_H
