#ifndef KGRAMTREEVIEW_H
#define KGRAMTREEVIEW_H

#include <wall_e/src/variant.h>
#include <src/gui/klibcore/kmacro.h>
#include <QQuickPaintedItem>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Q_DECLARE_METATYPE(wall_e::variant)
#endif

class KGramTreeView : public QQuickPaintedItem {
    Q_OBJECT
    K_QML_TYPE(KGramTreeView)
    K_AUTO_PROPERTY(wall_e::variant, tree, tree, setTree, treeChanged, wall_e::variant())
    K_META_TYPE(wall_e::variant)

    wall_e::variant m_var;
    int startMouseX = 0;
    int startMouseY = 0;
    int startTreeX = 0;
    int startTreeY = 0;
    int treeX = 100;
    int treeY = 100;
    static int string_radius(const std::string &string);
    static void print_branch(const wall_e::variant &branch, int x, int y, QPainter *painter);
    static int branch_width(const wall_e::variant &branch, bool onlyCells = false);
    double scaleMultiplier = 1;

    void displayTree(const wall_e::variant &var);
public:
    KGramTreeView(QQuickItem *parent = nullptr);


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
