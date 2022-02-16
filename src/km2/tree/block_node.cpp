#include "block_node.h"
#include  <iostream>

km2::block_node::block_node(const std::shared_ptr<stmt_node> &stmt, const std::shared_ptr<block_node> &next_node)
    : km2::abstract_value_node(cast_to_children(std::vector { stmt }, std::vector { next_node })),
      m_stmt(stmt),
      m_next_node(next_node),
      m_context(stmt ? stmt->context() : km2::context({})),
      m_acc_context(stmt
                    ? (next_node
                       ? stmt->context() + next_node->acc_context()
                       : stmt->context())
                    : km2::context({})),
      m_mutable_context(km2::context({})) {}

wall_e::gram::argument km2::block_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index) {
    std::cout << "km2::block_node::create" << std::endl;
    if (args.size() > 0) {
        const auto cmd = args[0].value_or<std::shared_ptr<stmt_node>>(nullptr);
        if (args.size() > 2) {
            const auto next_block = args[2].value_or<std::shared_ptr<block_node>>(nullptr);
            return std::make_shared<block_node>(cmd, next_block);
        } else {
            return std::make_shared<block_node>(cmd, nullptr);
        }
    }
    return std::make_shared<block_node>(nullptr);
}


wall_e::either<
wall_e::error,
llvm::Value *
> km2::block_node::generate_llvm(const std::shared_ptr<km2::translation_unit> &unit) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if(m_stmt) {
        if(const auto cmd_result = m_stmt->generate_llvm(unit)) {
        } else {
            return cmd_result.left();
        }
    }
    if(m_next_node) {
        return m_next_node->generate_llvm(unit);
    }
    return wall_e::right<llvm::Value *>(nullptr);
}


void km2::block_node::print(size_t level, std::ostream &stream) {
    stream << std::string(level, ' ') << "{block_node}:" << std::endl;
    if(m_stmt) {
        m_stmt->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "cmd not exist" << std::endl;
    }
    if(m_next_node) {
        m_next_node->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "next node not exist" << std::endl;
    }
}


std::list<wall_e::error> km2::block_node::errors() const {
    return { wall_e::error("err not implemented") };
}


std::optional<wall_e::error> km2::block_node::add_function(const function &function) {
    if(const auto& overload = find_overload_in_block_seq(function.namespace_stack(), function.name())) {
        if(const auto error = overload->add_value(function.value(), segment())) {
            return error;
        }
    } else {
        if(const auto error = mutable_context().add_overload(function, segment())) {
            return error;
        }
    }
    return std::nullopt;
}

std::shared_ptr<km2::overload> km2::block_node::find_overload_in_block_seq(const std::list<std::string> &namespace_stack, const std::string &name) {
    if(const auto& overload = mutable_context().find_overload(namespace_stack, name)) {
        return overload;
    } else if(const auto& p = parent_as<block_node>()) {
        return p->find_overload_in_block_seq(namespace_stack, name);
    } else {
        return nullptr;
    }
}

std::shared_ptr<km2::overload> km2::block_node::find_overload_in_whole_tree(
        const std::list<std::string> &namespace_stack,
        const std::string &name
        ) {
    return nullptr;
}

const km2::context &km2::block_node::context() const { return m_context; }
const km2::context &km2::block_node::acc_context() const { return m_acc_context; }
km2::context &km2::block_node::mutable_context() { return m_mutable_context; }
