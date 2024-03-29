#ifndef NODE_DOT_H
#define NODE_DOT_H

#include "node.h"

class node_dot : public node
{
private:
    QString icon_name;
    QGraphicsTextItem *tips_text;

    void set_size(int width, int height);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

public:
    node_dot(node_props _props, QMap<QString, node *> *_selected_nodes,
             QWidget *_node_graph);
    ~node_dot();
};

#endif // NODE_RECT_H
