#include <general_settings.h>
#include <nodes_load.h>
#include <os.h>
#include <path_utils.h>
#include <vinacomp.h>

void general_settings::setup_plugins()
{
    QVBoxLayout *layout = add_item("Plugins");

    plugin_tree = new QTreeWidget;
    plugin_tree->setObjectName("plugin_tree");
    plugin_tree->setAlternatingRowColors(true);
    QStringList columns{"Plugin Name", "Label", "Group"};
    plugin_tree->setHeaderLabels(columns);
    plugin_tree->setColumnWidth(0, 100);
    plugin_tree->setColumnWidth(1, 100);
    plugin_tree->setColumnWidth(2, 100);

    QWidget *buttons = new QWidget;
    buttons->setObjectName("plugin_buttons");
    QHBoxLayout *buttons_layout = new QHBoxLayout(buttons);
    buttons_layout->setMargin(0);

    button *remove_button = new button();
    button *edit_button = new button();

    connect(remove_button, &button::clicked, this,
            &general_settings::delete_plugin);

    remove_button->set_icon("delete");
    edit_button->set_icon("edit");

    buttons_layout->addWidget(edit_button);
    buttons_layout->addWidget(remove_button);
    buttons_layout->addStretch();

    layout->addWidget(plugin_tree);
    layout->addWidget(buttons);

    load_plugins();
}

void general_settings::load_plugins()
{
    QString base_path = "plugins/py_plugins/";
    for (QString plugin_path : os::listdir(base_path))
    {
        if (path_util::get_ext(plugin_path) != "json")
            continue;

        QJsonObject plugin = jread(plugin_path);

        QString name = plugin.value("id").toString();
        QString icon = plugin.value("icon").toString();
        QString label = plugin.value("label").toString();
        QString group = plugin.value("group").toString();


        QTreeWidgetItem *item = new QTreeWidgetItem;

        item->setText(0, name);
        item->setText(1, label);
        item->setText(2, group);

        QString default_icon = "resources/images/default_icon_normal.png";

        if (!os::isfile(icon))
            icon = default_icon;

        item->setIcon(0, QIcon(icon));

        QString icon_group = base_path + "/" + group + ".png";

        if (!os::isfile(icon_group))
            icon_group = default_icon;

        item->setIcon(2, QIcon(icon_group));

        plugin_tree->addTopLevelItem(item);
        plugin_items.push_back(item);
    }
}

void general_settings::update_plugin_tree()
{
    for (auto *item : plugin_items)
        delete item;

    plugin_items.clear();
    plugin_tree->clear();

    load_plugins();
}

void general_settings::delete_plugin()
{
    if (plugin_tree->selectedItems().isEmpty())
        return;

    auto *item = plugin_tree->selectedItems().first();

    QString plugin_name = item->text(0);
    QString base_plugin_path = "plugins/py_plugins/" + plugin_name;

    os::remove(base_plugin_path + ".json");
    os::remove(base_plugin_path + ".py");
    os::remove(base_plugin_path + ".png");

    nodes_load *nodes_loaded = static_cast<vinacomp *>(_vinacomp)
                                   ->get_node_graph()
                                   ->get_nodes_loaded();

    update_plugin_tree();
    nodes_loaded->update_py_plugins();
}