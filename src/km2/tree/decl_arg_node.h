#ifndef KM2_DECL_ARG_NODE_H
#define KM2_DECL_ARG_NODE_H

#include <src/km2/tree/abstract/abstract_node.h>
#include <src/km2/tree/abstract/abstract_type_node.h>
#include <wall_e/src/gram.h>

namespace km2 {

class decl_arg_node : public km2::abstract_node {
    const std::string m_name;
    const wall_e::text_segment m_name_segment;
    const std::shared_ptr<abstract_type_node> m_type_node;
    const bool m_is_variadic = false;
public:
    typedef abstract_node super_type;

    decl_arg_node(
            const wall_e::index &index,
            const std::string &name,
            const wall_e::text_segment& name_segment,
            std::shared_ptr<abstract_type_node> type_node,
            bool is_variadic = false
            );

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index);

    bool is_variadic() const;
    std::shared_ptr<abstract_type_node> type_node() const;
    std::string name() const;

    // abstract_node interface
public:
    std::ostream &short_print(std::ostream &stream) const override;
    virtual wall_e::list<wall_e::error> errors() const override;
    virtual wall_e::list<ast_token> tokens() const override;
    virtual std::ostream &write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context& ctx) const override;
};

} // namespace km2

#endif // KM2_DECL_ARG_NODE_H
