#ifndef NODE_LINK_H
#define NODE_LINK_H

#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include <QPoint>
#include <QString>

// Engine
#include <project_struct.h>

class node_link : public QGraphicsRectItem
{
private:
    QGraphicsScene *scene;
    QGraphicsItem *this_node;
    QGraphicsItem *connected_node;
    QGraphicsEllipseItem *ghost_dot;
    QJsonObject *link_connecting;
    project_struct *project;
    QWidget *_vinacomp;

    int link_size;
    int index;
    bool dragging;
    bool visible;
    QString label;
    bool has_mask;
    bool ghost_dot_visible;
    int ghost_dot_size;

    QGraphicsLineItem *link;
    QGraphicsPolygonItem *arrow;
    QGraphicsTextItem *text;

    float arrow_refresh(QPointF point_a, QPointF point_b);
    void link_refresh(QPointF point_a, QPointF point_b);
    void bbox_refresh(QPointF point_a, QPointF point_b);
    void text_refresh(QPointF point_a, QPointF point_b);

    QLineF subtract_distance_line(QLineF line, float distance);
    float get_rotation(QPointF point_a, QPointF point_b);
    float get_long(QPointF point_a, QPointF point_b);
    QPointF get_center(QPointF point_a, QPointF point_b);
    void update_visibility();

    inline void set_visible(bool visible);

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

public:
    node_link(QString input_label, bool has_mask, int _index,
              QGraphicsScene *_scene, QGraphicsItem *_node,
              QJsonObject *_link_connecting, project_struct *_project,
              QWidget *_vinacomp);
    ~node_link();

    void refresh();
    void update_connection();
    void connect_node(QGraphicsItem *_node);
    void disconnect_node();
    void set_selected(bool enable);
    inline QGraphicsItem *get_connected_node() const;
    inline void set_visible_ghost_dot(bool visible);
};

inline void node_link::set_visible_ghost_dot(bool visible)
{
    ghost_dot_visible = visible;
    refresh();
}

inline QGraphicsItem *node_link::get_connected_node() const
{
    return connected_node;
}

inline void node_link::set_visible(bool _visible)
{
    visible = _visible;
    this->setVisible(visible);
    link->setVisible(visible);
    arrow->setVisible(visible);
}

#endif // NODE_LINK_H
