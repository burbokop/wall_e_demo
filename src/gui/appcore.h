#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <wall_e/src/variant.h>
#include <src/km2/km2.h>
#include "klibcore/kmacro.h"
#include "asmexecutor.h"
#include "highlighter.h"
#include <QQuickTextDocument>

Q_DECLARE_METATYPE(km2::error)

class AppCore : public QObject {
    Q_OBJECT
    K_QML_TYPE(AppCore)
    K_READONLY_PROPERTY(QString, tokens, tokens, setTokens, tokensChanged, QString())
    K_READONLY_PROPERTY(QString, gramatic, gramatic, setGramatic, gramaticChanged, QString())
    K_READONLY_PROPERTY(QString, asmCode, asmCode, setAsmCode, asmCodeChanged, QString())
    K_READONLY_PROPERTY(wall_e::variant, tree, tree, setTree, treeChanged, wall_e::variant())
    K_META_TYPE(km2::error)

    Highlighter *higlighter = nullptr;
    K_READONLY_PROPERTY(QList<km2::error>, errors, errors, setErrors, errorsChanged, QList<km2::error>());
public:
    enum Mode { ModeTokens, ModeGramatic, ModeTree, ModeAsm, ModeExec };
private:
    K_CONST_PROPERTY(AsmExecutor*, executor, new AsmExecutor(this));
    km2::compilation_result lastResult;
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
    QString errToString(const km2::error& err) const;
    int errBegin(const km2::error& err) const;
    int errEnd(const km2::error& err) const;
signals:

};

#endif // APPCORE_H
