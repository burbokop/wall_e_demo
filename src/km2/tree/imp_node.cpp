#include "imp_node.h"
#include "substitution_node.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include <src/km2/km2.h>

std::shared_ptr<const km2::abstract_value_node> km2::imp_node::eval_mod_exp_root_cache() const {
    if(const auto& mod = *m_module_cresult.get()) {
        if(const auto& rn = mod->root_node()) {
            if(const auto s = rn->find<substitution_node>([](const std::shared_ptr<const substitution_node>& s) -> bool {
                return s->lvalue().lval_kind() == lvalue::Exp;
            })) {
                return s->rvalue();
            }
        }
    }
    return nullptr;
}

km2::imp_node::imp_node(const wall_e::index &index,
        const wall_e::lex::token &keyword_token,
        const wall_e::lex::token &name_token,
        const wall_e::opt<compilation_result> &module_cresult,
        const wall_e::list<wall_e::error> &search_errors
        )
    : abstract_value_node(index, {}, keyword_token.segment() + name_token.segment()),
      m_keyword_token(keyword_token),
      m_name_token(name_token),
      m_module_cresult(wall_e::opt_box<compilation_result>::from(module_cresult)),
      m_search_errors(search_errors) {}

wall_e::gram::argument km2::imp_node::create(const wall_e::gram::arg_vector &args, const wall_e::index &index, const wall_e::gram::environment* env) {
    if(debug) std::cout << "km2::imp_node::create: " << args << std::endl;
    if(args.size() > 1) {
        const auto module_name_token = args[1].option<wall_e::lex::token>();

        if(module_name_token) {
            const auto& module_uri = std::filesystem::path(env->uri()).parent_path() / (module_name_token->text + ".km2");
            if(std::filesystem::exists(module_uri) && std::filesystem::is_regular_file(module_uri)) {
                std::ifstream input(module_uri);
                return std::make_shared<imp_node>(
                            index,
                            args[0].value_or<wall_e::lex::token>(),
                            *module_name_token,
                            km2::compile(nullptr, input, module_uri.string(), {}),
                            wall_e::list<wall_e::error>()
                        );
            } else {
                return std::make_shared<imp_node>(
                            index,
                            args[0].value_or<wall_e::lex::token>(),
                           *module_name_token,
                            std::nullopt,
                            wall_e::list<wall_e::error> {
                                wall_e::error(
                                            "module '" + module_name_token->text + "' not found in '" + module_uri.string() + "'",
                                            wall_e::error::Err,
                                            wall_e::error::Semantic,
                                            0,
                                            module_name_token->segment()
                                            )
                            }
                );
            }
        }
    }
    return std::make_shared<imp_node>(
                index,
                wall_e::lex::token(),
                wall_e::lex::token(),
                std::nullopt,
                wall_e::list<wall_e::error>()
                );
}


std::ostream &km2::imp_node::write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const {
    short_print(stream << ctx.node_begin()) << ctx.node_end() << ctx.edge();
    const auto& cres = *m_module_cresult.get();
    if(cres && cres->root_node()) {
        cres->root_node()->write(stream, fmt, ctx.new_child("module"));
    }
    if(const auto& exp_root = mod_exp_root()) {
        exp_root->write(stream, fmt, ctx.new_child("module public api"));
    }
    return stream;
}

std::ostream &km2::imp_node::short_print(std::ostream &stream) const {
    return stream << "imp_node { " << m_name_token.text << " }";
}

km2::ast_token_list km2::imp_node::tokens() const {
    return wall_e::list<km2::ast_token> {
        ast_token {
            .type = AstKeyword,
            .modifier = wall_e::enums::max_value<ast_token_modifier>(),
            .node_type = wall_e::type_name<imp_node>(),
            .hover = hover(),
            .text = m_keyword_token.text,
            .segment = m_keyword_token.segment()
        },
        ast_token {
            .type = AstMacro /** TODO use type depending on what exported (namespace, variable, function, ...) */,
            .modifier = wall_e::enums::max_value<ast_token_modifier>(),
            .node_type = wall_e::type_name<imp_node>(),
            .hover = hover(),
            .text = m_name_token.text,
            .segment = m_name_token.segment()
        }
    };
}

wall_e::list<wall_e::error> km2::imp_node::errors() const {
    const auto& cres = *m_module_cresult.get();
    if(cres) {
        if(cres->errors().contains([](const wall_e::error& err) { return err.sev() == wall_e::error::Err; })) {
            return m_search_errors
                    .with(wall_e::error(
                              "Errors detected in module '" + m_name_token.text + "'",
                              wall_e::error::Err,
                              wall_e::error::Semantic,
                              0,
                              m_name_token.segment()
                              ))
                    .with(cres->errors());
        } else {
            return m_search_errors.with(cres->errors());
        }
    }
    return m_search_errors;
}

wall_e::either<wall_e::error, km2::backend::value *> km2::imp_node::generate_backend_value(const std::shared_ptr<backend::unit> &unit) {
    return wall_e::left(wall_e::error("not implemented", wall_e::error::Err, wall_e::error::Semantic, 0, segment()));
}


km2::ast_token_type km2::imp_node::rvalue_type() const {
    return wall_e::enums::null;
}

km2::markup_string km2::imp_node::hover() const {
    using namespace km2::literals;
    return "**module** "_md + m_name_token.text;
}
