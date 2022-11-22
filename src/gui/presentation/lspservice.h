#ifndef LSPSERVICE_H
#define LSPSERVICE_H

#include <QObject>
#include "theme.h"
#include "../compiler.h"

class MarkupString {
    Q_GADGET
    Q_PROPERTY(QString str READ str)
    Q_PROPERTY(QString format READ format)

    km2::markup_string m_data;
public:
    explicit MarkupString(const km2::markup_string &data = {}) : m_data(data) {}
    const km2::markup_string &data() const { return m_data; };
    inline bool operator ==(const MarkupString& other) const { return m_data == other.m_data; }

    QString str() const { return QString::fromStdString(m_data.str()); }
    QString format() const {
        switch(m_data.format()) {
        case km2::markup_string::PlainText: return "plain";
        case km2::markup_string::Markdown: return "md";
        }
        return "unknown";
    }

};

class LSPService : public QObject {
    Q_OBJECT
    km2::lsp::service m_service;
public:
    LSPService(QObject* parent = nullptr) : QObject(parent) {}

public slots:
    void initialize(const QUrl& url, const SemanticTokensClientCapability& cap);
    void changeContent(const QUrl &url, QTextDocument *doc);
    void hoverText(const QUrl &url, int pos);


signals:
    void initialized(const QUrl &url, const SemanticTokensLegend& legend);
    void contentChanged(const QUrl &url, const QList<CompilationError>&, const QList<SemanticToken>&, bool astTokensReady);
    void hover(const QUrl &url, const MarkupString& str);
    void unhover(const QUrl &url);

};

#endif // LSPSERVICE_H
