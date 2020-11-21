#include <maker.h>

maker::maker(node_graph *__node_graph, properties *__properties)
    : _node_graph(__node_graph),
      _properties(__properties)
{

    QString json_nodes_path = "source/engine/nodes/json";

    // Carga todos los efectos del directorio
    for (QString effect_path : os::listdir(json_nodes_path))
    {
        QJsonObject effect = jread(effect_path);
        QString effect_id = effect.value("id").toString();

        effects.insert(effect_id, effect);
    }
    //
    //

    finder = new node_finder(_node_graph, &effects);
    connect(finder->tree, &QTreeWidget::itemPressed, this, [this](QTreeWidgetItem *item) {
        QString node_id = item->text(1);
        create_fx(node_id);
        finder->hide();
    });

    setup_shortcut();
}

maker::~maker()
{
}

void maker::setup_shortcut()
{
    qt::shortcut("Tab", _node_graph, [this]() {
        finder->show_finder();
    });

    qt::shortcut("Escape", _node_graph, [this]() {
        finder->hide();
    });

    qt::shortcut("G", _node_graph, [this]() {
        create_fx("grade");
    });

    qt::shortcut("T", _node_graph, [this]() {
        create_fx("transform");
    });

    qt::shortcut("B", _node_graph, [this]() {
        create_fx("blur");
    });

    qt::shortcut("M", _node_graph, [this]() {
        create_fx("merge");
    });

    qt::shortcut("R", _node_graph, [this]() {
        create_fx("read");
    });

    qt::shortcut("W", _node_graph, [this]() {
        create_fx("write");
    });

    qt::shortcut("K", _node_graph, [this]() {
        create_fx("copy");
    });
}

void maker::create_fx(QString id)
{
    QJsonObject effect = this->get_effect(id);
    if (effect.empty())
        return;

    QString group = effect["group"].toString();
    QString label = effect["label"].toString();
    QString icon_name = effect["icon"].toString();
    QJsonArray knobs = effect["knobs"].toArray();
    QColor color = default_color(group);

    // Encuentra un nombre disponible
    QString name;
    int node_number = 1;
    while (true)
    {
        name = label + QString::number(node_number);
        if (!_node_graph->get_node(name))
            break;
        node_number++;
    }
    //
    //

    // Crear Knob
    trim_panel *panel = new trim_panel(
        _properties,
        name,
        icon_name,
        &knobs);
    _properties->add_trim_panel(panel);
    //
    //

    // Creación del nodo, con un número que no se ha utilizado.
    _node_graph->create_node(name, panel, icon_name, color);
    //
    //
}

QJsonObject maker::get_effect(QString id)
{
    return effects.value(id).toObject();
}

QJsonObject maker::get_effects(QString group)
{
    if (group.isEmpty())
        return effects;

    QJsonObject group_effects;
    for (QJsonValue value : effects)
    {
        QJsonObject effect = value.toObject();
        if (effect["group"].toString() == group)
            group_effects.insert(effect["id"].toString(), effect);
    }

    return group_effects;
}

QColor maker::default_color(QString effect_group)
{
    QMap<QString, QColor> colors;

    colors.insert("draw", QColor(76, 128, 51));
    colors.insert("time", QColor(175, 163, 93));
    colors.insert("channel", QColor(158, 59, 98));
    colors.insert("color", QColor(122, 168, 255));
    colors.insert("filter", QColor(204, 128, 77));
    colors.insert("keyer", QColor(0, 200, 0));
    colors.insert("merge", QColor(75, 93, 198));
    colors.insert("transform", QColor(165, 121, 170));

    if (colors.contains(effect_group))
        return colors.value(effect_group);
    else
        return QColor(150, 150, 150);
}