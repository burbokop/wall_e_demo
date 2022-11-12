#include "appcore.h"
#include "wall_e/src/macro.h"
#include <src/km2/km2.h>
#include <QTimer>
#include <QTextBlock>
#include <QThread>
#include <QtConcurrent>
#include <src/km2/backend/entities/function.h>
#include <src/km2/backend/backend.h>

static QDir projDir() {
    return QDir("../../examples").absolutePath();
}

QList<ProjFile *> AppCore::scanProjFiles(QObject *parent) {
    QList<ProjFile *> result;
    const auto d = projDir();
    qDebug() << "scanning proj dir: " << d;
    QDirIterator it(d, QDirIterator::IteratorFlag::Subdirectories);
    while(it.hasNext()) {
        it.next();
        const auto info = it.fileInfo();
        if(info.isFile()) {
            result.push_back(new ProjFile(
                                 info.absoluteFilePath(),
                                 d.relativeFilePath(info.absoluteFilePath()),
                                 parent
                                 ));
        }

    }
    return result;
}


CompilationError AppCore::makeExecutable(const QString &path) {
    if(unit().valid()) {
        if(const auto err = unit().data()->make_executable(path.toStdString()).left()) {
            return CompilationError(err.value());
        }
        return {};
    }
    return CompilationError(wall_e::error("empty module"));
}

AppCore::AppCore(QObject *parent) : QObject(parent) {
    qRegisterMetaType<Either>();


    m_projectFiles = scanProjFiles(this);
}

AppCore::~AppCore() {
    if(m_currentFile) {
        if(m_currentFile->isOpen()) {
            m_currentFile->close();
        }
        delete m_currentFile;
    }
}

Either AppCore::startExecuting() {
    if(backendFactory()->currentBackend().valid()) {
        if(unit().valid()) {
            if(const auto& f = dynamic_cast<km2::backend::function*>(entry().data())) {
                return executor()->start(unit().data(), f, verbose());
            }
            return Either::newLeft("backend value is not a function");
        }
        return Either::newLeft("no compilation proceeded yet");
    } else {
        return Either::newLeft("no backend assigned");
    }
}


void AppCore::loadFile(ProjFile *f, const QString &codeToWrite) {
    if(m_currentFile) {
        writeToCurrentFile(codeToWrite);
        delete m_currentFile;
    }

    m_currentFile = new QFile(f->fullPath());
    if(m_currentFile->open(QFile::ReadOnly)) {
        const auto& content = QString::fromUtf8(m_currentFile->readAll());
        m_currentFile->close();
        emit codeLoaded(content);
        setOpenedProjFile(f);
        setCodeEdited(false);
    } else {
        qWarning() << "can not open file" << f->fullPath() << m_currentFile->errorString();
        delete m_currentFile;
    }
}

void AppCore::writeToCurrentFile(const QString &codeToWrite) {
    if(m_currentFile && !m_currentFile->isOpen()) {
        if(m_currentFile->open(QFile::WriteOnly)) {
            m_currentFile->write(codeToWrite.toUtf8());
            if(m_currentFile->error()) {
                qWarning() << "error writing file:" << m_currentFile->error();
            } else {
                setCodeEdited(false);
            }
            m_currentFile->close();
        }
    }
}

QString AppCore::readFromCurrentFile() {
    if(m_currentFile && !m_currentFile->isOpen()) {
        if(m_currentFile->open(QFile::ReadOnly)) {
            const auto& r = m_currentFile->readAll();
            m_currentFile->close();
            return r;
        }
    }
    return QString();
}

extern "C" {
int my_test_func() {
    qDebug() << "SOME Q DEBUG";
    return 0;
}
}

