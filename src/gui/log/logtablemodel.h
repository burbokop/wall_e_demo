#ifndef LOGTABLEMODEL_H
#define LOGTABLEMODEL_H

#include <QAbstractTableModel>
#include <QColor>
#include <QSize>
#include "../klibcore/kmacro.h"
#include "../compiler.h"

class LogTableModel : public QAbstractTableModel {
    Q_OBJECT
    K_AUTO_PROPERTY(GramLogSharedPtr, log, log, setLog, logChanged, GramLogSharedPtr())
    K_AUTO_PROPERTY(bool, unimportentVisible, unimportentVisible, setUnimportentVisible, unimportentVisibleChanged, false)
    K_READONLY_PROPERTY(QStringList, headerModel, headerModel, setHeaderModel, headerModelChanged, QStringList())

    struct Item {
        QString text;
        QString hoverText;
        QColor color;
    };

    QVector<Item> m_items;
    QSize m_size;

    static QPair<QVector<Item>, QStringList> convert(const GramLogSharedPtr& log, bool includeUnimportent, QSize* sizeOutput);
public:
    enum Role {
        DisplayRole = Qt::DisplayRole,
        HoverRole = Qt::UserRole,
        ColorRole
    };
    Q_ENUM(Role);

    explicit LogTableModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
};


#endif // LOGTABLEMODEL_H
