#include "lspservice.h"
#include <QTextDocument>
#include <QTextBlock>

#include <wall_e/src/macro.h>


void LSPService::initialize(const QUrl &url, const SemanticTokensClientCapability &cap) {
    emit initialized(url, SemanticTokensLegend(m_service.register_semantic_tokens(cap.data())));
}

void LSPService::changeContent(const QUrl &url, QTextDocument *doc) {
    QString wholeText;
    for(auto it = doc->begin(); it != doc->end(); it = it.next()) {
        wholeText += it.text() + '\n';
    }
    const auto& errs = m_service.change_content(url.toString(QUrl::PrettyDecoded).toStdString(), wholeText.toStdString());
    emit contentChanged(
                url,
                Compiler::errorsFromWallE(errs),
                SemanticToken::listFromWallE(m_service.semantic_tokens(url.toString(QUrl::PrettyDecoded).toStdString(), true)),
                m_service.ast_tokens_ready(url.toString(QUrl::PrettyDecoded).toStdString())
                );
}

void LSPService::hoverText(const QUrl &url, int pos) {
    if(const auto& data = m_service.hover(url.toString(QUrl::PrettyDecoded).toStdString(), pos)) {
        emit hover(url, MarkupString(*data));
    } else {
        emit unhover(url);
    }
}
