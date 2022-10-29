#include "service.h"

const std::map<std::string, km2::lsp::semantic_token_type> &km2::lsp::default_semantic_token_types_map() {
    static std::map<std::string, semantic_token_type> res = {
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
        { "TOK_ID", semantic_token_type::Variable },
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

const std::map<km2::ast_token_type, km2::lsp::semantic_token_type> &km2::lsp::default_ast_semantic_token_types_map() {
    static std::map<ast_token_type, semantic_token_type> res = {
        { AstNamespace,     Namespace },
        { AstType,          Type },
        { AstClass,         Class },
        { AstEnum,          Enum },
        { AstInterface,     Interface },
        { AstStruct,        Struct },
        { AstTypeParameter, TypeParameter },
        { AstParameter,     Parameter },
        { AstVariable,      Variable },
        { AstProperty,      Property },
        { AstEnumMember,    EnumMember },
        { AstEvent,         Event },
        { AstFunction,      Function },
        { AstMethod,        Method },
        { AstMacro,         Macro },
        { AstKeyword,       Keyword },
        { AstModifier,      Modifier },
        { AstComment,       Comment },
        { AstString,        String },
        { AstNumber,        Number },
        { AstRegexp,        Regexp },
        { AstOperator,      Operator },
        { AstDecorator,     Decorator },
    };
    return res;
}
