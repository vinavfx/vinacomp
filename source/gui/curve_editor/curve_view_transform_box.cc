#include <curve_view.h>

QList<key_frame *> curve_view::get_selected_keys()
{
    QList<key_frame *> selected;

    for (curve *_curve : curves)
        for (key_frame *key : _curve->get_keys())
            if (key->selected())
                selected.push_back(key);

    return selected;
}

bool curve_view::is_point_in_rectangle(QPointF point, QLineF rectangle)
{
    // verifica si un punto esta dentro de un rectangulo,
    // el rectangulo esta representado por una linea en diagonal.
    float x = point.x();
    float y = point.y();

    QPointF p1 = rectangle.p1();
    QPointF p2 = rectangle.p2();

    bool iside_x = false;
    bool iside_y = false;

    if ((x > p1.x() and x < p2.x()) || (x < p1.x() and x > p2.x()))
        iside_x = true;
    if ((y > p1.y() and y < p2.y()) || (y < p1.y() and y > p2.y()))
        iside_y = true;

    return (iside_x && iside_y);
}

void curve_view::selector_move(QPoint cursor_position)
{
    if (!selecting)
        return;

    selector.setP2(get_coords(cursor_position));
    for (curve *_curve : curves)
    {
        for (key_frame *key : _curve->get_keys())
        {
            if (is_point_in_rectangle(key->pos(), selector))
                key->select(true);
            else
                key->select(false);
        }
    }

    update();
}

QLineF curve_view::get_rectangle_of_keyframes(QList<key_frame *> keys)
{
    if (keys.count() <= 1)
        return {};

    key_frame *top = keys.first();
    key_frame *bottom = keys.first();
    key_frame *left = keys.first();
    key_frame *right = keys.first();

    for (key_frame *key : keys)
    {
        if (key->y() > top->y())
            top = key;

        if (key->y() < bottom->y())
            bottom = key;

        if (key->x() > right->x())
            right = key;

        if (key->x() < left->x())
            left = key;
    }

    QPointF bottom_left = {left->x(), bottom->y()};
    QPointF top_right = {right->x(), top->y()};

    return {bottom_left, top_right};
}

QString curve_view::get_transform_action(QPoint cursor_position)
{
    QPointF bottom_left = transform_box.p1();
    QPointF top_right = transform_box.p2();
    QPointF bottom_right = {top_right.x(), bottom_left.y()};
    QPointF top_left = {bottom_left.x(), top_right.y()};

    // si las posiciones x o y de la caja de tranformacion son iguales, entoces solo se puede mover
    if (transform_box.x1() == transform_box.x2() || transform_box.y1() == transform_box.y2())
        if (cursor_above_rect(cursor_position, transform_box.p1(), transform_box.p2()))
            return "center_translate";
    //

    // vertices
    if (cursor_above_point(cursor_position, bottom_left))
        return "bottom_left_scale";
    if (cursor_above_point(cursor_position, top_right))
        return "top_right_scale";
    if (cursor_above_point(cursor_position, bottom_right))
        return "bottom_right_scale";
    if (cursor_above_point(cursor_position, top_left))
        return "top_left_scale";
    //
    //

    // Lineas del border
    QLineF left = {bottom_left, top_left};
    QLineF right = {bottom_right, top_right};
    QLineF bottom = {bottom_left, bottom_right};
    QLineF top = {top_left, top_right};

    if (cursor_above_rect(cursor_position, left.p1(), left.p2()))
        return "left_scale";
    if (cursor_above_rect(cursor_position, right.p1(), right.p2()))
        return "right_scale";
    if (cursor_above_rect(cursor_position, bottom.p1(), bottom.p2()))
        return "bottom_scale";
    if (cursor_above_rect(cursor_position, top.p1(), top.p2()))
        return "top_scale";
    //
    //

    // Lineas centrales
    int distance = 30;

    float center_x = (bottom_left.x() + bottom_right.x()) / 2;
    float center_y = (bottom_right.y() + top_right.y()) / 2;

    QPointF center = {center_x, center_y};
    center = get_position(center);

    QPointF horizontal_p1 = get_coordsf({center.x() - distance, center.y()});
    QPointF horizontal_p2 = get_coordsf({center.x() + distance, center.y()});
    bool horizontal = cursor_above_rect(cursor_position, horizontal_p1, horizontal_p2);

    QPointF vertical_p1 = get_coordsf({center.x(), center.y() - distance});
    QPointF vertical_p2 = get_coordsf({center.x(), center.y() + distance});
    bool vertical = cursor_above_rect(cursor_position, vertical_p1, vertical_p2);

    if (vertical && horizontal)
        return "center_translate";
    else if (vertical)
        return "vertical_translate";
    else if (horizontal)
        return "horizontal_translate";
    //
    //

    return "";
}

