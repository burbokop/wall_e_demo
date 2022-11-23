#include "theme.h"

Theme::HighlightLegend Theme::highlightLegend(const SemanticTokensLegend &legend) const {
    const auto tokenFormats = legend
            .data()
            .token_types
            .map<QTextCharFormat>([this](km2::lsp::semantic_token_type t) { return this->format(t); });

    const auto tokenModifiers = legend
            .data()
            .token_modifiers
            .map<Modifier>([this](km2::lsp::semantic_token_modifier m) { return this->modifier(m); });

    return HighlightLegend {
        .tokenFormats = QVector<QTextCharFormat>(tokenFormats.begin(), tokenFormats.end()),
        .tokenFormatModifiers = QVector<Modifier>(tokenModifiers.begin(), tokenModifiers.end())
    };
}

SemanticTokensClientCapability Theme::capability() const {
    const auto& tt = m_formats.keys();
    wall_e::vec<km2::lsp::semantic_token_type> vec(tt.begin(), tt.end());

    const auto& mm = m_modifiers.keys();
    wall_e::vec<km2::lsp::semantic_token_modifier> mvec(mm.begin(), mm.end());

    return SemanticTokensClientCapability(km2::lsp::semantic_tokens_client_capability {
                                              .token_types = vec,
                                              .token_modifiers = mvec
                                          });
}


Theme *Theme::qtCreator(QObject *parent) {
    static const Formats formats = {
                             { km2::lsp::Namespace, formatFromColor(0x800080) },
                             { km2::lsp::Type, formatFromColor(0x800080) },
                             { km2::lsp::Class, formatFromColor(0x800080) },
                             //{ km2::lsp::Enum, formatFromColor(0x292747) },
                             //{ km2::lsp::Interface, formatFromColor(0x384756) },
                             //{ km2::lsp::Struct, formatFromColor(0x387476) },
                             //{ km2::lsp::TypeParameter, formatFromColor(0x387601) },
                             { km2::lsp::Parameter, formatFromColor(0x092e64) },
                             { km2::lsp::Variable, formatFromColor(0x092e64) },
                             { km2::lsp::Property, italicFormat() },
                             //{ km2::lsp::EnumMember, formatFromColor(0x123456) },
                             //{ km2::lsp::Event, formatFromColor(0x987654) },
                             { km2::lsp::Function, formatFromColor(0x00677c) },
                             //{ km2::lsp::Method, formatFromColor(0x937465) },
                             { km2::lsp::Macro, formatFromColor(0x00677c) },
                             { km2::lsp::Keyword, formatFromColor(0x808000) },
                             //{ km2::lsp::Modifier, formatFromColor(0x323944) },
                             { km2::lsp::Comment, formatFromColor(0x008000) },
                             { km2::lsp::String, formatFromColor(0x008000) },
                             { km2::lsp::Number, formatFromColor(0x000080) },
                             //{ km2::lsp::Regexp, formatFromColor(0xaaf445) },
                             //{ km2::lsp::Operator, formatFromColor(0xff99ff) },
                             //{ km2::lsp::Decorator, formatFromColor(0xff00ff) }
                         };

    static const Modifiers modifiers = {
        { km2::lsp::Declaration, Modifier { .weight = QFont::Bold } },
        //{ km2::lsp::Definition,
        //{ km2::lsp::Readonly,
        { km2::lsp::Static, Modifier { .foreground = 0x800080 } },
        //{ km2::lsp::Deprecated,
        { km2::lsp::Abstract, Modifier { .foreground = 0x00677c, .italic = true } },
        //{ km2::lsp::Async,
        //{ km2::lsp::Modification,
        { km2::lsp::Documentation, Modifier { .foreground = 0x000080 } } // TODO Doxygen.Comment" foreground="#000080"/> Doxygen.Tag" foreground="#0000ff"/>
        //{ km2::lsp::DefaultLibrary
    };

    return new Theme(
                formats,
                formatFromColor(0xff000000),
                modifiers,
                Modifier {},
                parent
                );


}

QTextCharFormat Theme::Modifier::modify(QTextCharFormat f) const {
    if(foreground) { f.setForeground(*foreground); }
    if(background) { f.setBackground(*background); }
    if(italic) { f.setFontItalic(*italic); }
    if(weight) { f.setFontWeight(*weight); }
    return f;
}
