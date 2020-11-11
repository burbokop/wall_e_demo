#ifndef ASMEXECUTOR_H
#define ASMEXECUTOR_H

#include "klibcore/kmacro.h"

#include <QProcess>
#include <QQueue>


class AsmExecutor : public QObject {
    Q_OBJECT
    K_QML_TYPE(AsmExecutor)
    K_READONLY_PROPERTY(bool, executing, executing, setExecuting, executingChanged, false)
    QProcess process;
public:
    AsmExecutor(QObject *parent = nullptr);
    bool start(std::string code);
    void abort();

signals:
    void message(QString, bool);
};

#endif // ASMEXECUTOR_H
