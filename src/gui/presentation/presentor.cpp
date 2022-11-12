#include "presentor.h"
#include <wall_e/src/macro.h>
#include <QTimer>

QString Presentor::uriFromDoc(const QTextDocument *doc) {
    const auto& u = doc->baseUrl().toString();
    if(u.isEmpty()) {
        QString s;
        { QTextStream ss(&s); ss << doc; }
        return s;
    } else {
        return u;
    }
}

void Presentor::initialize() {
    for(const auto& c : m_documentConnections) disconnect(c);
    if(!m_serviceThread->isRunning() || !m_doc) return;
    const auto& uri = uriFromDoc(m_doc);
    if(m_higlighter) { m_higlighter->deleteLater(); }
    m_higlighter = new Highlighter(m_doc);
    m_service->initialize(uri, m_theme->capability());
    m_documentConnections.push_back(connect(m_doc, &QTextDocument::contentsChanged, this, [uri, this](){
        QMetaObject::invokeMethod(m_service, [this, uri]{ m_service->changeContent(uri, m_doc); }, Qt::QueuedConnection);
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

