#include "node_backdrop.h"
#include "node_dot.h"
#include <knob_check_box.h>
#include <node_view.h>
#include <vinacomp.h>

node *node_view::create_node(node_struct node_data, bool basic_creation,
                             bool from_project)
{
    // la 'basic_creation' : crea position bajo el cursor, no conecta el nodo y
    // no lo selecciona

    node_props props;

    props.scene = scene;
    props.current_z_value = current_z_value;
    props.link_connecting = link_connecting;
    props.color = node_data.color;
    props.type = node_data.type;
    props.name = node_data.name;
    props.tips = node_data.tips;
    props.z_value = node_data.z_value;
    props.size = node_data.size;
    props._properties = _properties;
    props.vinacomp = _vinacomp;
    props.nodes_loaded = nodes_loaded;
    props.project = project;
    props.from_project = from_project;

    QPointF position = node_data.pos;
    QString type = node_data.type;

    // creacion de nodo segun clase
    node *_node;
    node_backdrop *backdrop = nullptr;
    if (type == "dot")
        _node = new node_dot(props, selected_nodes, _node_graph);
    else if (type == "backdrop")
    {
        backdrop = new node_backdrop(props, selected_nodes, this);
        _node = backdrop;
    }
    else if (type == "group")
        _node = new node_group(props, selected_nodes, _node_graph);
    else
        _node = new node_rect(props, selected_nodes, _node_graph);
    //
    //

    node *selected_node = get_selected_node();

    // si al posicion no viene del un proyecto
    if (!from_project)
    {
        if (selected_node && !basic_creation)
        {
            position = get_min_node_separation(_node, selected_node);
        }
        else
        {
            // crea el nodo en la posicion del puntero del mouse
            QPoint origin = this->mapFromGlobal(QCursor::pos());
            position = this->mapToScene(origin);

            auto size = _node->get_size();
            position = {position.x() - (size.width() / 2),
                        position.y() - (size.height() / 2)};
        }
    }
    //
    //

    if (backdrop)
    {
        if (!backdrop->is_selected_nodes())
            _node->set_position(position.x(), position.y());
    }
    else
    {
        _node->set_position(position.x(), position.y());
    }

    nodes->insert(node_data.name, _node);

    // inserta un item de nodo en el proyecto, con los atributos necesarios para
    // renderizar el resto de atributos se generan al guardar el proyecto
    project->insert_node(node_data.name, node_data.type);
    //

    if (!basic_creation && !backdrop)
        // conecta los nodos conectado al nodo seleccionado, al nuevo nodo
        connect_node_to_selected_nodes(_node);

    if (!basic_creation)
    {
        select_all(false);
        select_node(node_data.name, true);
    }

    return _node;
}

void node_view::delete_selected_nodes()
{
    // crea una copia para que no de conflicto ya que en selected_nodes se
    // borraran items
    auto delete_nodes = *selected_nodes;

    for (node *node_to_delete : delete_nodes)
        delete_node(node_to_delete->get_name());
}

void node_view::delete_node(QString name)
{
    node *_node = get_node(name);
    if (!_node)
        return;

    if (_node->get_type() != "backdrop")
        extract_node(_node);

    selected_nodes->remove(name);
    nodes->remove(name);
    project->delete_node(name);

    delete _node;
}

void node_view::extract_selected_nodes()
{
    for (node *selected_node : *selected_nodes)
        extract_node(selected_node);
}

void node_view::extract_node(node *_node)
{
    // extrae los nodos de sus conecciones, y los nodos que estaban conectados,
    // los conecta a los nodos anterior y siguiente, esto sirve para el
    // 'delete_node' y 'cut_node'

    QGraphicsItem *node_from_input_1 = nullptr;
    node_link *link_1 = _node->get_link(1);
    if (link_1)
        node_from_input_1 = link_1->get_connected_node();

    for (node_link *link : *_node->get_links())
        link->disconnect_node();

    for (node_link *output_link : _node->get_output_links())
    {
        if (node_from_input_1)
            output_link->connect_node(node_from_input_1);
        else
            output_link->disconnect_node();
    }

    _node->set_position(_node->x() + _node->get_size().width() + 40,
                        _node->y());
}

