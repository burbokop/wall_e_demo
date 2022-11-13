#include "logtablemodel.h"


LogTableModel::LogTableModel(QObject *parent) : QAbstractTableModel{ parent } {
    connect(this, &LogTableModel::logChanged, this, [this](const GramLogSharedPtr& log) {
        beginResetModel();
        const auto& res = convert(log, unimportentVisible(), &m_size);
        m_items = res.first;
        setHeaderModel(res.second);
        endResetModel();
    });
    connect(this, &LogTableModel::unimportentVisibleChanged, this, [this](bool u) {
        beginResetModel();
        const auto& res = convert(log(), u, &m_size);
        m_items = res.first;
        setHeaderModel(res.second);
        endResetModel();
    });
}

int LogTableModel::rowCount(const QModelIndex &parent) const {
    return m_size.height();
}

int LogTableModel::columnCount(const QModelIndex &parent) const {
    return m_size.width();
}

QHash<int, QByteArray> LogTableModel::roleNames() const {
    return {
        { DisplayRole, "display" },
        { HoverRole, "hover" },
        { ColorRole, "textColor" }
    };
}

QVariant LogTableModel::data(const QModelIndex &index, int role) const {
    const auto i = index.column() + m_size.width() * index.row();
    if(i < m_items.size()) {
        const auto& item = m_items[i];
        if(role == DisplayRole) {
            return item.text;
        } else if(role == HoverRole) {
            return item.hoverText;
        } else if(role == ColorRole) {
            return item.color;
        }
    }
    return {};
}

QPair<QVector<LogTableModel::Item>, QStringList> LogTableModel::convert(const GramLogSharedPtr &log, bool includeUnimportent, QSize *sizeOutput) {
    if(!log.valid()) {
        sizeOutput->setWidth(0);
        sizeOutput->setHeight(0);
        return {};
    }

    QVector<LogTableModel::Item> result;
    const auto& steps = includeUnimportent
            ? log.data()->steps()
            : log.data()->steps().filter([](const wall_e::gram::log::step& s){ return s.importent; });

    const auto& tokens = log.data()->tokens();

    sizeOutput->setWidth(tokens.size());
    sizeOutput->setHeight(steps.size());
    result.resize(sizeOutput->width() * sizeOutput->height());

    QStringList headerModel;
    for(std::size_t i = 0; i < tokens.size(); ++i) {
        headerModel.push_back(QString::fromStdString(tokens[i].name));
    }

    for(std::size_t i = 0; i < steps.size(); ++i) {
        const auto x = steps[i].token_index;
        const auto y = i;
        const auto index = x + sizeOutput->width() * y;
        if(index < result.size()) {
            result[index] = Item {
                .text = QString::fromStdString(steps[i].short_text),
                .hoverText = QString::fromStdString(steps[i].hover_text),
                .color = QColor(steps[i].color.argb())
            };
        }
    }
    return { result, headerModel };
}
