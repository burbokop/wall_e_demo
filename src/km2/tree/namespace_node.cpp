#include "namespace_node.h"


#include <iostream>

#include <src/km2/translation_unit/translation_unit.h>

#include <src/km2/translation_unit/capabilities/namespace_capability.h>


km2::namespace_node::namespace_node(const wall_e::index &index, const std::string &name, const std::shared_ptr<block_node>& block_node)
    : abstract_value_node(index, { block_node }),
      m_name(name),
      m_block_node(block_node),
      m_context({}) {}

km2::abstract_node::factory km2::namespace_node::create(const std::string &name_token) {
    return [name_token](const wall_e::gram::arg_vector &args, const wall_e::index& index) -> wall_e::gram::argument {
        std::cout << wall_e::type_name<namespace_node>() << "::create: " << args << std::endl;
        if (args.size() > 2) {
            if(const auto& name_with_ob = args[1].option<wall_e::gram::arg_vector>()) {
                if(name_with_ob->size() > 0) {
                    const auto& name_or_ob = (*name_with_ob)[0].option<wall_e::lex::token>();
                    if(name_with_ob->size() > 1 && name_or_ob->name == name_token) {
                        return std::make_shared<namespace_node>(index, name_or_ob->text, args[2].value<std::shared_ptr<block_node>>());
                    }
                }
            }
            return std::make_shared<namespace_node>(index, std::string(), args[2].value<std::shared_ptr<block_node>>());
        }
        return std::make_shared<namespace_node>(index);
    };
}


wall_e::either<wall_e::error, llvm::Value *> km2::namespace_node::generate_llvm(const std::shared_ptr<km2::translation_unit> &unit) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if(m_block_node) {
        unit->cap<namespace_capability>()->begin_namespace(full_name());

        if(const auto result = m_block_node->generate_llvm(unit)) {
        } else {
            return result.left();
        }

        return wall_e::right<llvm::Value *>(unit->cap<namespace_capability>()->end_namespace());
    }
    return wall_e::right<llvm::Value *>(nullptr);
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


std::list<wall_e::error> km2::namespace_node::errors() const {
    return {};
}


std::string km2::namespace_node::name() const {
    return m_name;
}

std::list<std::string> km2::namespace_node::full_name() const {
    if(const auto ns = nearest_ancestor<namespace_node>()) {
        auto result = ns->full_name();
        result.push_back(m_name);
        return result;
    } else {
        return { m_name };
    }
}

const km2::context &km2::namespace_node::context() const {
    return m_context;
}

void km2::namespace_node::short_print(std::ostream &stream) const {
    stream << "namespace_node: { name: " << m_name << " }";
}
