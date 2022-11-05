#include "internal_block_node.h"
#include <iostream>
#include "wall_e/src/macro.h"

km2::internal_block_node::internal_block_node(const wall_e::index &index, std::shared_ptr<stmt_node> stmt, std::shared_ptr<internal_block_node> next_node)
    : km2::abstract_value_node(index, cast_to_children(std::vector { stmt }, std::vector { next_node })),
      m_stmt(stmt),
      m_next_node(next_node)
{}

wall_e::gram::argument km2::internal_block_node::create(const wall_e::gram::arg_vector &args, const wall_e::index &index) {
    if(debug) std::cout << "km2::internal_block_node::create" << std::endl;
    if (args.size() > 0) {
        const auto stmt = args[0].value_or<std::shared_ptr<stmt_node>>(nullptr);
        if (args.size() > 2) {
            const auto next_block = args[2].value_or<std::shared_ptr<internal_block_node>>(nullptr);
            return std::make_shared<internal_block_node>(index, stmt, next_block);
        } else {
            return std::make_shared<internal_block_node>(index, stmt, nullptr);
        }
    }
    return std::make_shared<internal_block_node>(index, nullptr);
}

wall_e::either<
    wall_e::error,
    km2::backend::value*
> km2::internal_block_node::generate_backend_value(const std::shared_ptr<backend::unit> &unit) {
    if(debug) std::cout << wall_e_this_function << std::endl;
    if(m_stmt) {
        if(const auto cmd_result = m_stmt->generate_backend_value(unit)) {
        } else {
            return cmd_result.left();
        }
    }
    if(m_next_node) {
        return m_next_node->generate_backend_value(unit);
    }
    return wall_e::right<backend::value*>(nullptr);
}

void km2::internal_block_node::print(size_t level, std::ostream &stream) const {
    stream << std::string(level, ' ') << "{internal_block_node}:" << std::endl;
    if(m_stmt) {
        m_stmt->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "stmt not exist" << std::endl;
    }
    if(m_next_node) {
        m_next_node->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "next node not exist" << std::endl;
    }
}


wall_e::list<wall_e::error> km2::internal_block_node::errors() const {
    return { wall_e::error("err not implemented") };
}

void km2::internal_block_node::short_print(std::ostream &stream) const {
    stream << "internal_block_node { recursive content }";
}


wall_e::list<km2::ast_token> km2::internal_block_node::tokens() const {
    return
            (m_stmt ? m_stmt->tokens() : wall_e::list<ast_token> {}) +
            (m_next_node ? m_next_node->tokens() : wall_e::list<ast_token> {});
}
