#include "node_backdrop.h"
#include "node_view.h"

node_backdrop::node_backdrop(node_props _props,
                             QMap<QString, node *> *_selected_nodes,
                             QWidget *__node_graph, QWidget *__node_view)

    : node(_props, _selected_nodes, __node_graph)
    , props(_props)
    , _node_view(__node_view)
    , selected_nodes(_selected_nodes)
    , title_area_height(50)
    , clicked_body_area(false)
    , resizing(false)
    , parent(nullptr)
    , base_z_value(-1000000000)
{
    this->setFlags(QGraphicsItem::ItemIsMovable);
    set_minimum_size(150, 150);

    // Name
    name_text = new QGraphicsTextItem;
    QFont name_font;
    name_font.setPixelSize(23);
    name_text->setFont(name_font);
    name_text->setParentItem(this);
    //
    //

    // Tips
    tips_text = new QGraphicsTextItem;
    QFont font_tips;
    font_tips.setPixelSize(14);
    tips_text->setFont(font_tips);
    tips_text->setParentItem(this);
    //
    //

    // Backdrop
    QColor color = get_random_color();
    if (props.from_project)
        color = props.color;

    QPen pen(Qt::black);
    pen.setWidth(0);
    this->setPen(pen);

    node::set_color(color);
    update_title_area_height(title_area_height);
    //
    //

    // esquina de reescalado
    corner = new QGraphicsLineItem;
    corner_size = {30, 30};
    corner->setPen(QPen(Qt::black, 0));
    corner->setLine(
        {{(float)corner_size.width(), 0}, {0, (float)corner_size.height()}});
    props.scene->addItem(corner);
    //

    if (props.from_project)
    {
        set_size(props.size.width(), props.size.height());
        set_z_value(props.z_value);
    }
    else
    {
        calculate_size();
        increase_z_value();
    }

    set_name(_props.name);
    set_tips(_props.tips);
}

node_backdrop::~node_backdrop()
{
    delete name_text;
    delete tips_text;
    delete corner;
}

void node_backdrop::update_title_area_height(int height)
{
    height *= 2;
    title_area_height = height;

    set_color(get_color());
}

void node_backdrop::update_text(QString _name, QString _tips)
{
    _name = _name.split('.').last();

    tips_text->setPlainText(_tips);
    bool has_tips = !_tips.isEmpty();

    int padding = 10;

    if (has_tips)
    {
        name_text->setPlainText(_name + ":");
        name_text->setX(padding);
    }
    else
    {
        name_text->setPlainText(_name);
        // si no tiene tips, centra el nombre
        name_text->setX((get_size().width() / 2) -
                        (name_text->boundingRect().width() / 2));
    }

    int name_width = name_text->boundingRect().width();
    int name_height = name_text->boundingRect().height();

    tips_text->setTextWidth(get_size().width() - name_width - (padding * 2));
    int tips_height = tips_text->boundingRect().height();

    // si el alto del tips es mas chico que el nombre, centra el tips
    int tips_x = name_width + padding;
    if (tips_height < name_height)
        tips_text->setPos(tips_x, ((name_height / 2) - (tips_height / 2)));
    else
        tips_text->setPos(tips_x, 0);
    //

    int title_area = name_height;
    if (tips_height > name_height)
        title_area = tips_text->y() + tips_height;

    int max_title_area_height = get_size().height() / 2;
    if (title_area > max_title_area_height)
    {
        title_area = name_height;
        tips_text->setPlainText("...");
    }

    update_title_area_height(title_area);
}

void node_backdrop::set_tips(QString _tips)
{
    update_text(get_name(), _tips);
    node::set_tips(_tips);
}

void node_backdrop::set_name(QString _name)
{
    update_text(_name, get_tips());
    node::set_name(_name);
}

void node_backdrop::set_z_value(int value)
{
    this->setZValue(value);
    corner->setZValue(value);
}

int node_backdrop::get_z_value() const
{
    return base_z_value + zValue();
}