void curve_view::scale_key_from_point(QPointF point)
{
    float last_width = last_transform_box.x2() - last_transform_box.x1();
    float width = transform_box.x2() - transform_box.x1();

    float last_height = last_transform_box.y2() - last_transform_box.y1();
    float height = transform_box.y2() - transform_box.y1();

    float multiply_x = width / last_width;
    float multiply_y = height / last_height;

    for (key_frame *key : selected_key_frames)
    {
        float x = ((key->get_last_position().x() - point.x()) * multiply_x) + point.x();
        float y = ((key->get_last_position().y() - point.y()) * multiply_y) + point.y();

        key->set_pos({x, y});
    }
}

void curve_view::translate_keys(QPointF add_translate)
{
    for (key_frame *key : selected_key_frames)
        key->set_pos(key->get_last_position() + add_translate);
}

bool curve_view::key_overlap(key_frame *key)
{
    // verifica si un key frame seleccionado esta
    // sobre otro key frame no seleccionado en posicion 'x'
    key_frame *previous_key = get_previous_key(key);
    key_frame *next_key = get_next_key(key);

    if (next_key)
        if (key->x() > next_key->x())
            if (!next_key->selected())
                return true;

    if (previous_key)
        if (key->x() < previous_key->x())
            if (!previous_key->selected())
                return true;

    return false;
}

QLineF curve_view::get_limit_coord(QString orientation)
{
    // Respaldar posiciones
    for (key_frame *key : selected_key_frames)
        key->stamp_position();
    //
    //

    // determina de que punto se esta escalando los key frames, derecha o izquierda.
    float scale_point_x;
    if (orientation == "left")
        scale_point_x = transform_box.x2();
    else if (orientation == "right")
        scale_point_x = transform_box.x1();
    else
        return {};
    //
    //

    int samples = 1000;

    auto scale_keys = [=](float multiply) {
        // escala todos los key frames seleccionados y verifica si hay alguno sobrepuesto
        bool overlap = false;
        for (key_frame *key : selected_key_frames)
        {
            float x = ((key->get_last_position().x() - scale_point_x) * multiply) + scale_point_x;
            key->set_pos({x, key->y()});

            if (key_overlap(key))
                overlap = true;
        }
        return overlap;
    };

    auto last_key_position = [=](float multiply_section) {
        float multiply = 1;

        // encuentra la ultima posicion de cada key frames antes
        // que un key frames seleccionado se sobreponga
        // a otro, y retorna la posicion del ultimo key frame.
        for (int i = 0; i < samples; i++)
        {
            bool overlap = scale_keys(multiply);
            if (overlap)
            {
                multiply += multiply_section;
                scale_keys(multiply);
                break;
            }

            multiply -= multiply_section;
        }

        if (orientation == "left")
            return selected_key_frames.first()->x();
        else if (orientation == "right")
            return selected_key_frames.last()->x();
    };

    // Movimiento hacia la izquierda
    float subtract = 1.0 / samples;
    float left_coord = last_key_position(subtract);
    //
    //

    // Restauracion de posiciones
    for (key_frame *key : selected_key_frames)
        key->restore_position();
    //
    //

    // Movimiento hacia la derecha
    float added = 20.0 / samples;
    float right_coord = last_key_position(-added);
    //
    //

    // Restauracion de posiciones
    for (key_frame *key : selected_key_frames)
        key->restore_position();
    //
    //

    return {{left_coord, 0}, {right_coord, 0}};
}

