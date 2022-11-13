#ifndef KGRAMTREEVIEW_H
#define KGRAMTREEVIEW_H

#include <src/gui/klibcore/kmacro.h>
#include "compiler.h"
#include <QQuickPaintedItem>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Q_DECLARE_METATYPE(wall_e::variant)
#endif

class KGramTreeView : public QQuickPaintedItem {
    Q_OBJECT
    K_QML_TYPE(KGramTreeView)
    K_AUTO_PROPERTY(TokenTree*, tree, tree, setTree, treeChanged, nullptr)
    int m_startMouseX = 0;
    int m_startMouseY = 0;
    int m_startTreeX = 0;
    int m_startTreeY = 0;
    int m_currentMouseX = 0;
    int m_currentMouseY = 0;
    int m_treeX = 100;
    int m_treeY = 100;
    double m_scaleMultiplier = 1;
    static int stringRadius(const std::string &string);
    static void drawBranch(const wall_e::variant &branch, int x, int y, QVector2D spacingMultiplier, QPainter *painter);
    static int branchWidth(const wall_e::variant &branch, bool onlyCells = false);
public:
    KGramTreeView(QQuickItem *parent = nullptr);

public slots:
    void openInBrowser();

    // QObject interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    // QQuickPaintedItem interface
public:
    virtual void paint(QPainter *painter) override;

    // QQuickItem interface
protected:
    virtual void wheelEvent(QWheelEvent *event) override;
};



#endif // TREEWIDGET_H
