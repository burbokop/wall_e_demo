#include "arg_node.h"
#include "const_node.h"

#include <iostream>

#include <src/km2/translation_unit/translation_unit.h>
#include "arg_node.h"

km2::const_node::const_node(
        const wall_e::index& index,
        const std::string &id,
        const wall_e::text_segment &id_segment,
        const std::shared_ptr<arg_node> &value
        )
    : km2::abstract_value_node(index, { value }),
      m_id(id),
      m_id_segment(id_segment),
      m_value(value) {}

wall_e::gram::argument km2::const_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index) {
    std::cout << "km2::const_node::create: " << args << std::endl;
    if(args.size() > 3) {
        if(const auto id = args[1].option<wall_e::lex::token>()) {
            if(const auto value = args[3].option_cast<std::shared_ptr<arg_node>>()) {
                return std::make_shared<const_node>(index, id->text, id->segment(), *value);
            } else {
                return std::make_shared<const_node>(index, id->text, id->segment());
            }
        }
    }
    return std::make_shared<const_node>(index, std::string(), wall_e::text_segment());
}

void km2::const_node::print(size_t level, std::ostream &stream) const {
    stream << std::string(level, ' ') << "{const_node}:" << std::endl;
    stream << std::string(level + 1, ' ') + m_id << std::endl;
    if(m_value) {
        m_value->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "value node not exist" << std::endl;
    }
}

wall_e::list<wall_e::error> km2::const_node::errors() const {
    return {};
}

wall_e::either<wall_e::error, llvm::Value *> km2::const_node::generate_llvm(const std::shared_ptr<translation_unit> &unit) {
    if(const auto value = m_value->generate_llvm(unit)) {
        const auto status = unit->set_arg(m_id, value.right_value());
        if(status == translation_unit::ArgSettingSuccess) {
            return value.right();
        } else if(status == translation_unit::ArgSettingDublicates) {
            return wall_e::left(wall_e::error("ArgSettingDublicates"));
        } else if(status == translation_unit::ArgSettingEmptyStack) {
            return wall_e::left(wall_e::error("ArgSettingEmptyStack"));
        } else {
            return wall_e::left(wall_e::error("ArgSettingUnknownStatus"));
        }
    } else {
        return value.left();
    }
}

void km2::const_node::short_print(std::ostream &stream) const {
    stream << "const_node { id: " << m_id << " }";
}

wall_e::list<km2::ast_token> km2::const_node::tokens() const {
    return wall_e::list<km2::ast_token> {
        ast_token {
            .type = AstVariable,
            .node_type = wall_e::type_name<const_node>(),
            .hover = "<b>constant</b> " + m_id,
            .text = m_id,
            .segment = m_id_segment
        }
    } + (m_value ? m_value->tokens() : wall_e::list<km2::ast_token> {});
}
