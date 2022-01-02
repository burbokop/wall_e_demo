#include "kgramtreeview.h"

#include <QPaintEvent>
#include <QPainter>
#include <wall_e/src/lex.h>
#include <wall_e/src/gram.h>
#include <src/km2/km2.h>
#include <src/km2/tree/abstract/abstract_node.h>
#include <QQmlListProperty>

KGramTreeView::KGramTreeView(QQuickItem *parent) : QQuickPaintedItem(parent) {
    setAcceptedMouseButtons(Qt::LeftButton);
    connect(this, &KGramTreeView::treeChanged, this, &KGramTreeView::displayTree);
}


void KGramTreeView::displayTree(const wall_e::variant &var) {
    m_var = var;
    update();
}


void KGramTreeView::paint(QPainter *painter) {
    painter->setPen(QPen(QColor(Qt::black), 4));
    painter->setRenderHint(QPainter::Antialiasing);


    auto ox = painter->window().width() / 2;
    auto oy = painter->window().height() / 2;
    //auto ox = currentMouseX;
    //auto oy = currentMouseY;

    painter->translate(ox, oy);
    painter->scale(scaleMultiplier, scaleMultiplier);
    painter->translate(-ox, -oy);

    painter->drawRect(treeX - 5, treeY - 5, 10, 10);
    print_branch(m_var, treeX, treeY, painter);
}

int KGramTreeView::string_radius(const std::string &string) {
    int bnc = 1;
    for(size_t i = 0; i < string.size(); ++i) {
        if(string[i] == '\n') {
            bnc++;
        }
    }
    return string.size() / bnc;
}

void KGramTreeView::print_branch(const wall_e::variant &branch, int x, int y, QPainter *painter) {
    const auto cellMargins = 5;
    const auto cellRadius = painter->font().pointSize() * branch_width(branch, true) / 2 + cellMargins;
    const auto spacingX = 50;
    const auto spacingY = branch.contains_type<wall_e::variant_vector>() ? 100 : 50 + cellRadius;

    painter->drawLine(QLine(x, y, x, y + spacingY - cellRadius));
    QRect cellRect(x - cellRadius, y + spacingY - cellRadius, cellRadius * 2, cellRadius * 2);
    painter->drawRect(cellRect);

    if(branch.contains_type<wall_e::variant_vector>()) {
        auto vec = branch.value<wall_e::variant_vector>();

        int pos = 0;
        for(size_t i = 0, cnt = vec.size(); i < cnt; ++i) {
            const int w = (branch_width(vec[i]) + 1) * spacingX;
            print_branch(vec[i], x + pos, y + spacingY, painter);
            if(i == vec.size() - 1) {
                painter->drawLine(QLine(x, y + spacingY, x + pos, y + spacingY));
            }
            pos += w;
        }
    } else if(branch.contains_type<wall_e::lex::token>()) {
        painter->drawText(cellRect, QString::fromStdString(branch.value<wall_e::lex::token>().text), QTextOption(Qt::AlignCenter));
    } else if(branch.contains_type<std::string>()) {
        painter->drawText(cellRect, QString::fromStdString(branch.value<std::string>()), QTextOption(Qt::AlignCenter));
    } else if(branch.contains_type<int>()) {
        painter->drawText(cellRect, QString::number(branch.value<int>()), QTextOption(Qt::AlignCenter));
    } else if(branch.inherited_by<km2::abstract_node*>()) {
        painter->drawText(cellRect, QString::fromStdString(branch.type()), QTextOption(Qt::AlignCenter));
    } else {
        const auto penBackup = painter->pen();
        painter->setPen(QPen(QColor(Qt::yellow | Qt::green), penBackup.width()));
        painter->drawRect(cellRect);
        painter->drawText(cellRect, "???", QTextOption(Qt::AlignCenter));
        painter->setPen(penBackup);
    }
}

int KGramTreeView::branch_width(const wall_e::variant &branch, bool onlyCells) {
    if(branch.contains_type<wall_e::variant_vector>()) {
        if(onlyCells)
            return 0;

        auto vec = branch.value<wall_e::variant_vector>();
        int count = 0;
        for(size_t i = 0, cnt = vec.size(); i < cnt; ++i) {
            count += branch_width(vec[i], onlyCells);
        }
        return count;
    } else if(branch.contains_type<wall_e::lex::token>()) {
        return string_radius(branch.value<wall_e::lex::token>().text);
    } else if(branch.contains_type<std::string>()) {
        return string_radius(branch.value<std::string>());
    } else if(branch.contains_type<int>()) {
        return std::to_string(branch.value<int>()).size();
    } else if(branch.inherited_by<km2::abstract_node*>()) {
        return string_radius(branch.type());
    } else {
        return 3;
    }
}

void KGramTreeView::mousePressEvent(QMouseEvent *event) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    startMouseX = event->x();
    startMouseY = event->y();
#else
    startMouseX = event->position().x();
    startMouseY = event->position().y();
#endif
    startTreeX = treeX;
    startTreeY = treeY;

    event->accept();
}

void KGramTreeView::mouseMoveEvent(QMouseEvent *event) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    treeX = startTreeX + (event->x() - startMouseX) / scaleMultiplier;
    treeY = startTreeY + (event->y() - startMouseY) / scaleMultiplier;
#else
    treeX = startTreeX + (event->position().x() - startMouseX) / scaleMultiplier;
    treeY = startTreeY + (event->position().y() - startMouseY) / scaleMultiplier;
#endif
    update();
    event->accept();
}


void KGramTreeView::wheelEvent(QWheelEvent *event) {
    QPoint numDegrees = event->angleDelta() / 8;

    currentMouseX = event->position().x();
    currentMouseY = event->position().y();

    if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        if(numSteps.y() > 0) {
            scaleMultiplier *= 1.5;
        } else if(numSteps.y() < 0) {
            scaleMultiplier /= 1.5;
        }
    }

    update();
    event->accept();
}
