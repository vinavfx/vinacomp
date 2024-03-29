#include <maker.h>
#include "node.h"
#include <node_graph.h>
#include <input_wire_ghost_dot.h>
#include <node_view.h>
#include <util.h>

ghost_dot::ghost_dot(int size, QWidget *__node_graph, QGraphicsItem *_input)
    : QGraphicsEllipseItem(0, 0, size, size)
    , _node_graph(__node_graph)
    , input(_input)
{
    this->setBrush(Qt::green);
}

ghost_dot::~ghost_dot() {}

void ghost_dot::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    node_graph *graph = static_cast<node_graph *>(_node_graph);

    node_view *__node_view = graph->get_node_view();
    maker *_maker = graph->get_maker();
    input_wire *_input = static_cast<input_wire *>(input);

    __node_view->set_visible_ghost_dots(false);
    node *dot = _maker->create_fx("dot", true);

    _input->insert_node_in_between(dot);

    dot->grabMouse();
}