QLineF curve_view::get_max_translate(QPoint cursor_position)
{
    float left_distance = 10000000;
    float right_distance = 10000000;
    for (key_frame *key : selected_key_frames)
    {
        key_frame *next_key = get_next_key(key);
        if (next_key)
            if (!next_key->selected())
            {
                float distance = next_key->x() - key->x();
                if (distance < right_distance)
                    right_distance = distance;
            }

        key_frame *previous_key = get_previous_key(key);
        if (previous_key)
            if (!previous_key->selected())
            {
                float distance = key->x() - previous_key->x();
                if (distance < left_distance)
                    left_distance = distance;
            }
    }

    return {{left_distance, 0}, {right_distance, 0}};
}

void curve_view::to_transform_box(QPoint cursor_position)
{
    // return;
    QString action = resize_current_action;

    QPointF click_coords = get_coordsf(click_position);
    QPointF coords = get_coordsf(cursor_position);
    QPointF add_translate = coords - click_coords;

    // Limitacion para que la escala desde la derecha, no traspase ningun keyframe no seleccionado
    if (action == "right_scale" || action == "top_right_scale" || action == "bottom_right_scale")
    {
        // Hacia la izquierda <---
        if (unselected_keys)
        {
            if (coords.x() < limit_coord_right.x1())
                coords.setX(limit_coord_right.x1());
        }
        else
        {
            if (coords.x() <= transform_box.x1())
                coords.setX(transform_box.x1());
        }

        // Hacia la derecha --->
        if (coords.x() > limit_coord_right.x2())
            coords.setX(limit_coord_right.x2());
    }

    // Limitacion para que la escala desde la izquierda, no traspase ningun keyframe no seleccionado
    if (action == "left_scale" || action == "top_left_scale" || action == "bottom_left_scale")
    {
        // Hacia la izquierda <---
        if (coords.x() < limit_coord_left.x2())
            coords.setX(limit_coord_left.x2());

        // Hacia la derecha --->
        if (unselected_keys)
        {
            if (coords.x() >= limit_coord_left.x1())
                coords.setX(limit_coord_left.x1());
        }
        else
        {
            if (coords.x() >= transform_box.x2())
                coords.setX(transform_box.x2());
        }
    }

    // Limitacion para que no traspase el borde superior
    if (action == "bottom_scale" || action == "bottom_left_scale" || action == "bottom_right_scale")
        if (coords.y() > transform_box.y2())
            coords.setY(transform_box.y2());

    // Limitacion para que no traspase el borde inferior
    if (action == "top_scale" || action == "top_right_scale" || action == "top_left_scale")
        if (coords.y() < transform_box.y1())
            coords.setY(transform_box.y1());
    //
    //

    // limita a la translacion para que no se superponga ningun keyframes
    if (action == "horizontal_translate" || action == "center_translate")
    {
        if (add_translate.x() < -max_translate.x1())
            add_translate.setX(-max_translate.x1());

        if (add_translate.x() > max_translate.x2())
            add_translate.setX(max_translate.x2());
    }
    //
    //

    if (action == "right_scale")
    {
        transform_box.setP2({coords.x(), transform_box.y2()});
        scale_key_from_point({transform_box.x1(), 0});
    }
    else if (action == "left_scale")
    {
        transform_box.setP1({coords.x(), transform_box.y1()});
        scale_key_from_point({transform_box.x2(), 0});
    }
    else if (action == "top_scale")
    {
        transform_box.setP2({transform_box.x2(), coords.y()});
        scale_key_from_point({0, transform_box.y1()});
    }
    else if (action == "bottom_scale")
    {
        transform_box.setP1({transform_box.x1(), coords.y()});
        scale_key_from_point({0, transform_box.y2()});
    }

    else if (action == "bottom_left_scale")
    {
        transform_box.setP1({coords.x(), transform_box.y1()});
        transform_box.setP1({transform_box.x1(), coords.y()});
        scale_key_from_point({transform_box.x2(), transform_box.y2()});
    }
    else if (action == "top_right_scale")
    {
        transform_box.setP2({coords.x(), transform_box.y2()});
        transform_box.setP2({transform_box.x2(), coords.y()});
        scale_key_from_point({transform_box.x1(), transform_box.y1()});
    }
    else if (action == "bottom_right_scale")
    {
        transform_box.setP1({transform_box.x1(), coords.y()});
        transform_box.setP2({coords.x(), transform_box.y2()});
        scale_key_from_point({transform_box.x1(), transform_box.y2()});
    }
    else if (action == "top_left_scale")
    {
        transform_box.setP1({coords.x(), transform_box.y1()});
        transform_box.setP2({transform_box.x2(), coords.y()});
        scale_key_from_point({transform_box.x2(), transform_box.y1()});
    }

    else if (action == "center_translate")
    {
        transform_box.setP1(last_transform_box.p1() + add_translate);
        transform_box.setP2(last_transform_box.p2() + add_translate);
        translate_keys(add_translate);
    }
    else if (action == "horizontal_translate")
    {
        QPointF p1 = {last_transform_box.x1() + add_translate.x(), last_transform_box.y1()};
        QPointF p2 = {last_transform_box.x2() + add_translate.x(), last_transform_box.y2()};

        transform_box.setP1(p1);
        transform_box.setP2(p2);

        translate_keys({add_translate.x(), 0});
    }
    else if (action == "vertical_translate")
    {
        QPointF p1 = {last_transform_box.x1(), last_transform_box.y1() + add_translate.y()};
        QPointF p2 = {last_transform_box.x2(), last_transform_box.y2() + add_translate.y()};

        transform_box.setP1(p1);
        transform_box.setP2(p2);

        translate_keys({0, add_translate.y()});
    }

    update();
}

