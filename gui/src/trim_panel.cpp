#include <trim_panel.hpp>

trim_panel::trim_panel(/* args */)
{
    this->setObjectName("trim_panel");
    setup_ui();
}

trim_panel::~trim_panel()
{
}

void trim_panel::setup_ui()
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    this->setLayout(layout);

    QWidget *butttons = top_buttons_setup_ui();

    layout->addWidget(butttons);

    tabs = tabs_ui();
    layout->addWidget(tabs);
    //
    //
}

QWidget *trim_panel::top_buttons_setup_ui()
{
    QWidget *widget = new QWidget();
    widget->setObjectName("butttons");
    widget->setMaximumHeight(30);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(5);

    widget->setLayout(layout);
    //
    //

    int icon_size = 20;

    QPushButton *settings = new QPushButton();
    qt::set_icon(settings, "settings_a", icon_size);
    layout->addWidget(settings);

    QPushButton *center_node = new QPushButton();
    qt::set_icon(center_node, "center_a", icon_size);
    layout->addWidget(center_node);

    layout->addStretch();

    QPushButton *icon_node = new QPushButton();
    qt::set_icon(icon_node, "transform_a", icon_size);
    layout->addWidget(icon_node);

    QLineEdit *node_name = new QLineEdit();
    layout->addWidget(node_name);

    layout->addStretch();

    // Minimize
    QPushButton *minimize = new QPushButton();
    connect(minimize, &QPushButton::clicked, this, [this]() {
        is_minimize = !is_minimize;
        tabs->setVisible(!is_minimize);
    });
    qt::set_icon(minimize, "minimize_a", icon_size);
    layout->addWidget(minimize);
    // Minimize
    //

    QPushButton *maximize = new QPushButton();
    qt::set_icon(maximize, "maximize_a", icon_size);
    layout->addWidget(maximize);

    QPushButton *close = new QPushButton();
    qt::set_icon(close, "close_a", icon_size);
    layout->addWidget(close);

    return widget;
}

QTabWidget *trim_panel::tabs_ui()
{

    QTabWidget *tabs = new QTabWidget();

    QWidget *controls = new QWidget();
    QVBoxLayout *controls_layout = new QVBoxLayout();
    controls->setLayout(controls_layout);
    controls->setObjectName("controls");

    QWidget *node = new QWidget();

    tabs->addTab(controls, "Controls");
    tabs->addTab(node, "Node");

    //
    knob_color *_knob_color = new knob_color();
    controls_layout->addWidget(_knob_color);

    knob_check_box *_knob_check_box = new knob_check_box();
    controls_layout->addWidget(_knob_check_box);

    knob_file *_knob_file = new knob_file();
    controls_layout->addWidget(_knob_file);

    controls_layout->addStretch();

    return tabs;
}