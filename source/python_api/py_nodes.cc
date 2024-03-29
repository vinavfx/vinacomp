#include <py_nodes.h>
#include <python_api.h>
#include <node_graph.h>

#ifdef GUI
    #include "../gui/node_graph/node.h"
    #include <node_graph.h>

    static node_graph *_node_graph;

    void py_nodes::init_module(QWidget *__node_graph)
    {
        _node_graph = static_cast<node_graph *>(__node_graph);
        init_methods();
    }
#elif ENGINE
    nodes *_nodes;

    void py_nodes::init_module(QJsonObject *project, nodes *__nodes)
    {
        _nodes = __nodes;
        init_methods();
    }
#endif

void py_nodes::init_methods()
{
    // el tamaño de la lista de metodos tiene que ser 1 mayor
    // a los metodos que existen
    static struct PyMethodDef methods[8];

    init_py_module("__py_nodes__", methods,
                   {{"create_node", py_nodes::create_node},
                    {"delete_node", py_nodes::delete_node},
                    {"set_position", py_nodes::set_position},
                    {"rename", py_nodes::rename},
                    {"get_node_graph_name", py_nodes::get_node_graph_name},
                    {"connect_input", py_nodes::connect_input},
                    {"node_exists", py_nodes::node_exists}});
}

PyObject *py_nodes::create_node(PyObject *self, PyObject *args)
{
    const char *node_id;

    if (!PyArg_ParseTuple(args, "s", &node_id))
        return 0;

    QString name;
    #ifdef GUI
        name = _node_graph->get_maker()->create_fx(node_id)->get_name();
    #elif ENGINE
        name = node_id;
        _nodes->create_node(node_id);
    #endif

    return py_string(name);
}

PyObject *py_nodes::delete_node(PyObject *self, PyObject *args)
{
    char *name;

    if (!PyArg_ParseTuple(args, "s", &name))
        return 0;

    print("delete: " + QString(name));

    // return py_bool(true);
    return py_string("string");
}

PyObject *py_nodes::set_position(PyObject *self, PyObject *args)
{
    int x;
    int y;
    const char *name;

    if (!PyArg_ParseTuple(args, "sii", &name, &x, &y))
        return 0;

    #ifdef GUI
        node *_node = _node_graph->get_node_view()->get_node(name);
        if (_node)
            _node->set_center_position(x, y);
    #endif

    return py_bool(true);
}

PyObject *py_nodes::rename(PyObject *self, PyObject *args)
{
    const char *name;
    const char *to_name;

    if (!PyArg_ParseTuple(args, "ss", &name, &to_name))
        return 0;

    #ifdef GUI
        node *_node = _node_graph->get_node_view()->get_node(name);
        if (_node)
        {
            _node_graph->get_node_view()->rename_node(_node, to_name);
            return py_bool(true);
        }
    #endif

    return py_bool(false);
}

PyObject *py_nodes::node_exists(PyObject *self, PyObject *args)
{
    const char *name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return 0;

    bool exist = false;
    #ifdef GUI
        exist = _node_graph->get_node_view()->get_node(name, true);
    #endif

    return py_bool(exist);
}

PyObject *py_nodes::get_node_graph_name(PyObject *self, PyObject *args)
{
    const char *node_name;
    if (!PyArg_ParseTuple(args, "s", &node_name))
        return 0;

    QString node_graph_name;

    #ifdef GUI
        node *_node = _node_graph->get_node_view()->get_node(node_name, true);
        if (_node)
        {
            node_graph *_node_graph =
                static_cast<node_graph *>(_node->get_node_graph());
            if (_node_graph)
                node_graph_name = _node_graph->get_group_name();
        }
    #endif

    return py_string(node_graph_name);
}

PyObject *py_nodes::connect_input(PyObject *self, PyObject *args)
{
    const char *src_node_name;
    const char *dst_node_name;
    int input_index;

    if (!PyArg_ParseTuple(args, "ssi", &src_node_name, &dst_node_name, &input_index))
        return 0;

    bool connected = false;

    #ifdef GUI
        node *src_node =
            _node_graph->get_node_view()->get_node(src_node_name, true);

        if (src_node)
        {
            input_wire *input = src_node->get_input(input_index);
            if (input)
            {
                node *dst_node =
                    _node_graph->get_node_view()->get_node(dst_node_name, true);

                if (dst_node)
                    connected = input->connect_node(dst_node);
            }
        }
    #endif

    return py_bool(connected);
}
