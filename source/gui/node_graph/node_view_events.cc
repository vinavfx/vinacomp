#include <node_backdrop.h>
#include <node_view.h>

void node_view::mousePressEvent(QMouseEvent *event)
{
    click_position = event->pos();
    clicked();

    if (!qt::alt())
    {
        QGraphicsItem *item =
            scene->itemAt(mapToScene(event->pos()), QTransform());
        QString item_name = item->data(0).toString();

        node_backdrop *backdrop = dynamic_cast<node_backdrop *>(item);

        // 'mousePressEvent' tiene que ir antes que el
        // 'backdrop->is_clicked_title_area' para poder usar el click del
        // backdrop primero o si no no funciona el metodo
        if (backdrop)
            graphics_view::mousePressEvent(event);
        //

        // impide la seleccion de nodos si se hizo el click en un wire
        if (item_name != "wire")
        {
            node *_node = get_node_from_position(event->pos());
            if (_node)
            {
                if (!qt::shift())
                    if (!_node->is_selected())
                        select_all(false);

                if (backdrop)
                {
                    // selectiona el backdrop si se clickeo en la barra de
                    // titulos, si es en el cuerpo deselecciona todo
                    if (backdrop->is_clicked_title_area())
                        select_node(_node->get_name(), true);
                    else if (!qt::shift())
                        select_all(false);
                }
                else
                {
                    select_node(_node->get_name(), true);
                    if (qt::control())
                    {
                        select_all(false);
                        select_connected_nodes(_node);
                    }
                }
            }
            else
            {
                if (!qt::shift())
                    select_all(false);
            }
        }

        // si el click no fue en un nodo o es un backdrop, comienza el area de
        // seleccion
        if (event->button() == Qt::LeftButton)
        {
            if (item)
            {
                if (backdrop)
                    if (!backdrop->is_clicked_title_area())
                        selecting = true;
            }
            else
                selecting = true;
        }
        //
    }

    graphics_view::mousePressEvent(event);
    node_rename_edit->hide();

    if (event->button() == Qt::RightButton)
        right_click();
}

void node_view::mouseReleaseEvent(QMouseEvent *event)
{
    selecting = false;
    selection_box->setVisible(false);

    connect_node(event->pos());
    connect_output_wire(event->pos());

    graphics_view::mouseReleaseEvent(event);
}

void node_view::mouseMoveEvent(QMouseEvent *event)
{
    if (selecting)
        if (!qt::alt())
            select_nodes_by_area(mapToScene(event->pos()));

    graphics_view::mouseMoveEvent(event);
}

void node_view::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control)
        if (!qt::shift())
            set_visible_ghost_dots(true);
    if (event->key() == Qt::Key_Shift)
        set_visible_ghost_dots(false);
    if (event->key() == Qt::Key_F)
        fit_view_to_nodes();
    if (event->key() == Qt::Key_1)
        connect_to_viewer();

    graphics_view::keyPressEvent(event);
}

void node_view::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control)
        set_visible_ghost_dots(false);

    graphics_view::keyReleaseEvent(event);
}

void node_view::focusOutEvent(QFocusEvent *event)
{
    set_visible_ghost_dots(false);
}

void node_view::resizeEvent(QResizeEvent *event)
{
    restore_rect(get_last_rect());
}
