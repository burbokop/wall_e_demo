#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <wall_e/src/models/variant.h>
#include <src/km2/km2.h>
#include <wall_e/src/models/error.h>
#include "klibcore/kmacro.h"
#include "highlighter.h"
#include "jitexecutor.h"
#include <QQuickTextDocument>
#include <thread>
#include <future>
#include <QFuture>
#include <QFutureWatcher>

Q_DECLARE_METATYPE(wall_e::error)
Q_DECLARE_METATYPE(std::thread::id)

class AppCore : public QObject {
    Q_OBJECT
    K_QML_TYPE(AppCore)
    K_READONLY_PROPERTY(QString, tokens, tokens, setTokens, tokensChanged, QString())
    K_READONLY_PROPERTY(QString, astTokens, astTokens, setAstTokens, astTokensChanged, QString())
    K_READONLY_PROPERTY(QString, gramatic, gramatic, setGramatic, gramaticChanged, QString())
    K_READONLY_PROPERTY(QString, asmCode, asmCode, setAsmCode, asmCodeChanged, QString())
    K_READONLY_PROPERTY(wall_e::variant, tree, tree, setTree, treeChanged, wall_e::variant())
    K_META_TYPE(wall_e::error)

    //std::thread *m_compileThread = nullptr;


    QFutureWatcher<km2::compilation_result> m_currentFutureWatcher;
    QFuture<km2::compilation_result> m_currentFuture;

    Highlighter *higlighter = nullptr;
    K_READONLY_PROPERTY(QList<wall_e::error>, errors, errors, setErrors, errorsChanged, QList<wall_e::error>());
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

    bool m_firstCompilation = true;
    void recompile();


private slots:
    void completeCompilation(const km2::compilation_result&cresult);

public:
    explicit AppCore(QObject *parent = nullptr);

public slots:
    QString makeExecutable(const QString& path);

    bool startExecuting();
    QString errToString(const wall_e::error& err) const;
    int errBegin(const wall_e::error& err) const;
    int errEnd(const wall_e::error& err) const;

signals:
    void compilationCompleated();
    void presentationCompleated();
};

#endif // APPCORE_H
