#ifndef COMPILER_H
#define COMPILER_H

#include "klibcore/kmacro.h"
#include <QObject>
#include <src/km2/km2.h>
#include "backendfactory.h"
#include <QFutureWatcher>

class CompilationError {
    Q_GADGET

    Q_PROPERTY(int regin READ begin)
    Q_PROPERTY(int end READ end)

    wall_e::error m_data;
public:
    CompilationError(wall_e::error data = {}) : m_data(data) {}
    const wall_e::error& data() const { return m_data; }

    Q_INVOKABLE inline QString toString() const {
        return QString::fromStdString(m_data.message());
    }
    inline int begin() const { return m_data.segment().begin(); }
    inline int end() const { return m_data.segment().end(); }

    inline bool operator==(const CompilationError& other) const {
        return m_data == other.m_data;
    }
};

class TokenTree : public QObject {
    Q_OBJECT
    wall_e::variant m_data;
public:
    TokenTree(QObject* parent = nullptr) : QObject(parent) {}
    static TokenTree* fromWallEVariant(const wall_e::variant &data = {}, QObject* parent = nullptr);

    const wall_e::variant& data() const { return m_data; }

};

class UnitSharedPtr {
    Q_GADGET
    Q_PROPERTY(bool valid READ valid)
    std::shared_ptr<km2::backend::unit> m_data;
public:
    UnitSharedPtr(const std::shared_ptr<km2::backend::unit>& data = nullptr) : m_data(data) {}
    inline bool valid() const { return m_data ? true : false; }
    inline const std::shared_ptr<km2::backend::unit>& data() const { return m_data; }

    inline bool operator==(const UnitSharedPtr& other) const {
        return m_data == other.m_data;
    }
};

class BackendValuePtr {
    km2::backend::value* m_data;
    Q_GADGET
    Q_PROPERTY(bool valid READ valid)
public:
    BackendValuePtr(km2::backend::value* data = nullptr) : m_data(data) {}
    inline bool valid() const { return m_data ? true : false; }
    inline km2::backend::value* data() const { return m_data; }

    inline bool operator==(const BackendValuePtr& other) const {
        return m_data == other.m_data;
    }
};

class GramLogSharedPtr {
    Q_GADGET
    std::shared_ptr<wall_e::gram::log> m_data;
    Q_PROPERTY(bool valid READ valid)
public:
    GramLogSharedPtr(const std::shared_ptr<wall_e::gram::log>& data = nullptr) : m_data(data) {}
    inline bool valid() const { return m_data ? true : false; }
    std::shared_ptr<wall_e::gram::log> data() const { return m_data; }
    inline bool operator==(const GramLogSharedPtr& other) const { return m_data == other.m_data; }
};

class Compiler : public QObject {
    Q_OBJECT
    K_READONLY_PROPERTY(QString, tokens, tokens, setTokens, tokensChanged, QString())
    K_READONLY_PROPERTY(QString, astTokens, astTokens, setAstTokens, astTokensChanged, QString())
    K_READONLY_PROPERTY(QString, gramatic, gramatic, setGramatic, gramaticChanged, QString())
    K_READONLY_PROPERTY(QString, asmCode, asmCode, setAsmCode, asmCodeChanged, QString())
    K_READONLY_PROPERTY(TokenTree*, tree, tree, setTree, treeChanged, nullptr)
    K_READONLY_PROPERTY(UnitSharedPtr, unit, unit, setUnit, unitChanged, UnitSharedPtr())
    K_READONLY_PROPERTY(BackendValuePtr, rootBackendValue, rootBackendValue, setRootBackendValue, rootBackendValueChanged, BackendValuePtr())
    K_READONLY_PROPERTY(GramLogSharedPtr, log, log, setLog, logChanged, GramLogSharedPtr())

    K_READONLY_PROPERTY(QList<CompilationError>, errors, errors, setErrors, errorsChanged, QList<CompilationError>());

    inline void addErrors(const QList<CompilationError>& errs) {
        m_errors.append(errs);
        emit errorsChanged(errors());
    }

    QFutureWatcher<km2::compilation_result> m_currentFutureWatcher;
    QFuture<km2::compilation_result> m_currentFuture;
    wall_e::opt<km2::compilation_result> m_prevResult;

    K_AUTO_PROPERTY(BackendSharedPtr, backend, backend, setBackend, backendChanged, BackendSharedPtr())
    K_AUTO_PROPERTY(QString, code, code, setCode, codeChanged, QString())
    K_AUTO_PROPERTY(bool, onlyTree, onlyTree, setOnlyTree, onlyTreeChanged, false)
    K_AUTO_PROPERTY(bool, verbose, verbose, setVerbose, verboseChanged, false)

    bool m_firstCompilation = true;
    void recompile();


private slots:
    void completeCompilation(const km2::compilation_result&cresult);

public:
    explicit Compiler(QObject *parent = nullptr);
    static QList<CompilationError> errorsFromWallE(const wall_e::list<wall_e::error>& lst);

public slots:

signals:
    void compilationCompleated();

};

#endif // COMPILER_H
