#ifndef JITEXECUTOR_H
#define JITEXECUTOR_H

#include <QObject>
#include <QTimer>
#include "klibcore/kmacro.h"
#include "either.h"
#include <sproc/src/fork.h>

namespace km2 {
namespace backend {
    class unit;
    class function;
}
}

class JitExecutor : public QObject {
    Q_OBJECT
    K_READONLY_PROPERTY(bool, executing, executing, setExecuting, executingChanged, false)
    sproc::non_blocking::process process;
    QTimer *timer = nullptr;
public:
    enum MessageType {
        Trace,
        Err,
        Warn
    };
    Q_ENUM(MessageType)


    explicit JitExecutor(QObject *parent = nullptr);
    Either start(const std::shared_ptr<km2::backend::unit> &unit, km2::backend::function *entry, bool verbose);
    void abort();

signals:
    void message(const QString& message, MessageType type, bool isSystem);
};

#endif // JITEXECUTOR_H
