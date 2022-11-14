#ifndef IMP_NODE_H
#define IMP_NODE_H

#include "abstract/abstract_value_node.h"

#include <wall_e/src/utility/box.h>

namespace km2 {

class compilation_result;

class imp_node : public abstract_value_node {
    const wall_e::lex::token m_keyword_token;
    const wall_e::lex::token m_name_token;
    const wall_e::opt_box<compilation_result> m_module_cresult;
    const wall_e::list<wall_e::error> m_search_errors;
public:
    typedef abstract_value_node super_type;

    imp_node(
            const wall_e::index& index,
            const wall_e::lex::token& keyword_token,
            const wall_e::lex::token& name_token,
            const wall_e::opt<compilation_result>& module_cresult,
            const wall_e::list<wall_e::error>& search_errors
            );

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index, const wall_e::gram::environment* env);


    // abstract_node interface
public:
    virtual std::ostream &write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const override;
    virtual std::ostream &short_print(std::ostream &stream) const override;
    virtual ast_token_list tokens() const override;
    virtual wall_e::list<wall_e::error> errors() const override;

    // abstract_value_node interface
public:
    virtual wall_e::either<wall_e::error, backend::value *> generate_backend_value(const std::shared_ptr<backend::unit> &unit) override;
};

}

#endif // IMP_NODE_H
