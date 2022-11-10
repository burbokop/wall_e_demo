#include "namespace_node.h"


#include <iostream>

#include <src/km2/backend/unit/unit.h>

#include <src/km2/backend/unit/capabilities/namespace_capability.h>
#include "wall_e/src/macro.h"


km2::namespace_node::namespace_node(const wall_e::index &index,
        const std::string &keyword,
        const wall_e::text_segment &keyword_segment,
        const std::string &name,
        const wall_e::text_segment &name_segment,
        const std::shared_ptr<block_node>& block_node
        , const bool exp)
    : abstract_value_node(index, { block_node }),
      m_keyword(keyword),
      m_keyword_segment(keyword_segment),
      m_name(name),
      m_name_segment(name_segment),
      m_block_node(block_node),
      m_context({}) {}

km2::abstract_node::factory km2::namespace_node::create(const std::string &name_token) {
    return [name_token](const wall_e::gram::arg_vector &args, const wall_e::index& index) -> wall_e::gram::argument {
        if(debug) std::cout << wall_e::type_name<namespace_node>() << "::create: " << args << std::endl;
        if (args.size() > 3) {
            const auto exp = args[0].option<wall_e::lex::token>()
                    .exists([](const wall_e::lex::token& t) { return t.name == "TOK_EXP"; });

            if(const auto& keyword_token = args[1].option<wall_e::lex::token>()) {
                if(const auto& name_with_ob = args[2].option<wall_e::gram::arg_vector>()) {
                    if(name_with_ob->size() > 0) {
                        const auto& name_or_ob = (*name_with_ob)[1].option<wall_e::lex::token>();
                        if(name_with_ob->size() > 1 && name_or_ob->name == name_token) {
                            return std::make_shared<namespace_node>(
                                        index,
                                        keyword_token->text,
                                        keyword_token->segment(),
                                        name_or_ob->text,
                                        name_or_ob->segment(),
                                        args[3].value<std::shared_ptr<block_node>>(),
                                        exp
                                    );
                        }
                    }
                }
                if(const auto& block = args[3].option<std::shared_ptr<block_node>>()) {
                    return std::make_shared<namespace_node>(
                                index,
                                keyword_token->text,
                                keyword_token->segment(),
                                std::string(),
                                wall_e::text_segment(),
                                *block,
                                exp
                                );
                }
            }
        }
        return std::make_shared<namespace_node>(index, std::string(), wall_e::text_segment());
    };
}

wall_e::either<
    wall_e::error,
    km2::backend::value*
> km2::namespace_node::generate_backend_value(const std::shared_ptr<km2::backend::unit> &unit) {
    if(debug) std::cout << wall_e_this_function << std::endl;
    if(m_block_node) {
        unit->cap<backend::namespace_capability>()->begin_namespace(full_name());

        if(const auto result = m_block_node->generate_backend_value(unit)) {
        } else {
            return result.left();
        }

        return wall_e::right<backend::value*>(unit->cap<backend::namespace_capability>()->end_namespace());
    }
    return wall_e::right<backend::value*>(nullptr);
}

void km2::namespace_node::print(size_t level, std::ostream &stream) const {
    stream << std::string(level, ' ') << "{namespace_node}:" << std::endl;
    stream << std::string(level + 1, ' ') << "name: " << m_name << std::endl;
    if(m_block_node) {
        m_block_node->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "block node not exist" << std::endl;
    }
}


wall_e::list<wall_e::error> km2::namespace_node::errors() const {
    return {};
}


std::string km2::namespace_node::name() const {
    return m_name;
}

wall_e::str_list km2::namespace_node::full_name() const {
    if(const auto ns = nearest_ancestor<namespace_node>()) {
        auto result = ns->full_name();
        result.push_back(m_name);
        return result;
    } else {
        return { m_name };
    }
}

const km2::backend::context &km2::namespace_node::context() const {
    return m_context;
}

void km2::namespace_node::short_print(std::ostream &stream) const {
    stream << "namespace_node: { name: " << m_name << " }";
}

km2::ast_token_list km2::namespace_node::tokens() const {
    using namespace km2::literals;
    const auto hover = m_name.empty() ? "**anonimus namespace**"_md : "**namespace** "_md + m_name;
    return ast_token_list {
        ast_token {
            .type = AstKeyword,
            .node_type = wall_e::type_name<namespace_node>(),
            .hover = hover,
            .text = m_keyword,
            .segment = m_keyword_segment
        },
    } + (m_name.empty() ? ast_token_list {} : ast_token_list {
        ast_token {
            .type = AstNamespace,
            .node_type = wall_e::type_name<namespace_node>(),
            .hover = hover,
            .text = m_name,
            .segment = m_name_segment
        }
    }) + (m_block_node ? m_block_node->tokens() : ast_token_list {});
}
