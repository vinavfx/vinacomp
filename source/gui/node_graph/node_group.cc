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
        _vinacomp->get_groups_node_graph()->remove(get_name());
        delete group_node_graph;
    }
}

void node_group::open_group()
{
    vinacomp *_vinacomp = static_cast<vinacomp *>(props.vinacomp);

    // crea el node_graph de grupo solo cuando se necesite abrir el grupo, para
    // economizar memoria
    if (!group_node_graph)
    {
        node_graph *_group_node_graph = new node_graph(
            props.vinacomp, props.project, props._properties, get_name());

        // restaura los datos del proyecto, si es que los tiene
        _group_node_graph->restore_tree();

        // inserta el grupo a la lista global
        _vinacomp->get_groups_node_graph()->insert(get_name(),
                                                   _group_node_graph);

        group_node_graph = _group_node_graph;
    }

    node_graph *graph = static_cast<node_graph *>(group_node_graph);

    _vinacomp->get_panels_layout()->add_node_graph_group(graph, get_name());
}

void node_group::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    open_group();
    node_rect::mouseDoubleClickEvent(event);
}
