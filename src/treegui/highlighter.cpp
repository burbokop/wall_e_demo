#include "highlighter.h"

//! [0]
Highlighter::Highlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
    connect(this, &Highlighter::errorsChanged, this, [this](){
        rehighlight();
    });

    addRules(Qt::darkYellow, {
                 "asm",
                 "number",
                 "string"
             });


    addRules(Qt::darkCyan, {
                 "%[re][abcd]x",
                 "%[re][ds]i",
                 "%[re][bs]p",
                 "%[abcd][hlx]",
                 "%xmm[0-9]+"
             });

    addRules(Qt::GlobalColor::magenta, {
                 "mov",
                 "xor",
                 "push",
                 "pop",
                 "call"
             });

    addRules(Qt::GlobalColor::darkMagenta, {
                 "(?![a-z])[0-9]+"
             });


    HighlightingRule rule;

    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);
//! [3]

//! [4]
    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);
//! [4]

//! [5]
    //functionFormat.setForeground(Qt::blue);
    //rule.pattern = QRegularExpression("[A-Za-z0-9_]+\\(");
    //rule.format = functionFormat;
    //highlightingRules.append(rule);


    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");

    std::reverse(highlightingRules.begin(), highlightingRules.end());
}

void Highlighter::addRules(const QBrush &brush, const QStringList &rules) {
    QTextCharFormat format;
    format.setForeground(brush);
    addRules(format, rules);
}

void Highlighter::addRules(const QTextCharFormat &format, const QStringList &rules) {
    for (const QString &pattern : rules) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = format;
        highlightingRules.append(rule);
    }
}

void Highlighter::highlightBlock(const QString &text) {
    for (const HighlightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }

    for(auto err : errors()) {
        const auto begin = currentBlock().begin();
        if(begin != currentBlock().end()) {
            const auto fragmentStartPos = begin.fragment().position();

            qDebug() << "error" << QString::fromStdString(err.message) << err.start << err.end << text;
            QTextCharFormat f;


            //f.setBackground(QBrush("#ff0000"));
            f.setFontUnderline(true);
            f.setUnderlineColor("#ff0000");
            //f.setUnderlineStyle(QTextCharFormat::UnderlineStyle::DotLine);
            setFormat(err.start - fragmentStartPos, err.end - err.start, f);
        }

    }
}
