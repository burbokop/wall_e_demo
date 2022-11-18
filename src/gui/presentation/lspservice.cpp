#include "lspservice.h"
#include <QTextDocument>
#include <QTextBlock>

#include <wall_e/src/macro.h>


void LSPService::initialize(const QString &uri, const SemanticTokensClientCapability &cap) {
    m_service.initialize(uri.toStdString());
    m_service.configure(uri.toStdString(), {});
    emit initialized(uri, SemanticTokensLegend(m_service.register_semantic_tokens(cap.data())));
}

void LSPService::changeContent(const QString &uri, QTextDocument *doc) {
    QString wholeText;
    for(auto it = doc->begin(); it != doc->end(); it = it.next()) {
        wholeText += it.text() + '\n';
    }
    const auto& errs = m_service.change_content(uri.toStdString(), wholeText.toStdString());
    emit contentChanged(
                uri,
                Compiler::errorsFromWallE(errs),
                SemanticToken::listFromWallE(m_service.semantic_tokens(uri.toStdString())),
                m_service.ast_tokens_ready(uri.toStdString())
                );
}

void LSPService::hoverText(const QString &uri, int pos) {
    if(const auto& data = m_service.hover(uri.toStdString(), pos)) {
        emit hover(uri, MarkupString(*data));
    } else {
        emit unhover(uri);
    }
}
