#ifndef JITEXECUTOR_H
#define JITEXECUTOR_H

#include <QObject>
#include <QTimer>
#include "klibcore/kmacro.h"
#include "either.h"
#include <sproc/src/fork.h>

namespace km2 {
    class translation_unit;
}

namespace llvm {
    class Function;
}

class JitExecutor : public QObject {
    Q_OBJECT
    K_READONLY_PROPERTY(bool, executing, executing, setExecuting, executingChanged, false)
    sproc::non_blocking::process process;
    QTimer *timer = nullptr;
public:
    explicit JitExecutor(QObject *parent = nullptr);
    Either start(const std::shared_ptr<km2::translation_unit> &unit, llvm::Function *entry);
    void abort();

signals:
    void message(QString, bool);
};

#endif // JITEXECUTOR_H
