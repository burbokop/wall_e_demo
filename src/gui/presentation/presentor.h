#ifndef PRESENTOR_H
#define PRESENTOR_H

#include "../klibcore/kmacro.h"
#include <QObject>
#include <QQuickTextDocument>
#include "highlighter.h"
#include "lspservice.h"

class Presentor : public QObject {
    Q_OBJECT
    K_AUTO_PROPERTY(QQuickTextDocument*, codeDocument, codeDocument, setCodeDocument, codeDocumentChanged, nullptr)
    K_READONLY_PROPERTY(QList<CompilationError>, errors, errors, setErrors, errorsChanged, QList<CompilationError>());
    K_READONLY_PROPERTY(bool, astTokensReady, astTokensReady, setAstTokensReady, astTokensReadyChanged, false)



    QList<QMetaObject::Connection> m_documentConnections;

    Highlighter *m_higlighter = nullptr;
    static QString uriFromDoc(const QTextDocument* doc);

    QTextDocument* m_doc = nullptr;
    LSPService* m_service = nullptr;
    QThread *m_serviceThread = nullptr;
    Theme *m_theme = nullptr;

    void initialize();
public:
    explicit Presentor(QObject *parent = nullptr);
    ~Presentor();

signals:
    void presentationCompleated();

};

#endif // PRESENTOR_H