void node_backdrop::increase_z_value()
{
    int value = base_z_value;

    (*props.current_z_value)++;
    this->setZValue(*props.current_z_value);

    value += *props.current_z_value;

    set_z_value(value);
}

bool node_backdrop::is_selected_nodes() const
{
    // encuentra algun nodo seleccionado que no se un backdrop
    bool _selected_nodes = false;
    for (node *_node : *selected_nodes)
        if (_node->get_type() != "backdrop")
            _selected_nodes = true;
    //

    return _selected_nodes;
}

bool node_backdrop::is_inside_backdrop(node_backdrop *backdrop)
{
    int x1 = x();
    int x2 = x1 + get_size().width();
    int y1 = y();
    int y2 = y1 + get_size().height();

    int _x1 = backdrop->x();
    int _x2 = _x1 + backdrop->get_size().width();
    int _y1 = backdrop->y();
    int _y2 = _y1 + backdrop->get_size().height();

    return x1 > _x1 && x2 < _x2 && y1 > _y1 && y2 < _y2;
}

void node_backdrop::set_size(int width, int height)
{
    if (width < minimum_width)
        width = minimum_width;
    if (height < minimum_height)
        height = minimum_height;

    node::set_size(width, height);

    int radius = 3;
    QPainterPath rectangle;
    rectangle.addRoundedRect(QRectF(0, 0, width, height), radius, radius);
    this->setPath(rectangle);

    refresh_corner();
}

void node_backdrop::calculate_size()
{
    // calcula el tamaño del backdrop si hay nodos seleccionados, lo encaja a
    // los nodos

    int width = 500;
    int height = 300;

    if (is_selected_nodes())
    {
        node_view *__node_view = static_cast<node_view *>(_node_view);
        QRectF bbox = __node_view->bbox_nodes(selected_nodes);
        int padding = 70;

        width = bbox.width() + padding * 2;
        height = bbox.height() + padding * 2;
        set_position(bbox.x() - padding, bbox.y() - padding);
    }

    set_size(width, height);
}

void node_backdrop::set_selected(bool enable)
{
    if (clicked_body_area && enable)
        return;

    if (enable)
        corner->setPen(QPen(Qt::white, 2));
    else
        corner->setPen(QPen(Qt::black, 0));

    node::set_selected(enable);
}

void node_backdrop::set_position(float x, float y)
{
    node::set_position(x, y);
    refresh_corner();
}

void node_backdrop::refresh()
{
    update_text(get_name(), get_tips());
}

void node_backdrop::refresh_corner()
{
    float cor_x = x() + get_size().width() - corner_size.width();
    float cor_y = y() + get_size().height() - corner_size.height();
    corner->setPos(cor_x, cor_y);
}

bool node_backdrop::is_under_selector(QRectF selector)
{
    // verifica si el backdrop esta dentro de un rectangulo, se usa para la
    // seleccion

    int x1 = this->x();
    int x2 = x1 + this->get_size().width();
    int y1 = this->y();
    int y2 = y1 + this->get_size().height();

    int sel_x1 = selector.x();
    int sel_x2 = sel_x1 + selector.width();
    int sel_y1 = selector.y();
    int sel_y2 = sel_y1 + selector.height();

    return sel_x1 < x1 && sel_x2 > x2 && sel_y1 < y1 && sel_y2 > y2;
}

QColor node_backdrop::get_random_color() const
{
    float gain = 0.55;
    int saturation = 80; // 0 - 255

    int red = std::rand() % saturation + (255 - saturation);
    int green = std::rand() % saturation + (255 - saturation);
    int blue = std::rand() % saturation + (255 - saturation);

    red *= gain;
    green *= gain;
    blue *= gain;

    return QColor(red, green, blue);
}

void node_backdrop::set_color(QColor color1)
{
    node::set_color(color1);
    QColor color2 = color1;

    // baja la intensidad al color 2
    int h, s, l;
    color2.getHsl(&h, &s, &l);
    color2.setHsl(h, s, l * 0.9);
    //

    QLinearGradient ramp(0, title_area_height, 0, 0);
    ramp.setColorAt(0.5000, color);
    ramp.setColorAt(0.5001, color2);

    QBrush brush(ramp);
    this->setBrush(brush);
}

