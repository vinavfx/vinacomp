#include <curve_view.hpp>

QLineF curve_view::get_handler_points(
    key_frame *key,
    bool infinite)
{

    key_frame *previous_key = get_previous_key(key);
    key_frame *next_key = get_next_key(key);

    float left_separation, right_separation;

    if (previous_key)
        left_separation = (key->x() - previous_key->x()) * key->exaggeration();
    if (next_key)
        right_separation = (next_key->x() - key->x()) * key->exaggeration();

    // si es el primer o ultimo keyframe, le asigna la separacion del lado contrario
    if (!previous_key)
        left_separation = right_separation;
    if (!next_key)
        right_separation = left_separation;
    //
    //

    float left_point_x = key->x() - left_separation;
    float right_point_x = key->x() + right_separation;

    QPointF left_point = {left_point_x, key->y()};
    QPointF right_point = {right_point_x, key->y()};

    // genera el punto vertical infinito donde apunta el manejador
    float left_tangent = tan(key->get_left_angle() * M_PI / 180);
    float right_tangent = tan(-key->get_right_angle() * M_PI / 180);
    QPointF infinite_left_point = {left_point.x(), left_point.y() + (left_tangent * left_separation)};
    QPointF infinite_right_point = {right_point.x(), right_point.y() - (right_tangent * right_separation)};
    //

    if (!infinite)
    {
        // antes de rotar el manejador, transforma el punto de key y los
        // 2 puntos del manejador de cordenadas a puntos en la
        // position del visor, con esto logramos que el manejador
        // siempre quede del mismo tamaño.
        QPointF viwport_left_point = get_position(left_point);
        QPointF viwport_right_point = get_position(right_point);

        QPointF view_infinite_left = get_position(infinite_left_point);
        QPointF view_infinite_right = get_position(infinite_right_point);

        QPointF view_anchor_point = get_position(key->pos());

        float left_angle = get_angle_two_points(view_infinite_left, view_anchor_point) - 90;
        float right_angle = get_angle_two_points(view_infinite_right, view_anchor_point) + 90;

        viwport_left_point = rotate_point(viwport_left_point, view_anchor_point, left_angle - 180);
        viwport_right_point = rotate_point(viwport_right_point, view_anchor_point, right_angle);

        left_point = get_coordsf(viwport_left_point);
        right_point = get_coordsf(viwport_right_point);
    }

    if (infinite)
        return {infinite_left_point, infinite_right_point};
    else
        return {left_point, right_point};
}

QPointF curve_view::cubic_bezier(
    QPointF p1, QPointF p2,
    QPointF p3, QPointF p4,
    float value)
{
    // Algoritmo bezier
    QPointF L1 = ((1 - value) * p1) + (value * p2);
    QPointF L2 = ((1 - value) * p2) + (value * p3);
    QPointF L3 = ((1 - value) * p3) + (value * p4);

    QPointF Q1 = ((1 - value) * L1) + (L2 * value);
    QPointF Q2 = ((1 - value) * L2) + (L3 * value);

    return ((1 - value) * Q1) + (Q2 * value);
    //
}

void curve_view::create_curve()
{
    QString name = "translate_x";

    key_frame *key1 = new key_frame(name, 0);
    key_frame *key2 = new key_frame(name, 1);
    key_frame *key3 = new key_frame(name, 2);
    key_frame *key4 = new key_frame(name, 3);
    key_frame *key5 = new key_frame(name, 4);
    key_frame *key6 = new key_frame(name, 5);

    key1->set_pos({0, 2});
    key2->set_pos({0.5, 1});
    key3->set_pos({1, 0});
    key4->set_pos({2, 0.4});
    key5->set_pos({2.5, 1});
    key6->set_pos({3, 0.2});

    curves.insert(name, {key1, key2, key3, key4, key5, key6});

    update();
}

key_frame *curve_view::get_previous_key(key_frame *key)
{
    QString curve_name = key->get_curve();
    if (!curves.contains(curve_name))
        return NULL;

    auto keys = curves.value(curve_name);
    return keys.value(key->get_index() - 1);
}

key_frame *curve_view::get_next_key(key_frame *key)
{
    QString curve_name = key->get_curve();
    if (!curves.contains(curve_name))
        return NULL;

    auto keys = curves.value(curve_name);
    return keys.value(key->get_index() + 1);
}

void curve_view::set_interpolation_to_selected(int number)
{
    // 0: Linear
    // 1: Horizontal
    // 2: Smooth
    // 3: Break
    // 4: Custom

    for (key_frame *key : get_selected_keys())
    {
        if (number == 1)
        {
            key->set_left_angle(0);
            key->set_right_angle(0);

            key->set_break(false);
        }
        else if (number == 3)
        {
            key->set_break(true);
        }

        // key->set_interpolation(number);
    }

    update();
}

void curve_view::key_press(QPoint cursor_position)
{
    // si el click del mouse fue presionado en algun keyframe o en
    // alguno de los 2 puntos del manejador, los asigna a las variables de 'drag'

    for (auto curve_name : curves.keys())
    {
        auto keys = curves[curve_name];
        for (key_frame *key : keys)
        {
            QLineF handler = get_handler_points(key);

            // verifica si el click se hizo en el key point
            // o se hizo en alguno de los 2 puntos manejadores.
            int handler_point = 0;

            if (is_cursor_above(cursor_position, key->pos()))
                key->select(true);
            else if (is_cursor_above(cursor_position, handler.p1()))
                handler_point = 1;
            else if (is_cursor_above(cursor_position, handler.p2()))
                handler_point = 2;
            else
                key->select(false);
            //
            //

            if (key->selected())
            {
                key->stamp_position();
                drag_key_frame = key;
                drag_handler = handler_point;
                dragging = true;
            }
        }
    }
}

void curve_view::key_move(QPoint cursor_position)
{

    // cambia el cursor si esta sobre algun key frame
    if (!transform_box_visible)
        for (auto keys : curves)
            for (key_frame *key : keys)
                if (is_cursor_above(cursor_position, key->pos()))
                    this->setCursor(Qt::CrossCursor);
    //
    //

    if (dragging)
    {
        key_frame *key = drag_key_frame;

        if (drag_handler == 1)
        {
            float angle = 90 - get_angle_two_points(get_coords(cursor_position), key->pos());

            // Limitacion de handler 1 a 90 grados
            if (angle >= 90 and angle < 180)
                angle = 89.9;
            else if (angle > 180)
                angle = -89.9;
            //
            //

            key->set_left_angle(angle);

            if (!key->is_break())
                key->set_right_angle(-angle);
        }
        else if (drag_handler == 2)
        {
            float angle = 270 - get_angle_two_points(get_coords(cursor_position), key->pos());

            // Limitacion de handler 2 a 90 grados
            if (angle <= 270 and angle > 180)
                angle = 271;
            else if (angle < 180)
                angle = 449;
            //
            //

            key->set_right_angle(-angle);
            if (!key->is_break())
                key->set_left_angle(angle);
        }
        else
        {
            QPointF coords = get_coords(cursor_position) - (get_coords(click_position) - key->get_last_position());

            // limitar posicion al key_frame anterior y siguiente
            key_frame *previous_key = get_previous_key(key);
            if (previous_key)
            {
                if (coords.x() < previous_key->x())
                    coords.setX(previous_key->x());
            }

            key_frame *next_key = get_next_key(key);
            if (next_key)
            {
                if (coords.x() > next_key->x())
                    coords.setX(next_key->x());
            }
            //
            //

            key->set_pos(coords);
        }

        update();
    }
}
