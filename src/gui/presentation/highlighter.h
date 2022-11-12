#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

#include "../compiler.h"
#include "theme.h"
#include "../klibcore/kmacro.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE


class Highlighter : public QSyntaxHighlighter {
    Q_OBJECT
    K_AUTO_PROPERTY(QList<CompilationError>, errors, errors, setErrors, errorsChanged, QList<CompilationError>());

    QList<SemanticToken> m_tokens;
    Theme::HighlightLegend m_legend;

    void setFormatBySegment(const qsizetype& blockTextSize, const wall_e::text_segment& segment, const QTextCharFormat& fmt);
public:

    Highlighter(QTextDocument *parent = nullptr);

    void setLegend(const Theme::HighlightLegend& legend);
    void setSemanticTokens(const QList<SemanticToken>& tokens);
    void setErrorsAndSemanticTokens(const QList<CompilationError>& errs, const QList<SemanticToken>& tokens);

protected:
    void highlightBlock(const QString &text) override;
};

#endif // HIGHLIGHTER_H
