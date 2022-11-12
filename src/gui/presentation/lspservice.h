#ifndef LSPSERVICE_H
#define LSPSERVICE_H

#include <QObject>
#include "theme.h"
#include "../compiler.h"

class LSPService : public QObject {
    Q_OBJECT
    km2::lsp::service m_service;
public:
    LSPService(QObject* parent = nullptr) : QObject(parent) {}
public slots:
    void initialize(const QString& uri, const SemanticTokensClientCapability& cap);
    void changeContent(const QString &uri, QTextDocument *doc);

signals:
    void initialized(const SemanticTokensLegend& legend);
    void contentChanged(const QList<CompilationError>&, const QList<SemanticToken>&, bool astTokensReady);

};

#endif // LSPSERVICE_H
