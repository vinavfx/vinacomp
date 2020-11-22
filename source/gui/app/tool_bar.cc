#include <tool_bar.h>

tool_bar::tool_bar(maker *__maker)
    : _maker(__maker)
{
    this->setMouseTracking(true);
    this->setTabletTracking(true);

    this->setObjectName("tool_bar");
    this->setMaximumWidth(40);

    layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 10, 2, 10);

    setup_ui();
}

tool_bar::~tool_bar()
{
}

void tool_bar::setup_ui()
{
    int icon_size = 25;

    add_menu("image", "image_a");
    add_menu("draw", "brush_a");
    add_menu("time", "time_a");
    add_menu("channel", "channel_a");
    add_menu("color", "color_a");
    add_menu("filter", "filter_a");
    add_menu("keyer", "keyer_a");
    add_menu("merge", "merge_a");
    add_menu("transform", "transform_a");
    add_menu("other", "other_a");

    layout->addStretch();

    QPushButton *script_layout = new QPushButton();
    qt::set_icon(script_layout, "view_compact_a", icon_size);
    layout->addWidget(script_layout);

    QPushButton *comp_layout = new QPushButton();
    qt::set_icon(comp_layout, "vertical_split_a", icon_size);
    layout->addWidget(comp_layout);
}

void tool_bar::add_menu(QString group, QString icon_group)
{
    int icon_size = 25;

    QPushButton *popup_button = new QPushButton(this);

    menu *_menu = new menu(this);

    connect(popup_button, &QPushButton::clicked, this, [=] {
        _menu->exec(popup_button->mapToGlobal({this->width(), 0}));
    });

    for (QJsonValue value : _maker->get_effects(group))
    {
        QJsonObject effect = value.toObject();
        QString label = effect["label"].toString();
        QString id = effect["id"].toString();
        QString icon = effect["icon"].toString();

        QAction *effect_action = new QAction(label);
        effect_action->setIcon(QIcon("resources/images/" + icon + ".png"));
        _menu->addAction(effect_action);

        connect(effect_action, &QAction::triggered, this, [=]() {
            _maker->create_fx(id);
        });
    }

    qt::set_icon(popup_button, icon_group, icon_size);

    layout->addWidget(popup_button);
}

void menu::mousePressEvent(QMouseEvent *event)
{
    QMenu::mousePressEvent(event);

    // ya que el QMenu no permite clickear otro widget cuando esta abierto,
    // por eso buscamos la posicion del click global para encuentra el widget en esa posicion
    // y si existe un boton le da click.
    QPoint global_position = this->mapToGlobal(event->pos());

    QWidget *widget = qApp->widgetAt(global_position);
    QPushButton *button = dynamic_cast<QPushButton *>(widget);
    if (button)
        button->click();
}