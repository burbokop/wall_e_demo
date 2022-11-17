#include "arg_node.h"
#include "const_node.h"

#include <iostream>

#include <src/km2/backend/unit/unit.h>
#include "arg_node.h"
#include <src/km2/backend/entities/value.h>

km2::const_node::const_node(
        const wall_e::index& index,
        const std::shared_ptr<arg_node> &value
        )
    : km2::abstract_value_node(index, { value }),
      m_value(value) {}

km2::abstract_node::factory km2::const_node::create() {
    return [](const wall_e::gram::arg_vector &args, const wall_e::index& index, const wall_e::gram::environment* env) -> wall_e::gram::argument {
        if(debug) std::cout << "km2::const_node::create: " << args << std::endl;
        if(args.size() > 0) {
                if(const auto rvalue = args[0].option_cast<std::shared_ptr<arg_node>>()) {
                    return std::make_shared<const_node>(index, *rvalue);
                } else {
                    return std::make_shared<const_node>(index);
                }
            return nullptr;
        }
        return nullptr;
    };
}

km2::markup_string km2::const_node::hover() const {
    using namespace km2::literals;
    if(const auto& lvalue = lval()) {
        switch (lvalue->lval_kind()) {
        case lvalue::Exp: return "**export constant**"_md;
        case lvalue::Id: return "**constant** "_md + lvalue->token().text;
        case lvalue::AnonId: return "**anonimus constant**"_md;
        case lvalue::__kind_max_value: break;
        }
    }
    return "**unknown kind constant**"_md;
}

wall_e::list<wall_e::error> km2::const_node::errors() const {
    return m_value ? m_value->errors() : wall_e::list<wall_e::error> {};
}

wall_e::either<wall_e::error, km2::backend::value*> km2::const_node::generate_backend_value(const std::shared_ptr<backend::unit> &unit) {
    if(const auto value = m_value->generate_backend_value(unit)) {
        std::cout << "const node id: " << lval() << " -> " << value << std::endl;
        if(const auto& name = lval().map<std::string>([](const lvalue& v) { return v.token().text; })) {
            const auto status = unit->set_arg(*name, value.right_value());
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
            return wall_e::left(wall_e::error("const does not have lvalue"));
        }
    } else {
        return value.left();
    }
}

std::ostream &km2::const_node::short_print(std::ostream &stream) const {
    return stream << "const_node { id: " << lval() << " }";
}

wall_e::list<km2::ast_token> km2::const_node::tokens() const {
    return m_value ? m_value->tokens() : wall_e::list<km2::ast_token> {};
}


std::ostream &km2::const_node::write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const {
    if(fmt == Simple) {
        stream << std::string(ctx.level(), ' ') << "{const_node}:" << std::endl;
        //stream << std::string(ctx.level() + 1, ' ') + m_id << std::endl;
        //if(m_rvalue) {
        //    m_rvalue->write(stream, fmt, ctx.new_child("value"));
        //} else {
        //    stream << std::string(ctx.level() + 1, ' ') + "value node not exist" << std::endl;
        //}
    } else if(fmt == TreeWriter) {
        stream << ctx.node_begin()
               << "const_node { " << lval().map_member_func<std::string>(&lvalue::pretty_str) << " }"
               << ctx.node_end()
               << ctx.edge();

        if(m_value) {
            m_value->write(stream, fmt, ctx.new_child("rvalue"));
        }
    }
    return stream;
}

km2::ast_token_type km2::const_node::rvalue_type() const {
    return AstVariable;
}
