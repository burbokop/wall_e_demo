#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <wall_e/src/models/variant.h>
#include <src/km2/km2.h>
#include <wall_e/src/models/error.h>
#include "klibcore/kmacro.h"
#include "backendfactory.h"
#include "either.h"
#include "compiler.h"
#include "jitexecutor.h"
#include <thread>
#include <future>
#include <QFuture>
#include <QFutureWatcher>

Q_DECLARE_METATYPE(wall_e::error)
Q_DECLARE_METATYPE(std::thread::id)

extern "C" {
int my_test_func();
}

class ProjFile : public QObject {
    Q_OBJECT
    K_CONST_PROPERTY(QString, fullPath, {})
    K_CONST_PROPERTY(QString, relativePath, {})
public:
    ProjFile(
            const QString& fullPath = {},
            const QString& relativePath = {},
            QObject* parent = nullptr
            )
    : QObject(parent),
      m_fullPath(fullPath),
      m_relativePath(relativePath) {}
};

class AppCore : public QObject {
    Q_OBJECT
    K_QML_TYPE(AppCore)
    QList<ProjFile*> m_projectFiles;
    Q_PROPERTY(QQmlListProperty<ProjFile> projectFiles READ projectFiles NOTIFY projectFilesChanged)

    K_META_TYPE(wall_e::error)
    K_AUTO_PROPERTY(UnitSharedPtr, unit, unit, setUnit, unitChanged, UnitSharedPtr())
    K_AUTO_PROPERTY(BackendValuePtr, entry, entry, setEntry, entryChanged, BackendValuePtr())

    static QList<ProjFile*> scanProjFiles(QObject* parent);

    //std::thread *m_compileThread = nullptr;

public:
    enum Mode { ModeTokens, ModeGramatic, ModeTree, ModeAsm, ModeExec };
private:
    K_CONST_PROPERTY(JitExecutor*, executor, new JitExecutor(this));
    K_CONST_PROPERTY(BackendFactory*, backendFactory, new BackendFactory(this));
    Q_ENUM(Mode)
    K_AUTO_PROPERTY(Mode, mode, mode, setMode, modeChanged, ModeTree)

    K_AUTO_PROPERTY(ProjFile*, openedProjFile, openedProjFile, setOpenedProjFile, openedProjFileChanged, nullptr)
    K_AUTO_PROPERTY(bool, codeEdited, codeEdited, setCodeEdited, codeEditedChanged, false)
    K_AUTO_PROPERTY(bool, verbose, verbose, setVerbose, verboseChanged, false)

    QFile *m_currentFile = nullptr;



public:
    explicit AppCore(QObject *parent = nullptr);
    ~AppCore();

    QQmlListProperty<ProjFile> projectFiles() {
        return QQmlListProperty<ProjFile>(this, &m_projectFiles);
    }

public slots:
    CompilationError makeExecutable(const QString& path);

    Either startExecuting();

    void loadFile(ProjFile *f, const QString& codeToWrite);
    void writeToCurrentFile(const QString& codeToWrite);
    QString readFromCurrentFile();

signals:
    void projectFilesChanged();
    void codeLoaded(const QString&);
};


#endif // APPCORE_H