void curve_view::show_transform_box()
{
    transform_box = get_rectangle_of_keyframes(get_selected_keys());
    transform_box_visible = !transform_box.isNull();

    update();
}

void curve_view::transform_box_press(QPoint cursor_position)
{
    if (!transform_box_visible)
        return;

    QString action = get_transform_action(cursor_position);

    if (!action.isEmpty())
    {
        resize_current_action = action;
        transforming = true;
        last_transform_box = transform_box;

        selected_key_frames = get_selected_keys();
        limit_coord_right = get_limit_coord("right");
        limit_coord_left = get_limit_coord("left");

        max_translate = get_max_translate(cursor_position);

        // verifica si en el rango de la seleccion, existe un keyframe deseleccionado
        int start = selected_key_frames.first()->get_index();
        int end = selected_key_frames.last()->get_index();
        int selected_range = end - start + 1;
        unselected_keys = selected_key_frames.count() != selected_range;
        //
        //
    }
}

void curve_view::transform_box_move(QPoint cursor_position)
{
    if (!transform_box_visible)
        return;

    QString action = get_transform_action(cursor_position);

    if (action == "bottom_left_scale" || action == "top_right_scale")
        this->setCursor(Qt::SizeBDiagCursor);
    else if (action == "bottom_right_scale" || action == "top_left_scale")
        this->setCursor(Qt::SizeFDiagCursor);
    else if (action == "left_scale" || action == "right_scale" || action == "horizontal_translate")
        this->setCursor(Qt::SizeHorCursor);
    else if (action == "top_scale" || action == "bottom_scale" || action == "vertical_translate")
        this->setCursor(Qt::SizeVerCursor);
    else if (action == "center_translate")
        this->setCursor(Qt::SizeAllCursor);

    if (transforming)
        to_transform_box(cursor_position);
}
