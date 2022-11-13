#include "arg_node.h"
#include "const_node.h"

#include <iostream>

#include <src/km2/backend/unit/unit.h>
#include "arg_node.h"
#include <src/km2/backend/entities/value.h>

km2::const_node::const_node(
        const wall_e::index& index,
        const std::string &keyword_text,
        const wall_e::text_segment &keyword_segment,
        const std::string &id,
        const wall_e::text_segment &id_segment,
        const std::shared_ptr<arg_node> &value
        )
    : km2::abstract_value_node(index, { value }),
      m_keyword_text(keyword_text),
      m_keyword_segment(keyword_segment),
      m_id(id),
      m_id_segment(id_segment),
      m_value(value) {}

wall_e::gram::argument km2::const_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index) {
    if(debug) std::cout << "km2::const_node::create: " << args << std::endl;
    if(args.size() > 3) {
        if(const auto& keyword_token = args[0].option<wall_e::lex::token>()) {
            if(const auto id = args[1].option<wall_e::lex::token>()) {
                if(const auto value = args[3].option_cast<std::shared_ptr<arg_node>>()) {
                    return std::make_shared<const_node>(index, keyword_token->text, keyword_token->segment(), id->text, id->segment(), *value);
                } else {
                    return std::make_shared<const_node>(index, keyword_token->text, keyword_token->segment(), id->text, id->segment());
                }
            }
            return std::make_shared<const_node>(index, keyword_token->text, keyword_token->segment(), std::string(), wall_e::text_segment());
        }
    }
    return std::make_shared<const_node>(index, std::string(), wall_e::text_segment(), std::string(), wall_e::text_segment());
}

wall_e::list<wall_e::error> km2::const_node::errors() const {
    return {};
}

wall_e::either<wall_e::error, km2::backend::value*> km2::const_node::generate_backend_value(const std::shared_ptr<backend::unit> &unit) {
    if(const auto value = m_value->generate_backend_value(unit)) {
        std::cout << "const node id: " << m_id << " -> " << value << std::endl;
        const auto status = unit->set_arg(m_id, value.right_value());
        if(status == backend::unit::ArgSettingSuccess) {
            return value.right();
        } else if(status == backend::unit::ArgSettingDublicates) {
            return wall_e::left(wall_e::error("ArgSettingDublicates"));
        } else if(status == backend::unit::ArgSettingEmptyStack) {
            return wall_e::left(wall_e::error("ArgSettingEmptyStack"));
        } else {
            return wall_e::left(wall_e::error("ArgSettingUnknownStatus"));
        }
    } else {
        return value.left();
    }
}

std::ostream &km2::const_node::short_print(std::ostream &stream) const {
    return stream << "const_node { id: " << m_id << " }";
}

wall_e::list<km2::ast_token> km2::const_node::tokens() const {
    using namespace km2::literals;
    const auto hover = "**constant** "_md + m_id;
    return wall_e::list<km2::ast_token> {
        ast_token {
            .type = AstKeyword,
            .modifier = wall_e::enums::max_value<ast_token_modifier>(),
            .node_type = wall_e::type_name<const_node>(),
            .hover = hover,
            .text = m_keyword_text,
            .segment = m_keyword_segment
        },
        ast_token {
            .type = AstVariable,
            .modifier = AstDefinition,
            .node_type = wall_e::type_name<const_node>(),
            .hover = hover,
            .text = m_id,
            .segment = m_id_segment
        }
    } + (m_value ? m_value->tokens() : wall_e::list<km2::ast_token> {});
}


std::ostream &km2::const_node::write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const {
    if(fmt == Simple) {
        stream << std::string(ctx.level(), ' ') << "{const_node}:" << std::endl;
        stream << std::string(ctx.level() + 1, ' ') + m_id << std::endl;
        if(m_value) {
            m_value->write(stream, fmt, ctx.new_child("value"));
        } else {
            stream << std::string(ctx.level() + 1, ' ') + "value node not exist" << std::endl;
        }
    } else if(fmt == TreeWriter) {
        stream << ctx.node_begin()
               << "const_node { id: " << m_id << (m_value ? "" : ", no value node") << " }"
               << ctx.node_end()
               << ctx.edge();

        if(m_value) {
            m_value->write(stream, fmt, ctx.new_child("value"));
        }
    }
    return stream;
}
