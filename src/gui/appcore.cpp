#include "appcore.h"
#include "highlighter.h"
#include <src/km2/km2.h>
#include <QTimer>
#include <QTextBlock>
#include <QThread>
#include <src/km2/module.h>

void AppCore::recompile() {
    km2::flags flags;
    if(verbose()) {
        flags.push_back(km2::verbose);
    }

    if(onlyTree()) {
        flags.push_back(km2::only_tree);
    }

    const auto compileThread = new std::thread([flags, this](){
        const auto compileResult = km2::compile(code().toStdString(), flags);

        lastResult = compileResult;

        setTokens(QString::fromStdString(wall_e::lex::to_string(lastResult.tokens)));
        setGramatic(QString::fromStdString(lastResult.rules));
        setTree(lastResult.token_tree);
        if(lastResult.mod) {
            setAsmCode(QString::fromStdString(lastResult.mod->llvmAssembly()));
        }
        setErrors(QList<km2::error>(lastResult.errors.begin(), lastResult.errors.end()));

        if(higlighter) {
            higlighter->setErrors(errors());
        }

        compilationCompleated(std::this_thread::get_id());
    });
    m_threads[compileThread->get_id()] = compileThread;

    connect(this, &AppCore::compilationCompleated, this, [this](std::thread::id id){
        const auto it = m_threads.find(id);
        if(it != m_threads.end()) {
            if(it->second) {
                it->second->join();
                delete it->second;
            }
            m_threads.erase(it);
        }
    }, Qt::QueuedConnection);
}

#define K_TIMER(INTERVAL, OBJECT, FUNCTION) \
    QTimer *timer = new QTimer(OBJECT); \
    connect(timer, &QTimer::timeout, OBJECT, FUNCTION); \
    timer->start(INTERVAL);

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
    recompile();
}


bool AppCore::startExecuting() {
    return executor()->start(lastResult.mod, lastResult.llvm_value);
}

QString AppCore::errToString(const km2::error &err) const {
    return QString::fromStdString(err.message());
}

int AppCore::errBegin(const km2::error &err) const {
    return err.segment().begin();
}

int AppCore::errEnd(const km2::error &err) const {
    return err.segment().end();
}
