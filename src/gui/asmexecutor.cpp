#include "asmexecutor.h"

#include <fstream>
#include <unistd.h>
AsmExecutor::AsmExecutor(QObject *parent) : QObject(parent) {
    connect(&process, &QProcess::readyReadStandardOutput, this, [this](){
        emit message(QString::fromUtf8(process.readAllStandardOutput()), false);
    });

    connect(&process, &QProcess::readyReadStandardError, this, [this](){
        emit message("err: " + QString::fromUtf8(process.readAllStandardError()), true);
    });

    connect(&process, &QProcess::started, this, [this](){
        emit message("__started__\n", false);
    });

    connect(&process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, [this](int exitCode, QProcess::ExitStatus exitStatus){
        (void)exitStatus;
        emit message("__finished with code: " + QString::number(exitCode) + " __", false);
        setExecuting(false);
    });
}

bool AsmExecutor::start(const std::string &code) {
    if(!executing()) {
        char tmpfile[] = "/tmp/kmova_XXXXXX.s";
        auto fd = mkstemps(tmpfile, 2);
        if(fd >= 0) {
            write(fd, code.data(), code.size());

            auto programName = std::string(tmpfile) + ".elf" + std::to_string(static_cast<uint64_t>(rand() * clock()));
            if(system((std::string("gcc -no-pie ") + tmpfile + " -o " + programName).c_str()) == 0) {
                process.start(QString::fromStdString(programName), QStringList());
            }
            unlink(tmpfile);
        }

        setExecuting(true);
        return true;
    }
    return false;
}

void AsmExecutor::abort() {
    setExecuting(false);
}
