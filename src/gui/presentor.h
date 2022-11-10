#ifndef PRESENTOR_H
#define PRESENTOR_H

#include "klibcore/kmacro.h"
#include <QObject>
#include <QQuickTextDocument>

#include "compiler.h"
#include "highlighter.h"

class Presentor : public QObject {
    Q_OBJECT
    K_AUTO_PROPERTY(QQuickTextDocument*, codeDocument, codeDocument, setCodeDocument, codeDocumentChanged, nullptr)
    K_AUTO_PROPERTY(QList<CompilationError>, errors, errors, setErrors, errorsChanged, QList<CompilationError>());

    Highlighter *m_higlighter = nullptr;
public:
    explicit Presentor(QObject *parent = nullptr);

signals:
    void presentationCompleated();

};

#endif // PRESENTOR_H
