#include "lspservice.h"
#include <QTextDocument>
#include <QTextBlock>

#include <wall_e/src/macro.h>


void LSPService::initialize(const QString &uri, const SemanticTokensClientCapability &cap) {
    m_service.initialize(uri.toStdString());
    m_service.configure(uri.toStdString(), {});
    emit initialized(SemanticTokensLegend(m_service.register_semantic_tokens(cap.data())));
}

void LSPService::changeContent(const QString &uri, QTextDocument *doc) {
    qDebug() << wall_e_this_function << QThread::currentThreadId();
    QString wholeText;
    for(auto it = doc->begin(); it != doc->end(); it = it.next()) {
        wholeText += it.text() + '\n';
    }
    const auto& errs = m_service.change_content(uri.toStdString(), wholeText.toStdString());
    emit contentChanged(
                Compiler::errorsFromWallE(errs),
                SemanticToken::listFromWallE(m_service.semantic_tokens(uri.toStdString())),
                m_service.ast_tokens_ready(uri.toStdString())
                );
}
