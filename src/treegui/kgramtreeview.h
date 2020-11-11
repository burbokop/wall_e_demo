#ifndef KGRAMTREEVIEW_H
#define KGRAMTREEVIEW_H

#include <wall_e/src/kgramvariant.h>
#include <src/treegui/klibcore/kmacro.h>
#include <QQuickPaintedItem>

Q_DECLARE_METATYPE(kgram_variant_t)

class KGramTreeView : public QQuickPaintedItem {
    Q_OBJECT
    K_QML_TYPE(KGramTreeView)
    K_AUTO_PROPERTY(kgram_variant_t, tree, tree, setTree, treeChanged, kgram_variant_t())
    K_META_TYPE(kgram_variant_t)

    kgram_variant_t m_var;
    int startMouseX = 0;
    int startMouseY = 0;
    int startTreeX = 0;
    int startTreeY = 0;
    int treeX = 100;
    int treeY = 100;
    static int string_radius(const std::string &string);
    static void print_branch(const kgram_variant_t &branch, int x, int y, QPainter *painter);
    static int branch_width(const kgram_variant_t &branch, bool onlyCells = false);
    double scaleMultiplier = 1;

    void displayTree(const kgram_variant_t &var);
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
