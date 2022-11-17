#ifndef LVALUE_H
#define LVALUE_H

#include <wall_e/src/enums.h>
#include <wall_e/src/lex.h>
#include "../ast_token.h"
namespace km2 {

struct lvalue {
    wall_e_enum_member(kind,
        Exp,
        Id,
        AnonId
    )
private:
    kind m_kind;
    wall_e::lex::token m_token;

    inline lvalue(
            const kind kind,
            const wall_e::lex::token& token
            )
        : m_kind(kind),
          m_token(token) {}
public:
    kind lval_kind() const { return m_kind; }
    const wall_e::lex::token& token() const { return m_token; };

    typedef std::function<wall_e::opt<lvalue>(const wall_e::lex::token&)> factory;

    inline static factory fac(
            const std::string& exp_token_name,
            const std::string& id_token_name,
            const std::string& anon_id_token_name
            ) {
        return [=](const wall_e::lex::token& token) -> wall_e::opt<lvalue> {
            if(token.name == exp_token_name) {
                return lvalue(Exp, token);
            } else if(token.name == id_token_name) {
                return lvalue(Id, token);
            } else if(token.name == anon_id_token_name) {
                return lvalue(AnonId, token);
            } else {
                return std::nullopt;
            }
        };
    };
    inline friend std::ostream& operator<<(std::ostream& stream, const lvalue& lval) {
        return stream << "{ kind: " << lval.m_kind
                      << ", token: " << lval.m_token
                      << " }";
    }

    inline ast_token_type token_type(ast_token_type value_type) const {
        switch (m_kind) {
        case lvalue::Exp: return AstKeyword;
        case lvalue::Id: return value_type;
        case lvalue::AnonId: return value_type;
        case lvalue::__kind_max_value: return wall_e::enums::max_value<ast_token_type>();
        }
        return wall_e::enums::max_value<ast_token_type>();
    }

    inline ast_token_modifier token_modifier() const {
        switch (m_kind) {
        case lvalue::Exp: return wall_e::enums::max_value<ast_token_modifier>();
        case lvalue::Id: return AstDeclaration;
        case lvalue::AnonId: return AstDeclaration;
        case lvalue::__kind_max_value: return wall_e::enums::max_value<ast_token_modifier>();
        }
        return wall_e::enums::max_value<ast_token_modifier>();
    }

    inline std::string pretty_str() const {
        switch (m_kind) {
        case lvalue::Exp: return "<export>";
        case lvalue::Id: return m_token.text;
        case lvalue::AnonId: return "<anonimus>";
        case lvalue::__kind_max_value: break;
        }
        return "<unknown kind>";
    }
};

}

#endif // LVALUE_H