node *node_view::get_node(QString name)
{
    return nodes->value(name);
}

void node_view::rename_node(node *_node, QString name, QString new_name)
{
    _node->set_name(new_name);

    nodes->remove(name);
    nodes->insert(new_name, _node);

    selected_nodes->remove(name);
    selected_nodes->insert(new_name, _node);

    _node->refresh();
}

node *node_view::get_selected_node() const
{
    // obtiene 1 solo nodo seleccionado, si hay mas de 1 nodo seleccionado
    // retorna nulo y si no hay nodos seleccionado retorna nulo tambien
    if (selected_nodes->count() != 1)
        return nullptr;

    node *_node = selected_nodes->first();
    return _node;
}

void node_view::select_node(QString name, bool select)
{
    node *_node = get_node(name);
    if (!_node)
        return;

    _node->set_selected(select);

    if (select)
        selected_nodes->insert(name, _node);
    else
        selected_nodes->remove(name);
}

void node_view::connect_node_to_selected_nodes(node *_node)
{
    if (selected_nodes->empty())
        return;

    auto *links = _node->get_links();

    // crea una lista con los link y nodos seleccionados que se van a conectar
    QList<std::pair<node_link *, node *>> items_to_connect;
    for (int i = 1; i < links->count(); i++)
    {
        node_link *link = links->value(i);

        node *selected_node = selected_nodes->values().value(i - 1);
        if (!selected_node)
            break;

        items_to_connect.push_back({link, selected_node});
    }
    //
    //

    auto connect_link = [=](node_link *link, node *to_node) {
        link->connect_node(to_node);

        // conecta los nodos conectado al nodo seleccionado, al nuevo nodo
        for (node_link *sel_link : to_node->get_output_links())
            sel_link->connect_node(_node);
    };

    int count = items_to_connect.count();

    // si hay 2 nodos que se pueden conectar, deja el nodo esquinado entre los 2
    // y conecta el nodo de mas arriba al link 1
    if (count == 2)
    {
        node *node_a = items_to_connect.first().second;
        node *node_b = items_to_connect.last().second;

        QPointF position;
        if (node_a->y() < node_b->y())
        {
            connect_link(links->value(1), node_a);
            connect_link(links->value(2), node_b);
            position = {node_a->get_center_position().x(),
                        node_b->get_center_position().y()};
        }
        else
        {
            connect_link(links->value(1), node_b);
            connect_link(links->value(2), node_a);
            position = {node_b->get_center_position().x(),
                        node_a->get_center_position().y()};
        }

        _node->set_center_position(position.x(), position.y());
    }
    else
    {
        for (auto item : items_to_connect)
            connect_link(item.first, item.second);

        // si los nodos seleccionados no son 2, deja el primer nodo bajo el otro
        node *first_node = items_to_connect.first().second;
        QPointF position = get_min_node_separation(_node, first_node);
        _node->set_position(position.x(), position.y());
    }
}

void node_view::find_nodes(QString key)
{
    // encuentra los nodos a partir de una key los selecciona
    // y los ajusta al 'node_view'
    select_all(false);
    if (key.isEmpty())
    {
        fit_view_to_nodes();
        return;
    }

    key = key.toUpper();
    QList<node *> finded_nodes;
    for (node *_node : *nodes)
    {
        if (_node->get_name().toUpper().contains(key))
            select_node(_node->get_name(), true);
    }

    fit_view_to_nodes();
}

void node_view::select_connected_nodes(node *root_node)
{
    select_node(root_node->get_name(), true);

    for (node *_node : *root_node->get_input_nodes())
        select_connected_nodes(_node);
}

