#ifndef NODE_OUTPUT_LINK_H
#define NODE_OUTPUT_LINK_H

#include <QGraphicsItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QPen>
#include <QWidget>

class output_wire : public QGraphicsRectItem
{
private:
    QGraphicsPolygonItem *arrow;
    QGraphicsItem *this_node;
    QGraphicsLineItem *wire;
    QGraphicsScene *scene;

    QWidget *_node_view;

    void refresh_arrow(QPointF dst_pos);
    void wire_refresh(QPointF dst_pos);
    void set_visible(bool visible);

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

public:
    output_wire(QGraphicsScene *scene, QWidget *_node_view,
                QGraphicsItem *_this_node);
    ~output_wire();

    void refresh();
    void set_selected(bool enable);
};

#endif // NODE_OUTPUT_LINK_H
