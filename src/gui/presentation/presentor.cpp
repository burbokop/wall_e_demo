#include "presentor.h"
#include <wall_e/src/macro.h>
#include <QTimer>

#include <filesystem>

bool Presentor::compareUrl(const QUrl &u0, const QUrl &u1) {
    return u0 == u1;
    //if(ur)
    //return std::filesystem::path(u0.toStdString()) == std::filesystem::path(u1.toStdString());
}

QUrl Presentor::substituteUrl(const QUrl &url) {
    if(url.isEmpty()) {
        static int next = 0;
        return "doi://" + QString::number(next++);
    } else {
        return url;
    }
}

void Presentor::initialize() {
    for(const auto& c : m_documentConnections) disconnect(c);
    if(!m_serviceThread->isRunning() || !m_doc) return;
    if(m_higlighter) { m_higlighter->deleteLater(); }
    m_higlighter = new Highlighter(m_doc);
    QMetaObject::invokeMethod(m_service, [this, cap = m_theme->capability()]{ m_service->initialize(substituteUrl(url()), cap); }, Qt::QueuedConnection);
    m_documentConnections.push_back(connect(m_doc, &QTextDocument::contentsChanged, this, [this](){
        QMetaObject::invokeMethod(m_service, [this]{ m_service->changeContent(substituteUrl(url()), m_doc); }, Qt::QueuedConnection);
    }));
}

Presentor::Presentor(QObject *parent) : QObject{ parent } {
    m_theme = Theme::qtCreator(this);
    m_serviceThread = new QThread(this);
    m_service = new LSPService;
    m_service->moveToThread(m_serviceThread);

    connect(this, &Presentor::codeDocumentChanged, this, [this](QQuickTextDocument *v) {
        if(v) {
            m_doc = v->textDocument();
            initialize();
        } else {
            m_doc = nullptr;
            if(m_higlighter) { m_higlighter->deleteLater(); }
        }
    });
    connect(m_serviceThread, &QThread::started, this, [this](){
        initialize();
    });

    connect(m_service, &LSPService::contentChanged, this, [this](const QUrl&, const QList<CompilationError>& errs, const QList<SemanticToken>& tokens, bool astTokensReady){
        setAstTokensReady(astTokensReady);
        setErrors(errs);
        if(m_higlighter) {            
            QList<CompilationError> thisUriErrors;
            for(const auto& e : errs) {
                if(compareUrl(e.url(), url())) {
                    thisUriErrors.push_back(e);
                }
            }

            m_higlighter->setErrorsAndSemanticTokens(thisUriErrors, tokens);
        }
    });

    connect(m_service, &LSPService::initialized, this, [this](const QUrl&, const SemanticTokensLegend& legend){
        if(m_higlighter) {
            m_higlighter->setLegend(m_theme->highlightLegend(legend));
        }
    });

    connect(m_service, &LSPService::hover, this, [this](const QUrl&, const MarkupString& str) {
        emit hover(str);
    });

    connect(m_service, &LSPService::unhover, this, [this](const QUrl&) {
        emit unhover();
    });

    m_serviceThread->start();
}

Presentor::~Presentor() {
    if(m_serviceThread) {
        m_serviceThread->quit();
        m_serviceThread->wait();
    }
}

void Presentor::hoverText(int pos) {
    if(m_service) {
        QMetaObject::invokeMethod(m_service, [this, pos]{ m_service->hoverText(substituteUrl(url()), pos); }, Qt::QueuedConnection);
    }
}

