#include "jitexecutor.h"

#include <QThread>
#include <QTimer>

#include <llvm/IR/Function.h>


#include <src/km2/module.h>

JitExecutor::JitExecutor(QObject *parent) : QObject(parent) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this](){
        if(process) {
            const auto result = process.result();
            if(result.has_value()) {
                emit message(QString::fromStdString(result.value().out), false);
                emit message(QString::fromStdString(result.value().err), true);
                emit message("__finished with code: " + QString::number(result.value().ext_code) + " __", false);
                timer->stop();
                setExecuting(false);
            } else {
                const auto out = process.read_all_out();
                if(out.size() > 0) {
                    emit message(QString::fromStdString(out), false);
                }
                const auto err = process.read_all_err();
                if(err.size() > 0) {
                    emit message(QString::fromStdString(err), true);
                }
            }
        }
    });
}

bool JitExecutor::start(const std::shared_ptr<km2::module> &module, llvm::Value *entry) {
    if(!executing()) {
        llvm::Function* f = static_cast<llvm::Function*>(entry);

        process = sproc::non_blocking::fork([module, f, this](){
            if(const auto result = module->runJit(f)) {
                return result.right_value();
            } else {
                return -1;
            }
        });
        timer->start(100);
        setExecuting(true);
        return true;
    }
    return false;
}

void JitExecutor::abort() {
    setExecuting(false);
}
