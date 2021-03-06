#include "kgramtreeview.h"

#include <QPaintEvent>
#include <QPainter>
#include <wall_e/src/lex.h>
#include <wall_e/src/gram.h>
#include <wall_e/src/utility/asm_tools.h>
#include <src/km2/km2.h>



KGramTreeView::KGramTreeView(QQuickItem *parent) : QQuickPaintedItem(parent) {
    setAcceptedMouseButtons(Qt::LeftButton);
    connect(this, &KGramTreeView::treeChanged, this, &KGramTreeView::displayTree);
}


void KGramTreeView::displayTree(const wall_e::variant &var) {
    m_var = var;
    update();
}


void KGramTreeView::paint(QPainter *painter) {
    painter->setPen(QPen(QColor("#000000"), 4));
    painter->setRenderHint(QPainter::Antialiasing);

    painter->translate(treeX, treeY);
    painter->scale(scaleMultiplier, scaleMultiplier);
    painter->translate(-treeX, -treeY);

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
    } else if(branch.contains_type<wall_e::asm_unit>()) {
        painter->drawText(cellRect, QString::fromStdString(branch.value<wall_e::asm_unit>().code), QTextOption(Qt::AlignCenter));
    } else {
        const auto penBackup = painter->pen();
        painter->setPen(QPen(QColor("#ff8800"), penBackup.width()));
        painter->drawRect(cellRect);
        if(branch.contains_type<wall_e::gram::recursion_error>()) {
            painter->drawText(cellRect,  QString::fromStdString(wall_e::type_name<wall_e::gram::recursion_error>()), QTextOption(Qt::AlignCenter));
        } else {
            painter->drawText(cellRect, "???", QTextOption(Qt::AlignCenter));
        }
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
    } else if(branch.contains_type<wall_e::gram::recursion_error>()) {
        return wall_e::type_name<wall_e::gram::recursion_error>().size();
    } else if(branch.contains_type<int>()) {
        return std::to_string(branch.value<int>()).size();
    } else if(branch.contains_type<wall_e::asm_unit>()) {
        return string_radius(branch.value<wall_e::asm_unit>().code);
    } else {
        return 3;
    }
}



void KGramTreeView::mousePressEvent(QMouseEvent *event) {
    startMouseX = event->x();
    startMouseY = event->y();
    startTreeX = treeX;
    startTreeY = treeY;

    event->accept();
}

void KGramTreeView::mouseMoveEvent(QMouseEvent *event) {
    treeX = startTreeX + event->x() - startMouseX;
    treeY = startTreeY + event->y() - startMouseY;
    update();
    event->accept();
}


void KGramTreeView::wheelEvent(QWheelEvent *event) {
    QPoint numDegrees = event->angleDelta() / 8;

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
