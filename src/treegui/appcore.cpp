#include "appcore.h"
#include "highlighter.h"
#include <src/km2/km2.h>
#include <QTimer>
#include <QTextBlock>

void AppCore::recompile() {
    km2_flags flags;
    if(verbose()) {
        flags.push_back(km2_verbose);
    }

    if(onlyTree()) {
        flags.push_back(km2_only_tree);
    }

    lastResult = km2_compile(code().toStdString(), flags);

    setTokens(QString::fromStdString(wall_e::lex::to_string(lastResult.tokens)));
    setGramatic(QString::fromStdString(lastResult.rules));
    setTree(lastResult.tree);
    setAsmCode(QString::fromStdString(lastResult.assembly.code));
    setErrors(QList<km2_error>::fromStdList(lastResult.errors));

    if(higlighter) {
        higlighter->setErrors(errors());
    }
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
        }
    });
    recompile();

    //K_TIMER(1000, this, [this](){
    //    if(codeDocument()) {
    //        if(fff) {
    //
    //            //codeDocument()->textDocument()->setHtml("<p style='color: #ff00ff;' >");
    //        } else {
    //            //codeDocument()->textDocument()->setHtml("<p style='color: #ffff00;' >");
    //        }
    //        codeDocument()->textDocument()->
    //        auto it = codeDocument()->textDocument()->begin();
    //        if(it != codeDocument()->textDocument()->end()) {
    //            auto o = new QTextBlockUserData();
    //            it.setUserData(o);
    //            it.setRevision(343);
    //        }
    //        fff = !fff;
    //    }
    //});
}

bool AppCore::startExecuting() {
    return executor()->start(lastResult.assembly.code);
}

