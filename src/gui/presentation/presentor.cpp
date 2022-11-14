#include "presentor.h"
#include <wall_e/src/macro.h>
#include <QTimer>


QString Presentor::substituteUri(const QString &uri) {
    if(uri.isEmpty()) {
        static int next = 0;
        return "unknown_uri_" + QString::number(next++);
    } else {
        return uri;
    }
}

void Presentor::initialize() {
    for(const auto& c : m_documentConnections) disconnect(c);
    if(!m_serviceThread->isRunning() || !m_doc) return;
    const auto& u = substituteUri(uri());
    if(m_higlighter) { m_higlighter->deleteLater(); }
    m_higlighter = new Highlighter(m_doc);
    m_service->initialize(u, m_theme->capability());
    m_documentConnections.push_back(connect(m_doc, &QTextDocument::contentsChanged, this, [u, this](){
        QMetaObject::invokeMethod(m_service, [this, u]{ m_service->changeContent(u, m_doc); }, Qt::QueuedConnection);
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

    connect(m_service, &LSPService::contentChanged, this, [this](const QList<CompilationError>& errs, const QList<SemanticToken>& tokens, bool astTokensReady){
        setAstTokensReady(astTokensReady);
        setErrors(errs);
        if(m_higlighter) {
            m_higlighter->setErrorsAndSemanticTokens(errs, tokens);
        }
    });

    connect(m_service, &LSPService::initialized, this, [this](const SemanticTokensLegend& legend){
        if(m_higlighter) {
            m_higlighter->setLegend(m_theme->highlightLegend(legend));
        }
    });

    m_serviceThread->start();
}

Presentor::~Presentor() {
    if(m_serviceThread) {
        m_serviceThread->quit();
        m_serviceThread->wait();
    }
}

