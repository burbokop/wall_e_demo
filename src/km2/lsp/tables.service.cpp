#include "service.h"

const wall_e::map<std::string, km2::lsp::semantic_token_type> &km2::lsp::default_semantic_token_types_map() {
    static const wall_e::map<std::string, semantic_token_type> res = {
        { "TOK_WAIT", semantic_token_type::Keyword },
        { "TOK_ASM", semantic_token_type::Keyword },
        { "TOK_NUMBER", semantic_token_type::Keyword },
        { "TOK_STRING", semantic_token_type::Keyword },
        { "TOK_CONST", semantic_token_type::Keyword },
        { "TOK_NAMESPACE", semantic_token_type::Keyword },
        { "TOK_UNSIGNED", semantic_token_type::Keyword },
        { "TOK_SIGNED", semantic_token_type::Keyword },
        { "TOK_FLOAT", semantic_token_type::Keyword },
        { "TOK_DOUBLE", semantic_token_type::Keyword },
        { "STRING_LITERAL", semantic_token_type::String },
        { "ID", semantic_token_type::Variable },
        { "FLOAT_LITERAL", semantic_token_type::Number },
        { "INT_LITERAL", semantic_token_type::Number },
        /** { "OP", no semantic token }, */
        /** { "EP", no semantic token }, */
        /** { "OB", no semantic token }, */
        /** { "EB", no semantic token }, */
        /** { "DOUBLE_EQUALS", no semantic token }, */
        /** { "EQUALS", no semantic token }, */
        /** { "TOK_PLUS", no semantic token }, */
        /** { "TOK_MINUS", no semantic token }, */
        /** { "TOK_MUL", no semantic token }, */
        /** { "TOK_DIV", no semantic token }, */
        /** { "SEMICOLON", no semantic token }, */
        /** { "COLON", no semantic token }, */
        /** { "COMA", no semantic token }, */
        /** { "THREE_DOT", no semantic token }, */
        /** { "ignore", no semantic token } */
    };
    return res;
}

const wall_e::map<std::string, km2::lsp::semantic_token_modifier>& km2::lsp::default_semantic_token_modifiers_map() {
    static const wall_e::map<std::string, semantic_token_modifier> res = {
        /** { "TOK_WAIT", no semantic token modifier }, */
        /** { "TOK_ASM", no semantic token modifier }, */
        /** { "TOK_NUMBER", no semantic token modifier }, */
        /** { "TOK_STRING", no semantic token modifier }, */
        /** { "TOK_CONST", no semantic token modifier }, */
        /** { "TOK_NAMESPACE", no semantic token modifier }, */
        /** { "TOK_UNSIGNED", no semantic token modifier }, */
        /** { "TOK_SIGNED", no semantic token modifier }, */
        /** { "TOK_FLOAT", no semantic token modifier }, */
        /** { "TOK_DOUBLE", no semantic token modifier }, */
        /** { "STRING_LITERAL", no semantic token modifier }, */
        /** { "ID", no semantic token modifier }, */
        /** { "FLOAT_LITERAL", no semantic token modifier }, */
        /** { "INT_LITERAL", no semantic token modifier }, */
        /** { "OP", no semantic token modifier }, */
        /** { "EP", no semantic token modifier }, */
        /** { "OB", no semantic token modifier }, */
        /** { "EB", no semantic token modifier }, */
        /** { "DOUBLE_EQUALS", no semantic token modifier }, */
        /** { "EQUALS", no semantic token modifier }, */
        /** { "TOK_PLUS", no semantic token modifier }, */
        /** { "TOK_MINUS", no semantic token modifier }, */
        /** { "TOK_MUL", no semantic token modifier }, */
        /** { "TOK_DIV", no semantic token modifier }, */
        /** { "SEMICOLON", no semantic token modifier }, */
        /** { "COLON", no semantic token modifier }, */
        /** { "COMA", no semantic token modifier }, */
        /** { "THREE_DOT", no semantic token modifier }, */
        /** { "ignore", no semantic token modifier } */
    };
    return res;
}

const wall_e::map<km2::ast_token_type, km2::lsp::semantic_token_type> &km2::lsp::default_ast_semantic_token_types_map() {
    static const wall_e::map<ast_token_type, semantic_token_type> res = {
        { AstNamespace,     semantic_token_type::Namespace },
        { AstType,          semantic_token_type::Type },
        { AstClass,         semantic_token_type::Class },
        { AstEnum,          semantic_token_type::Enum },
        { AstInterface,     semantic_token_type::Interface },
        { AstStruct,        semantic_token_type::Struct },
        { AstTypeParameter, semantic_token_type::TypeParameter },
        { AstParameter,     semantic_token_type::Parameter },
        { AstVariable,      semantic_token_type::Variable },
        { AstProperty,      semantic_token_type::Property },
        { AstEnumMember,    semantic_token_type::EnumMember },
        { AstEvent,         semantic_token_type::Event },
        { AstFunction,      semantic_token_type::Function },
        { AstMethod,        semantic_token_type::Method },
        { AstMacro,         semantic_token_type::Macro },
        { AstKeyword,       semantic_token_type::Keyword },
        { AstModifier,      semantic_token_type::Modifier },
        { AstComment,       semantic_token_type::Comment },
        { AstString,        semantic_token_type::String },
        { AstNumber,        semantic_token_type::Number },
        { AstRegexp,        semantic_token_type::Regexp },
        { AstOperator,      semantic_token_type::Operator },
        { AstDecorator,     semantic_token_type::Decorator },
    };
    return res;
}

const wall_e::map<km2::ast_token_modifier, km2::lsp::semantic_token_modifier>& km2::lsp::default_ast_semantic_token_modifiers_map() {
    static const wall_e::map<ast_token_modifier, semantic_token_modifier> res = {
        { AstDeclaration,    semantic_token_modifier::Declaration },
        { AstDefinition,     semantic_token_modifier::Definition },
        { AstReadonly,       semantic_token_modifier::Readonly },
        { AstStatic,         semantic_token_modifier::Static },
        { AstDeprecated,     semantic_token_modifier::Deprecated },
        { AstAbstract,       semantic_token_modifier::Abstract },
        { AstAsync,          semantic_token_modifier::Async },
        { AstModification,   semantic_token_modifier::Modification },
        { AstDocumentation,  semantic_token_modifier::Documentation },
        { AstDefaultLibrary, semantic_token_modifier::DefaultLibrary }
    };
    return res;
}
