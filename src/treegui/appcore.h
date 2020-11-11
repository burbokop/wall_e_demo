#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <wall_e/src/kgramvariant.h>
#include <src/km2/km2.h>
#include "klibcore/kmacro.h"
#include "asmexecutor.h"
#include "highlighter.h"
#include <QQuickTextDocument>

Q_DECLARE_METATYPE(km2_error)

class AppCore : public QObject {
    Q_OBJECT
    K_QML_TYPE(AppCore)
    K_READONLY_PROPERTY(QString, tokens, tokens, setTokens, tokensChanged, QString())
    K_READONLY_PROPERTY(QString, gramatic, gramatic, setGramatic, gramaticChanged, QString())
    K_READONLY_PROPERTY(QString, asmCode, asmCode, setAsmCode, asmCodeChanged, QString())
    K_READONLY_PROPERTY(kgram_variant_t, tree, tree, setTree, treeChanged, kgram_variant_t())
    K_META_TYPE(km2_error)

    Highlighter *higlighter = nullptr;
    K_READONLY_PROPERTY(QList<km2_error>, errors, errors, setErrors, errorsChanged, QList<km2_error>());
public:
    enum Mode { ModeTokens, ModeGramatic, ModeTree, ModeAsm, ModeExec };
private:
    K_CONST_PROPERTY(AsmExecutor*, executor, new AsmExecutor(this));
    km2_compilation_result lastResult;
    Q_ENUM(Mode)
    K_AUTO_PROPERTY(Mode, mode, mode, setMode, modeChanged, ModeTree)
    K_AUTO_PROPERTY(QQuickTextDocument*, codeDocument, codeDocument, setCodeDocument, codeDocumentChanged, nullptr)

    K_SAVED_PROPERTY(QString, code, code, setCode, codeChanged, QString())
    K_AUTO_PROPERTY(bool, onlyTree, onlyTree, setOnlyTree, onlyTreeChanged, false)
    K_AUTO_PROPERTY(bool, verbose, verbose, setVerbose, verboseChanged, false)

    bool fff = false;
    void recompile();
public:
    explicit AppCore(QObject *parent = nullptr);

public slots:
    bool startExecuting();

signals:

};

#endif // APPCORE_H
