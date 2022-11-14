#include "namespace_node.h"


#include <iostream>

#include <src/km2/backend/unit/unit.h>

#include <src/km2/backend/unit/capabilities/namespace_capability.h>
#include "wall_e/src/macro.h"


km2::namespace_node::namespace_node(const wall_e::index &index,
        const wall_e::opt<std::string> &exp_keyword,
        const wall_e::text_segment &exp_keyword_segment,
        const std::string &keyword,
        const wall_e::text_segment &keyword_segment,
        const std::string &name,
        const wall_e::text_segment &name_segment,
        const std::shared_ptr<block_node>& block_node
        )
    : abstract_value_node(index, { block_node }),
      m_exp_keyword(exp_keyword),
      m_exp_keyword_segment(exp_keyword_segment),
      m_keyword(keyword),
      m_keyword_segment(keyword_segment),
      m_name(name),
      m_name_segment(name_segment),
      m_block_node(block_node),
      m_context({}) {}

km2::abstract_node::factory km2::namespace_node::create(const std::string& exp_token, const std::string &name_token) {
    return [name_token, exp_token](const wall_e::gram::arg_vector &args, const wall_e::index& index, const wall_e::gram::environment* env) -> wall_e::gram::argument {
        if(debug) std::cout << wall_e::type_name<namespace_node>() << "::create: " << args << std::endl;
        if (args.size() > 3) {
            const auto exp = args[0].option<wall_e::lex::token>()
                    .filter([exp_token](const wall_e::lex::token& t) { return t.name == exp_token; });

            if(const auto& keyword_token = args[1].option<wall_e::lex::token>()) {
                if(const auto& name_with_ob = args[2].option<wall_e::gram::arg_vector>()) {
                    if(name_with_ob->size() > 0) {
                        const auto& name_or_ob = (*name_with_ob)[0].option<wall_e::lex::token>();
                        if(name_with_ob->size() > 1 && name_or_ob->name == name_token) {
                            return std::make_shared<namespace_node>(
                                        index,
                                        exp.map<std::string>([](const wall_e::lex::token& t){ return t.text; }),
                                        exp.map<wall_e::text_segment>([](const wall_e::lex::token& t){ return t.segment(); }).value_or(wall_e::text_segment()),
                                        keyword_token->text,
                                        keyword_token->segment(),
                                        name_or_ob->text,
                                        name_or_ob->segment(),
                                        args[3].value<std::shared_ptr<block_node>>()
                                    );
                        }
                    }
                }
                if(const auto& block = args[3].option<std::shared_ptr<block_node>>()) {
                    return std::make_shared<namespace_node>(
                                index,
                                exp.map<std::string>([](const wall_e::lex::token& t){ return t.text; }),
                                exp.map<wall_e::text_segment>([](const wall_e::lex::token& t){ return t.segment(); }).value_or(wall_e::text_segment()),
                                keyword_token->text,
                                keyword_token->segment(),
                                std::string(),
                                wall_e::text_segment(),
                                *block
                                );
                }
            }
        }
        return std::make_shared<namespace_node>(index, std::nullopt, wall_e::text_segment(), std::string(), wall_e::text_segment());
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

wall_e::list<wall_e::error> km2::namespace_node::errors() const {
    return m_block_node ? m_block_node->errors() : wall_e::list<wall_e::error> {};
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

std::ostream &km2::namespace_node::short_print(std::ostream &stream) const {
    return stream << "namespace_node: { name: " << m_name << " }";
}

km2::ast_token_list km2::namespace_node::tokens() const {
    using namespace km2::literals;
    const auto hover = m_name.empty() ? "**anonimus namespace**"_md : "**namespace** "_md + m_name;
    return (m_exp_keyword ? ast_token_list {
        ast_token {
            .type = AstKeyword,
            .modifier = wall_e::enums::max_value<ast_token_modifier>(),
            .node_type = wall_e::type_name<namespace_node>(),
            .hover = hover,
            .text = *m_exp_keyword,
            .segment = m_exp_keyword_segment
        }
    } : ast_token_list {}) + ast_token_list {
        ast_token {
            .type = AstKeyword,
            .modifier = wall_e::enums::max_value<ast_token_modifier>(),
            .node_type = wall_e::type_name<namespace_node>(),
            .hover = hover,
            .text = m_keyword,
            .segment = m_keyword_segment
        },
    } + (m_name.empty() ? ast_token_list {} : ast_token_list {
        ast_token {
            .type = AstNamespace,
            .modifier = AstDeclaration,
            .node_type = wall_e::type_name<namespace_node>(),
            .hover = hover,
            .text = m_name,
            .segment = m_name_segment
        }
    }) + (m_block_node ? m_block_node->tokens() : ast_token_list {});
}


std::ostream &km2::namespace_node::write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const {
    if(fmt == Simple) {
        stream << std::string(ctx.level(), ' ') << "{namespace_node}:" << std::endl;
        stream << std::string(ctx.level() + 1, ' ') << "name: " << m_name << std::endl;
        if(m_block_node) {
            m_block_node->write(stream, fmt, ctx.new_child("block"));
        } else {
            stream << std::string(ctx.level() + 1, ' ') + "block node not exist" << std::endl;
        }
    } else if(fmt == TreeWriter) {
        stream << ctx.node_begin()
               << "namespace_node: { name: " << m_name << (m_block_node ? "" : ", no block") << " }"
               << ctx.node_end()
               << ctx.edge();

        if(m_block_node) {
            m_block_node->write(stream, fmt, ctx.new_child("block"));
        }
    }
    return stream;
}
