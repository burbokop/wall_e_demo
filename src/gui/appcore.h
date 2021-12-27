#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <wall_e/src/variant.h>
#include <src/km2/km2.h>
#include <src/km2/error.h>
#include "klibcore/kmacro.h"
#include "asmexecutor.h"
#include "highlighter.h"
#include "jitexecutor.h"
#include <QQuickTextDocument>
#include <thread>

Q_DECLARE_METATYPE(km2::error)
Q_DECLARE_METATYPE(std::thread::id)

class AppCore : public QObject {
    Q_OBJECT
    K_QML_TYPE(AppCore)
    K_READONLY_PROPERTY(QString, tokens, tokens, setTokens, tokensChanged, QString())
    K_READONLY_PROPERTY(QString, gramatic, gramatic, setGramatic, gramaticChanged, QString())
    K_READONLY_PROPERTY(QString, asmCode, asmCode, setAsmCode, asmCodeChanged, QString())
    K_READONLY_PROPERTY(wall_e::variant, tree, tree, setTree, treeChanged, wall_e::variant())
    K_META_TYPE(km2::error)

    //std::thread *m_compileThread = nullptr;


    std::map<std::thread::id, std::thread*> m_threads;

    Highlighter *higlighter = nullptr;
    K_READONLY_PROPERTY(QList<km2::error>, errors, errors, setErrors, errorsChanged, QList<km2::error>());
public:
    enum Mode { ModeTokens, ModeGramatic, ModeTree, ModeAsm, ModeExec };
private:
    K_CONST_PROPERTY(JitExecutor*, executor, new JitExecutor(this));
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
    void compilationCompleated(std::thread::id);
    void presentationCompleated();
};

#endif // APPCORE_H
