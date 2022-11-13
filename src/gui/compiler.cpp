#include "compiler.h"
#include <src/km2/tree/abstract/abstract_node.h>
#include <src/km2/backend/unit/unit.h>

#include <QtConcurrent>

Compiler::Compiler(QObject *parent) : QObject { parent } {
    connect(this, &Compiler::codeChanged, this, &Compiler::recompile);
    connect(this, &Compiler::onlyTreeChanged, this, &Compiler::recompile);
    connect(this, &Compiler::verboseChanged, this, &Compiler::recompile);
    connect(this, &Compiler::backendChanged, this, &Compiler::recompile);

    connect(&m_currentFutureWatcher, &QFutureWatcher<km2::compilation_result>::finished, this, [this](){
        this->completeCompilation(m_currentFutureWatcher.result());
    });

    recompile();
}


void Compiler::recompile() {
    setErrors({});
    if(!backend().valid()) {
        addErrors({ CompilationError(wall_e::error("backend not choosed", wall_e::error::warn)) });
    }
    qDebug() << "RRRRRRRRRRRRRR:" << verbose();

    km2::flags flags;
    if(verbose()) {
        flags.push_back(km2::Verbose);
    }

    if(onlyTree()) {
        flags.push_back(km2::OnlyTree);
    }

    if(m_firstCompilation) {
        completeCompilation(km2::compile(backend().data_ptr().get(), code().toStdString(), flags));
        m_firstCompilation = false;
    } else {
        if(m_currentFutureWatcher.isStarted()) {
            m_currentFutureWatcher.cancel();
            m_currentFuture.cancel();
        }
        const auto compile = [](const km2::backend::backend* b, const std::string &input, const km2::flags &flags = {}){ return km2::compile(b, input, flags); };
        m_currentFuture = QtConcurrent::run(compile, backend().data_ptr().get(), code().toStdString(), flags);
        m_currentFutureWatcher.setFuture(m_currentFuture);
    }
}

QList<CompilationError> Compiler::errorsFromWallE(const wall_e::list<wall_e::error> &lst) {
    QList<CompilationError> result;
    for(const auto& e : lst) {
        result.push_back(CompilationError(e));
    }
    return result;
}

void Compiler::completeCompilation(const km2::compilation_result &cresult) {
    setTokens(QString::fromStdString(wall_e::lex::to_string(cresult.tokens())));
    if(cresult.root_node()) {
        const auto& tokens = cresult.root_node()->tokens();
        if(tokens.size() > 0) {
            setAstTokens(QString::fromStdString(km2::to_string(tokens)));
        } else {
            setAstTokens("can not get ast tokens: empty list");
        }
    } else {
        setAstTokens("can not get ast tokens: root node is nullptr");
    }

    setGramatic(QString::fromStdString(cresult.rules()));

    if(tree()) tree()->deleteLater();
    setTree(TokenTree::fromWallEVariant(cresult.token_tree(), this));

    qDebug() << "COMPILATION COMPLETE";
    if(cresult.unit()) {
        setAsmCode(QString::fromStdString(cresult.unit()->llvm_assembly()));
    } else {
        setAsmCode("err: empty module");
    }
    addErrors(errorsFromWallE(cresult.errors()));
    setUnit(UnitSharedPtr(cresult.unit()));
    setRootBackendValue(BackendValuePtr(cresult.backend_value()));
    setLog(GramLogSharedPtr(cresult.gram_log()));

    m_prevResult = cresult;
    emit compilationCompleated();
}


TokenTree *TokenTree::fromWallEVariant(const wall_e::variant &data, QObject *parent) {
    TokenTree *res = new TokenTree(parent);
    res->m_data = data;
    return res;
}
