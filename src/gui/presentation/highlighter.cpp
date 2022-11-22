#include "highlighter.h"
#include <QDebug>
#include <QTextDocument>
#include <QTimer>
#include <QAbstractTextDocumentLayout>



void Highlighter::setFormatBySegment(const qsizetype &blockTextSize, const wall_e::text_segment &segment, const QTextCharFormat &fmt) {
    const auto begin = currentBlock().begin();
    if(begin != currentBlock().end()) {
        const auto fragmentStartPos = begin.fragment().position();
        const int relativeBegin = std::max(int(segment.begin()) - fragmentStartPos, 0);
        const int relativeEnd = std::min(int(segment.end()) - fragmentStartPos, int(blockTextSize));
        const auto length = relativeEnd - relativeBegin;
        if(length > 0) {
            setFormat(relativeBegin, length, fmt);
        }
    }
}

Highlighter::Highlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
    connect(this, &Highlighter::errorsChanged, this, [this, parent](){ rehighlight(); });
    parent->setDefaultFont(QFont("Source Code Pro", 10));
    auto defaultTextOption = parent->defaultTextOption();
    defaultTextOption.setTabStopDistance(parent->defaultFont().pointSize() * 3);
    parent->setDefaultTextOption(defaultTextOption);
}

void Highlighter::setLegend(const Theme::HighlightLegend &legend) {
    m_legend = legend;
    rehighlight();
}

void Highlighter::setSemanticTokens(const QList<SemanticToken> &tokens) {
    m_tokens = tokens;
    rehighlight();
}

void Highlighter::setErrorsAndSemanticTokens(const QList<CompilationError> &errs, const QList<SemanticToken> &tokens) {
    const bool errsChanged = m_errors != errs;
    if(!errsChanged && m_tokens == tokens) return;
    m_errors = errs;
    m_tokens = tokens;
    if(errsChanged) emit errorsChanged(m_errors);
    rehighlight();
}

void Highlighter::highlightBlock(const QString &text) {
    for(const auto& t : m_tokens) {
        if(t.data().type < m_legend.tokenFormats.size()) {
            auto fmt = m_legend.tokenFormats[t.data().type];
            if(t.data().modifier < m_legend.tokenFormatModifiers.size()) {
                fmt = m_legend.tokenFormatModifiers[t.data().modifier]
                        .modify(fmt);
            }
            setFormatBySegment(
                        text.size(),
                        t.data().segment,
                        fmt
                    );
        }
    }

    for(const auto &err : errors()) {
        QTextCharFormat format;
        format.setFontUnderline(true);
        format.setUnderlineStyle(QTextCharFormat::UnderlineStyle::SingleUnderline);
        format.setUnderlineColor(0xff888800);
        format.setForeground(QBrush(0xffff0000));
        setFormatBySegment(text.size(), err.data().segment(), format);
    }
}