void node_backdrop::resize(QSize size)
{
    if (!resizing)
        return;

    set_size(size.width(), size.height());
    refresh();
}

void node_backdrop::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // detecta si el click se hizo en la esquina de reescalado
    int width = get_size().width();
    int height = get_size().height();

    int width_less = width - corner_size.width();
    int height_less = height - corner_size.height();

    if (event->pos().x() > width_less && event->pos().y() > height_less)
    {
        corner_click_diff.setWidth(width - event->pos().x());
        corner_click_diff.setHeight(height - event->pos().y());
        resizing = true;
    }
    //

    clicked_body_area = event->pos().y() > (title_area_height / 2);
    if (clicked_body_area)
        return;

    increase_z_value();

    // encuentra todos los nodos que estan dentro del 'backdrop' para
    // luego arrastrarlos junto con el 'backdrop'
    QPainterPath rectangle;

    QRectF rect(this->x(), this->y(), current_width, current_height);
    rectangle.addRect(rect);

    node_view *__node_view = static_cast<node_view *>(_node_view);
    nodes_to_drag.clear();

    QList<QGraphicsItem *> selected_items = props.scene->items(rectangle);

    // crea una lista solo con los item que son nodos
    QList<node*> nodes;
    for (QGraphicsItem *item : selected_items)
    {
        QString node_name = item->data(0).toString();
        node *_node = __node_view->get_node(node_name);
        if (_node)
            nodes.push_back(_node);
    }
    //

    // agrega solo los nodos que no son backdrop a la lista de 'nodes_to_drag'
    for (node *_node : nodes)
    {
        if (!dynamic_cast<node_backdrop *>(_node))
        {
            _node->freeze_position();
            nodes_to_drag.push_back(_node);
        }
    }
    //

    // crea un a lista con los backdrop que estan dentro de este
    QList<node_backdrop *> backdrops;
    for (node *_node : nodes)
    {
        node_backdrop *backdrop = dynamic_cast<node_backdrop *>(_node);
        if (backdrop)
            if (backdrop->is_inside_backdrop(this))
                backdrops.push_back(backdrop);
    }
    //

    // ordena los backdrop segun profundidad
    std::sort(backdrops.begin(), backdrops.end(),
         [](node_backdrop *a, node_backdrop *b) {
             return a->zValue() < b->zValue();
         });
    //

    // agrega los backdrops ordenados a la lista de 'nodes_to_drag' e incremeta
    // la profundidad
    for (node_backdrop *backdrop : backdrops)
    {
        backdrop->increase_z_value();
        backdrop->freeze_position();
        nodes_to_drag.push_back(backdrop);
    }
    //

    node::mousePressEvent(event);
}

void node_backdrop::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    clicked_body_area = false;
    resizing = false;
    node::mouseReleaseEvent(event);
}

void node_backdrop::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // re escalado
    resize({(int)event->pos().x() + corner_click_diff.width(),
            (int)event->pos().y() + corner_click_diff.height()});
    //

    if (clicked_body_area)
        return;

    // calcula el movimiento del backdrop, restandolo con la posicion,
    // para que el movimiento inicie en 0, y asi sumarlo a los nodos
    QPointF position = mapToScene(event->pos());
    QPointF click_position_backdrop =
        node::click_position - node::get_freeze_position();

    QPointF new_position_backdrop = position - click_position_backdrop;
    QPointF add_position = new_position_backdrop - node::get_freeze_position();
    //

    node::mouseMoveEvent(event);

    // si el control esta presionado no arrastra los nodos
    if (qt::control())
        return;

    for (node *_node : nodes_to_drag)
    {
        QPointF pos = _node->get_freeze_position() + add_position;
        _node->set_position(pos.x(), pos.y());
    }
}
