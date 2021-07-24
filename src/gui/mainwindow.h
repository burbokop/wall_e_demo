#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QDataStream>

#include "kgramtreeview.h"


template<typename T>
class AutoSaver {
    std::function<T()> m_getter;
    QString m_id;
public:
    AutoSaver(std::function<T()> getter, std::function<void(T)> setter, QString id) {
        m_getter = getter;
        m_id = id;
        QFile file(id);
        if(file.open(QIODevice::ReadOnly)) {
            auto ba = file.readAll();
            QDataStream stream(&ba, QIODevice::ReadOnly);
            T tmp;
            stream >> tmp;
            setter(tmp);
        }
    }
    ~AutoSaver() {
        QFile file(m_id);
        if(file.open(QIODevice::WriteOnly)) {
            QByteArray ba;
            QDataStream stream(&ba, QIODevice::WriteOnly);
            stream << m_getter();
            file.write(ba);
        }
    }
};


class MainWindow : public QObject {
    Q_OBJECT
    //QTextEdit *textEdit = new QTextEdit(this);
    //TreeWidget *m_treeWidget = new TreeWidget(this);
    //QTextEdit *rulesArea = new QTextEdit(this);
    //QTextEdit *tokensArea = new QTextEdit(this);
    //QCheckBox *treeModeChackBox = new QCheckBox(this);
    //QCheckBox *verboseChackBox = new QCheckBox(this);
    //AutoSaver<QString> textSaver = AutoSaver<QString>([this](){ return textEdit->toPlainText(); }, [this](auto text){ textEdit->setText(text); updateTree(); }, "code");
public:
    explicit MainWindow(QObject *parent = nullptr);
    //TreeWidget *treeWidget() const;

public slots:
    void updateTree();


signals:

};

#endif // MAINWINDOW_H
