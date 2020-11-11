#include "asmexecutor.h"

#include <fstream>

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

bool AsmExecutor::start(std::string code) {
    if(!executing()) {
        const auto asmFileName = tmpnam(nullptr) + std::string(".s");
        std::ofstream asm_output;
        asm_output.open(asmFileName, std::ios::out);
        if(asm_output.is_open()) {
            asm_output.write(code.data(), code.size());
            asm_output.close();

            auto programName = std::string(asmFileName) + ".elf" + std::to_string(static_cast<uint64_t>(rand() * clock()));
            if(system((std::string("gcc -no-pie ") + asmFileName + " -o " + programName).c_str()) == 0) {
                process.start(QString::fromStdString(programName), QStringList());
            }
        }

        setExecuting(true);
        return true;
    }
    return false;
}

void AsmExecutor::abort() {
    setExecuting(false);
}
