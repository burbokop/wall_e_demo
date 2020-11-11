#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <src/km2/km2.h>
#include "klibcore/kmacro.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

//! [0]
class Highlighter : public QSyntaxHighlighter {
    Q_OBJECT
    K_AUTO_PROPERTY(QList<km2_error>, errors, errors, setErrors, errorsChanged, QList<km2_error>());
public:
    Highlighter(QTextDocument *parent = 0);



    void addRules(const QBrush &format, const QStringList &rules);
    void addRules(const QTextCharFormat &format, const QStringList &rules);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};
//! [0]

#endif // HIGHLIGHTER_H
