#include "node_group.h"
#include <node_graph.h>
#include <vinacomp.h>

node_group::node_group(node_props _props,
                       QMap<QString, node *> *_selected_nodes,
                       QWidget *_node_graph)

    : node_rect(_props, _selected_nodes, _node_graph)
    , props(_props)
    , group_node_graph(nullptr)
{
}

node_group::~node_group()
{
    vinacomp *_vinacomp = static_cast<vinacomp *>(props.vinacomp);

    if (group_node_graph)
    {
        node_graph *_node_graph = static_cast<node_graph *>(group_node_graph);

        // borra primero los nodos que no son grupo, para que no tengan
        // problemas al desvincular los nodos, los grupos se borran despues en el
        // 'delete _node_graph'
        for (node *to_delete_node : get_nodes())
            if (to_delete_node->get_type() != "group")
                _node_graph->get_node_view()->delete_node(to_delete_node);

        _vinacomp->get_groups_node_graph()->remove(get_name());
        _vinacomp->get_panels_layout()->delete_node_graph_group(_node_graph);

        delete _node_graph;
    }
    else
    {
        for (node_struct child_node :
             props.project->get_children_nodes(get_name(), true))
        {
            node::unlink_all_invisible_node(props.vinacomp, child_node.name);
        }
    }
}

QMap<QString, node *> node_group::get_nodes() const
{
    if (!group_node_graph)
        return {};

    return *static_cast<node_graph *>(group_node_graph)
                ->get_node_view()
                ->get_nodes();
}

void node_group::set_name(QString new_name)
{
    vinacomp *_vinacomp = static_cast<vinacomp *>(props.vinacomp);

    // si el 'node graph' del grupo esta creado renombra todos lo nodos del
    // 'node_graph',  si no esta creado renombra todos los nodos que
    // pertenecen al gurpo en el proyecto
    if (group_node_graph)
    {
        for (node *_node : get_nodes())
            _node->set_group_name(new_name);

        node_graph *_node_graph = static_cast<node_graph *>(group_node_graph);

        _vinacomp->get_groups_node_graph()->remove(get_name());
        _vinacomp->get_groups_node_graph()->insert(new_name, _node_graph);

        _vinacomp->get_panels_layout()->rename_node_graph_group(get_name(),
                                                                new_name);
        _node_graph->set_group_name(new_name);
    }
    else
    {
        props.project->replace_parent_name_to_children(get_name(), new_name);
    }

    node_rect::set_name(new_name);
}

void node_group::open_group(bool add_to_panel)
{
    vinacomp *_vinacomp = static_cast<vinacomp *>(props.vinacomp);

    bool first_time = false;

    if (!group_node_graph)
    {
        // crea el node_graph de grupo solo cuando se necesite abrir el grupo,
        // para economizar memoria
        node_graph *_group_node_graph = new node_graph(
            props.vinacomp, props.project, props._properties, get_name(), this);

        // restaura los datos del proyecto, si es que los tiene
        _group_node_graph->restore_tree();

        // inserta el grupo a la lista global
        _vinacomp->get_groups_node_graph()->insert(get_name(),
                                                   _group_node_graph);

        group_node_graph = _group_node_graph;

        first_time = true;
    }

    node_graph *graph = static_cast<node_graph *>(group_node_graph);

    if (add_to_panel)
    {
        _vinacomp->get_panels_layout()->add_node_graph_group(graph, get_name());

        if (first_time)
            graph->get_node_view()->fit_view_to_nodes();
    }
}

void node_group::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (qt::control())
        open_group();
    node_rect::mouseDoubleClickEvent(event);
}