void node_view::disable_selected_nodes()
{
    if (selected_nodes->empty())
        return;

    bool disable = false;
    bool keep_disable = false;
    for (node *_node : *selected_nodes)
    {
        node_rect *_node_rect = dynamic_cast<node_rect *>(_node);
        if (!_node_rect)
            continue;

        if (_node_rect->get_type() == "viewer")
            continue;

        if (!keep_disable)
        {
            disable = _node_rect->is_disable();
            keep_disable = true;
        }

        _node_rect->set_disable(!disable);

        // desabilita el nodo en el proyecto
        auto *params = _node->get_params();
        if (!disable)
            params->insert("disable_node", true);
        else
        {
            params->remove("disable_node");
            params->remove("disable_node_anim");
        }
        //

        trim_panel *panel = _node->get_trim_panel();
        if (panel)
        {
            knob_check_box *disable_node_knob =
                static_cast<knob_check_box *>(panel->get_knob("disable_node"));
            disable_node_knob->set_check(!disable, false);
        }
    }
    static_cast<vinacomp *>(_vinacomp)->update_render_all_viewer(true);
}

node_link *node_view::get_node_link(node *_node, int link_index)
{
    if (!_node)
        return NULL;

    node_link *link = _node->get_links()->value(link_index);

    return link;
}

node *node_view::get_node_from_position(QPoint position)
{
    // ya que el nodo esta compuesto por muchos hijos, al dar click puede ser un
    // hijo, y si es un hijo obtiene el nodo padre para poder extraer el nombre
    // del nodo.
    QGraphicsItem *item = scene->itemAt(mapToScene(position), QTransform());
    if (!item)
        return nullptr;

    QGraphicsItem *parent_item = item->parentItem();
    if (parent_item)
        item = parent_item;
    //
    //
    QString node_name = item->data(0).toString();

    return get_node(node_name);
}

void node_view::connect_node(QPoint position_node)
{
    // si un enlace input de un nodo esta siendo arrastrado para conectarlo a
    // otro nodo, 'link_connecting' no estara vacio y se determinara si se
    // conecta o no al nodo de destino.
    if (link_connecting->empty())
        return;

    QString node_name = link_connecting->value("name").toString();
    int link_index = link_connecting->value("index").toInt();

    node *from_node = get_node(node_name);
    node_link *link = get_node_link(from_node, link_index);

    node *to_node = get_node_from_position(position_node);
    if (!to_node)
        link->disconnect_node();
    else
    {
        // evita que se conecte asi mismo
        if (from_node != to_node)
            link->connect_node(to_node);
        else
            link->disconnect_node();
    }
    *link_connecting = {};
    //
    //
}

void node_view::select_nodes_by_area(QPointF selection_end_point)
{
    QPointF selection_start_point = mapToScene(click_position);

    QPainterPath rectangle;

    int start_x = selection_start_point.x();
    int start_y = selection_start_point.y();

    int end_x = selection_end_point.x();
    int end_y = selection_end_point.y();

    int width = end_x - start_x;
    int height = end_y - start_y;

    QRectF rect(start_x, start_y, width, height);
    rectangle.addRect(rect);

    // el 'QGraphicsRectItem' que se usa para el 'selection_box' da conflicto si
    // es negativo el ancho y alto, si es negativo lo invierte y se lo resta a
    // la position.
    if (width < 0)
    {
        width = -width;
        start_x -= width;
    }
    if (height < 0)
    {
        height = -height;
        start_y -= height;
    }
    QRectF rect_box(start_x, start_y, width, height);
    selection_box->setRect(rect_box);
    selection_box->setVisible(true);
    //

    if (!qt::shift())
        select_all(false);

    QList<QGraphicsItem *> selected_items = scene->items(rectangle);
    for (QGraphicsItem *item : selected_items)
    {
        node_backdrop *backdrop = dynamic_cast<node_backdrop *>(item);
        if (backdrop)
            if (!backdrop->is_under_selector(rect_box))
                continue;

        QString node_name = item->data(0).toString();
        select_node(node_name, true);
    }
}
