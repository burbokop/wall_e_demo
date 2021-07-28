#ifndef JITEXECUTOR_H
#define JITEXECUTOR_H

#include <QObject>
#include "klibcore/kmacro.h"
#include "abstractthread.h"

namespace km2 {
    class module;
}

namespace llvm {
    class Value;
}

class JitExecutor : public QObject {
    Q_OBJECT
    K_READONLY_PROPERTY(bool, executing, executing, setExecuting, executingChanged, false)
    AbstractThread *m_thread = nullptr;
public:
    explicit JitExecutor(QObject *parent = nullptr);
    bool start(const std::shared_ptr<km2::module> &module, llvm::Value* entry);
    void abort();

signals:
    void message(QString, bool);
};

#endif // JITEXECUTOR_H
