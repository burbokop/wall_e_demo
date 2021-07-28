#include "jitexecutor.h"

#include <QThread>

#include <llvm/IR/Function.h>

#include <src/km2/module.h>

JitExecutor::JitExecutor(QObject *parent) : QObject(parent) {
    m_thread = new AbstractThread(this);

    // TO DO std i/o redirect

    connect(m_thread, &AbstractThread::resultReady, this, [this](int exitCode){
        emit message("__finished with code: " + QString::number(exitCode) + " __", false);
        setExecuting(false);
    });
}

bool JitExecutor::start(const std::shared_ptr<km2::module> &module, llvm::Value *entry) {
    if(!executing()) {
        llvm::Function* f = static_cast<llvm::Function*>(entry);
        m_thread->start([module, f, this](){
            if(const auto result = module->runJit(f)) {
                return result.right_value();
            } else {
                emit message(QString("jit error: ") + QString::fromStdString(result.left_value()), true);
                return -1;
            }
        });
        setExecuting(true);
        return true;
    }
    return false;
}

void JitExecutor::abort() {
    setExecuting(false);
}
