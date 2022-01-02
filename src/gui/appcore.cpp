#include "appcore.h"
#include "highlighter.h"
#include <src/km2/km2.h>
#include <QTimer>
#include <QTextBlock>
#include <QThread>
#include <QtConcurrent>
#include <src/km2/module.h>

void AppCore::recompile() {
    km2::flags flags;
    if(verbose()) {
        flags.push_back(km2::verbose);
    }

    if(onlyTree()) {
        flags.push_back(km2::only_tree);
    }

    if(m_firstCompilation) {
        completeCompilation(km2::compile(code().toStdString(), flags));
        m_firstCompilation = false;
    } else {
        if(m_currentFutureWatcher.isStarted()) {
            m_currentFutureWatcher.cancel();
            m_currentFuture.cancel();
        }
        m_currentFuture = QtConcurrent::run(km2::compile, code().toStdString(), flags);
        m_currentFutureWatcher.setFuture(m_currentFuture);
    }
}

QString AppCore::makeExecutable(const QString &path) {
    if(lastResult.mod) {
        if(const auto err = lastResult.mod->make_executable(path.toStdString()).left()) {
            return errToString(err.value());
        }
        return {};
    }
    return "empty module";
}

void AppCore::completeCompilation(const km2::compilation_result &cresult) {
    setTokens(QString::fromStdString(wall_e::lex::to_string(cresult.tokens)));
    setGramatic(QString::fromStdString(cresult.rules));
    setTree(cresult.token_tree);

    qDebug() << "COMPILATION COMPLETE";
    if(cresult.mod) {
        setAsmCode(QString::fromStdString(cresult.mod->llvmAssembly()));
    } else {
        setAsmCode("err: empty module");
    }
    setErrors(QList<wall_e::error>(cresult.errors.begin(), cresult.errors.end()));

    if(higlighter) {
        higlighter->setErrors(errors());
    }

    lastResult = cresult;
    emit compilationCompleated();
}

AppCore::AppCore(QObject *parent) : QObject(parent) {
    connect(this, &AppCore::codeChanged, this, &AppCore::recompile);
    connect(this, &AppCore::onlyTreeChanged, this, &AppCore::recompile);
    connect(this, &AppCore::verboseChanged, this, &AppCore::recompile);
    connect(this, &AppCore::codeDocumentChanged, this, [this](QQuickTextDocument *v) {
        if(higlighter == nullptr) {
            higlighter = new Highlighter(v->textDocument());
            higlighter->setErrors(errors());
            connect(higlighter, &Highlighter::highlightingCompleated, this, [this](){
                emit presentationCompleated();
            });
        }
    });

    connect(&m_currentFutureWatcher, &QFutureWatcher<km2::compilation_result>::finished, this, [this](){
        qDebug() << __PRETTY_FUNCTION__;
        this->completeCompilation(m_currentFutureWatcher.result());
    });

    recompile();
}

bool AppCore::startExecuting() {
    return executor()->start(lastResult.mod, lastResult.llvm_value);
}

QString AppCore::errToString(const wall_e::error &err) const {
    return QString::fromStdString(err.message());
}

int AppCore::errBegin(const wall_e::error &err) const {
    return err.segment().begin();
}

int AppCore::errEnd(const wall_e::error &err) const {
    return err.segment().end();
}
