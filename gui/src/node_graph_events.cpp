#include <node_graph.hpp>

void node_graph::mousePressEvent(QMouseEvent *event)
{
    // print(scene->selectedItems());
    if (event->button() == Qt::RightButton)
    {
        nodes->value(0)->connect_input(0, nodes->value(1));
    }

    pressed = true;
    if (event->button() == Qt::MidButton)
    {
        panning = true;
        panning_start_x = event->x();
        panning_start_y = event->y();
        event->accept();
        return;
    };
    event->ignore();
    QGraphicsView::mousePressEvent(event);
}

void node_graph::mouseReleaseEvent(QMouseEvent *event)
{
    pressed = false;
    if (event->button() == Qt::MidButton)
    {
        panning = false;
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
    event->ignore();
}

void node_graph::mouseMoveEvent(QMouseEvent *event)
{
    if (panning)
    {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->x() - panning_start_x));
        verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->y() - panning_start_y));
        panning_start_x = event->x();
        panning_start_y = event->y();
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
    event->ignore();
}

void node_graph::wheelEvent(QWheelEvent *event)
{
    if (event->delta() > 0)
        scale(1.25, 1.25);
    else
        scale(0.8, 0.8);
}