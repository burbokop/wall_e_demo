#ifndef AST_TOKEN_H
#define AST_TOKEN_H

#if __has_include(<wall_e/src/enums.h>)
    #include <wall_e/src/enums.h>
#else
    #include <wall_e/enums.h>
#endif

#if __has_include(<wall_e/src/models/text_segment.h>)
    #include <wall_e/src/models/text_segment.h>
#else
    #include <wall_e/models/text_segment.h>
#endif

#if __has_include(<wall_e/src/utility/collections.h>)
    #include <wall_e/src/utility/collections.h>
#else
    #include <wall_e/utility/collections.h>
#endif

#if __has_include(<src/km2/utility/markup_string.h>)
    #include <src/km2/utility/markup_string.h>
#else
    #include <km2/utility/markup_string.h>
#endif

namespace km2 {

wall_e_enum(ast_token_type,
    AstNamespace = 0,
    AstType,
    AstClass,
    AstEnum,
    AstInterface,
    AstStruct,
    AstTypeParameter,
    AstParameter,
    AstVariable,
    AstProperty,
    AstEnumMember,
    AstEvent,
    AstFunction,
    AstMethod,
    AstMacro,
    AstKeyword,
    AstModifier,
    AstComment,
    AstString,
    AstNumber,
    AstRegexp,
    AstOperator,
    AstDecorator
)
wall_e_enum(ast_token_modifier,
    AstDeclaration = 0,
    AstDefinition,
    AstReadonly,
    AstStatic,
    AstDeprecated,
    AstAbstract,
    AstAsync,
    AstModification,
    AstDocumentation,
    AstDefaultLibrary
)


struct ast_token {
    ast_token_type type;
    ast_token_modifier modifier;
    std::string node_type;
    markup_string hover;
    std::string text;
    wall_e::text_segment segment;

    friend inline std::ostream& operator<<(std::ostream& stream, const ast_token& token) {
        return stream << "{ type: " << token.type
                      << ", node_type: " << token.node_type
                      << ", hover: " << token.hover
                      << ", text: " << token.text
                      << ", segment: " << token.segment
                      << " }";
    }
};

typedef wall_e::list<ast_token> ast_token_list;

std::string to_string(const ast_token_list& tokens);

}

#endif // AST_TOKEN_H
