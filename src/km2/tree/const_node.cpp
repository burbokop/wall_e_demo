#include "arg_node.h"
#include "const_node.h"

#include <iostream>

#include <src/km2/module.h>
#include "arg_node.h"

km2::const_node::const_node(const std::string &id, const std::shared_ptr<arg_node> &value)
    : km2::abstract_value_node({ value }),
      m_id(id),
      m_value(value) {}

wall_e::gram::argument km2::const_node::create(const wall_e::gram::arg_vector &args) {
    std::cout << "km2::const_node::create: " << args << std::endl;
    if(args.size() > 3) {
        if(const auto id = args[1].option<wall_e::lex::token>()) {
            if(const auto value = args[3].option_cast<std::shared_ptr<arg_node>>()) {
                return std::make_shared<const_node>(id->text, *value);
            } else {
                return std::make_shared<const_node>(id->text);
            }
        }
    }
    return std::make_shared<const_node>(std::string());
}

void km2::const_node::print(size_t level, std::ostream &stream) {
    stream << std::string(level, ' ') << "{const_node}:" << std::endl;
    stream << std::string(level + 1, ' ') + m_id << std::endl;
    if(m_value) {
        m_value->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "value node not exist" << std::endl;
    }
}

std::list<wall_e::error> km2::const_node::errors() {
    return {};
}

wall_e::either<wall_e::error, llvm::Value *> km2::const_node::generate_llvm(const std::shared_ptr<module> &module) {
    if(const auto value = m_value->generate_llvm(module)) {
        const auto status = module->setArg(m_id, value.right_value());
        if(status == module::ArgSettingSuccess) {
            return value.right();
        } else if(status == module::ArgSettingDublicates) {
            return wall_e::left(wall_e::error("ArgSettingDublicates"));
        } else if(status == module::ArgSettingEmptyStack) {
            return wall_e::left(wall_e::error("ArgSettingEmptyStack"));
        } else {
            return wall_e::left(wall_e::error("ArgSettingUnknownStatus"));
        }
    } else {
        return value.left();
    }
}
