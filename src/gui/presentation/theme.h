#ifndef THEME_H
#define THEME_H

#include <QObject>
#include <QTextCharFormat>
#include <src/km2/lsp/service.h>

class SemanticTokensClientCapability {
    Q_GADGET
    km2::lsp::semantic_tokens_client_capability m_data;
public:
    explicit SemanticTokensClientCapability(const km2::lsp::semantic_tokens_client_capability &data = {}) : m_data(data) {}
    const km2::lsp::semantic_tokens_client_capability &data() const { return m_data; };
    inline bool operator ==(const SemanticTokensClientCapability& other) const { return m_data == other.m_data; }
};


class SemanticTokensLegend {
    Q_GADGET
    km2::lsp::semantic_tokens_legend m_data;
public:
    explicit SemanticTokensLegend(const km2::lsp::semantic_tokens_legend &data = {}) : m_data(data) {}
    const km2::lsp::semantic_tokens_legend &data() const { return m_data; };
    inline bool operator ==(const SemanticTokensLegend& other) const { return m_data == other.m_data; }
};

class SemanticToken {
    Q_GADGET
    km2::lsp::semantic_token m_data;
public:
    explicit SemanticToken(const km2::lsp::semantic_token &data = {}) : m_data(data) {}
    const km2::lsp::semantic_token &data() const { return m_data; };
    inline bool operator ==(const SemanticToken& other) const { return m_data == other.m_data; }
    static QList<SemanticToken> listFromWallE(const wall_e::vec<km2::lsp::semantic_token>& v) {
        QList<SemanticToken> result;
        for(const auto& vv : v) {
            result.push_back(SemanticToken(vv));
        }
        return result;
    }
};

class Theme : public QObject {
    Q_OBJECT
public:
    typedef QMap<km2::lsp::semantic_token_type, QTextCharFormat> Formats;

    struct Modifier {
        wall_e::opt<QColor> foreground;
        wall_e::opt<QColor> background;
        wall_e::opt<bool> italic;
        wall_e::opt<QFont::Weight> weight;
        QTextCharFormat modify(QTextCharFormat f) const;
        friend inline QDebug& operator<<(QDebug& dbg, const Modifier& m) {
            return dbg << "{ f: " << m.foreground
                       << ", b: " << m.background
                       << ", i: " << m.italic
                       << ", w: " << m.weight
                       << " }";
        }

    };

    typedef QMap<km2::lsp::semantic_token_modifier, Modifier> Modifiers;

    struct HighlightLegend {
        QVector<QTextCharFormat> tokenFormats;
        QVector<Modifier> tokenFormatModifiers;
    };
private:
    const Formats m_formats;
    const QTextCharFormat m_defaultFormat;
    const Modifiers m_modifiers;
    const Modifier m_defaultModifier;
public:
    Theme(
            const QMap<km2::lsp::semantic_token_type, QTextCharFormat>& formats,
            const QTextCharFormat& defaultFormat,
            const Modifiers& modifiers,
            const Modifier& defaultModifier,
            QObject* parent = nullptr
            )
        : QObject(parent),
          m_formats(formats),
          m_defaultFormat(defaultFormat),
          m_modifiers(modifiers),
          m_defaultModifier(defaultModifier) {}

    const Formats &formats() const { return m_formats; }
    const QTextCharFormat &defaultFormat() const { return m_defaultFormat; }

    const Modifiers &modifiers() const { return m_modifiers; }
    const Modifier &defaultModifier() const { return m_defaultModifier; }

    inline QTextCharFormat format(km2::lsp::semantic_token_type t) const {
        return m_formats.value(t, m_defaultFormat);
    }

    inline Modifier modifier(km2::lsp::semantic_token_modifier m) const {
        return m_modifiers.value(m, m_defaultModifier);
    }

    inline static QTextCharFormat italicFormat() {
        QTextCharFormat r;
        r.setFontItalic(true);
        return r;
    }
    inline static QTextCharFormat formatFromColor(const QColor& foreground, const QColor& background = 0xffffff) {
        QTextCharFormat format;
        format.setForeground(foreground);
        format.setBackground(background);
        return format;
    }

    HighlightLegend highlightLegend(const SemanticTokensLegend& legend) const;

    SemanticTokensClientCapability capability() const;

    static Theme* qtCreator(QObject* parent = nullptr);
};


#endif // THEME_H
