#include "presentor.h"

Presentor::Presentor(QObject *parent) : QObject{ parent } {
    connect(this, &Presentor::codeDocumentChanged, this, [this](QQuickTextDocument *v) {
        if(m_higlighter == nullptr) {
            m_higlighter = new Highlighter(v->textDocument());
            m_higlighter->setErrors(errors());
        }
    });

    connect(this, &Presentor::errorsChanged, this, [this](const QList<CompilationError>& errs) {
        if(m_higlighter) {
            m_higlighter->setErrors(errors());
        }
    });
}
