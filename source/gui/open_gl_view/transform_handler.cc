#include <gl_view.h>

void gl_view::tf_handler_draw()
{
    QColor color = {200, 200, 200};
    int size = 6;
    int smooth = false;

	int handler_ratio = 50;
	int arrow_size = 7;

	for (auto &handler : tf_handlers)
	{
		float angle = handler.rotate;
		QPointF translate_viewport = get_position(handler.translate);

		QPointF x1 = arc_point({0, 0}, handler_ratio, angle + 180);
		QPointF x2 = arc_point({0, 0}, handler_ratio, angle);
		QPointF y1 = arc_point({0, 0}, handler_ratio, angle - 90);
		QPointF y2 = arc_point({0, 0}, handler_ratio, angle + 90);

		handler.x_handler = {
			get_coordsf(x1 + translate_viewport),
			get_coordsf(x2 + translate_viewport)
		};

		handler.y_handler = {
			get_coordsf(y1 + translate_viewport),
			get_coordsf(y2 + translate_viewport)
		};

		QPointF rotate_point = arc_point({0, 0}, handler_ratio + handler_ratio, angle);

		handler.rotate_handler = {
			handler.x_handler.p2() ,
			get_coordsf(rotate_point + translate_viewport)
		};

		handler.scale_handler_ratio = 100 * ( get_scale().x() / width() );

		// pintar ejes
		draw_line(handler.x_handler.p1(), handler.x_handler.p2(), Qt::red);
		draw_line(handler.y_handler.p1(), handler.y_handler.p2(), Qt::green);

		// pintar flechas
		draw_triangle(handler.x_handler.p1(), arrow_size, Qt::red, true, angle + 90);
		draw_triangle(handler.x_handler.p2(), arrow_size, Qt::red, true, angle - 90);
		draw_triangle(handler.y_handler.p1(), arrow_size, Qt::green, true, angle + 180);
		draw_triangle(handler.y_handler.p2(), arrow_size, Qt::green, true, angle);

		// pintar rotador
		draw_line(handler.rotate_handler.p1(), handler.rotate_handler.p2(), color);

		draw_circle(handler.translate, handler.scale_handler_ratio);
		draw_point(handler.translate, Qt::white, 10, true);
	}
}

void gl_view::tf_handler_add(QString name)
{
	if (tf_handlers.contains(name))
		return;

	tf_handler_struct handler;
	handler.name = name;
	handler.translate = {0, 0};
	handler.rotate = 0;
	handler.transforming = false;

	tf_handlers.insert(name, handler);
}

void gl_view::tf_handler_translate_update(QString name, QPoint translate)
{
	tf_handler_add(name);
	tf_handler_struct &handler = tf_handlers[name];
	handler.translate = translate;
}

void gl_view::tf_handler_rotate_update(QString name, float rotate)
{
	tf_handler_add(name);
	tf_handler_struct &handler = tf_handlers[name];
	handler.rotate = rotate;
}

void gl_view::tf_handler_clear()
{
	tf_handlers.clear();
}

void gl_view::tf_handler_translate(QPoint cursor_position, tf_handler_struct &handler)
{
	QPointF cursor = get_coordsf(cursor_position);
	QPointF cursor_click = get_coordsf(click_position);

	float x_diff = handler_click.translate.x() - cursor_click.x();
	float y_diff = handler_click.translate.y() - cursor_click.y();

	float x = cursor.x() + x_diff;
	float y = cursor.y() + y_diff;

	handler.translate = { x, y };
}

void gl_view::tf_handler_translate_axis(QPoint cursor_position, tf_handler_struct &handler, bool x_axis)
{
	QPointF cursor = get_coordsf(cursor_position);
	QPointF cursor_click = get_coordsf(click_position);

	float rotate = handler.rotate;
	float click_rotate = handler_click.rotate;

	if (!x_axis)
	{
		rotate += 90;
		click_rotate += 90;
	}

	bool horizontal = false;

	if (click_rotate < 45 && click_rotate > -45)
		horizontal = true;
	if (click_rotate < -135 && click_rotate > -225)
		horizontal = true;
	if (click_rotate > 135 && click_rotate < 225)
		horizontal = true;

	float angle = (M_PI * 2.0) * rotate / 360.0;

	float x, y;
	if (horizontal)
	{
		float x_diff = handler_click.translate.x() - cursor_click.x();
		x = cursor.x() + x_diff;

		float x_distance = handler_click.translate.x() - x;
		float y_separation = ( x_distance / cosf(angle) ) * sinf(angle);
		y = handler_click.translate.y() + y_separation;
	}
	else
	{
		float y_diff = handler_click.translate.y() - cursor_click.y();
		y = cursor.y() + y_diff;

		float y_distance = handler_click.translate.y() - y;
		float x_separation = ( y_distance / sinf(angle) ) * cosf(angle);
		x = handler_click.translate.x() + x_separation;
	}

	handler.translate = {x, y};
}

void gl_view::tf_handler_rotate(QPoint cursor_position, tf_handler_struct &handler)
{
	QPointF translate = get_position(handler.translate);

    // calcular la rotacion a partir de 2 puntos
    double delta_y = (translate.y() - cursor_position.y());
    double delta_x = (cursor_position.x() - translate.x());

    float rotate = atan2(delta_x, delta_y) * 180 / M_PI;
	handler.rotate = rotate - 90;
}

QString gl_view::tf_get_action(QPoint cursor_position, tf_handler_struct &handler)
{
	QString action = "";

	if ( is_cursor_above(cursor_position, handler.translate) )
		action = "translate";

	else if ( cursor_above_line(cursor_position, handler.x_handler) )
		action = "translate_x";

	else if ( cursor_above_line(cursor_position, handler.y_handler) )
		action = "translate_y";

	else if ( cursor_above_line(cursor_position, handler.rotate_handler) )
		action = "rotate";

	return action;
}

void gl_view::tf_handler_press(QPoint cursor_position)
{
	for (auto &handler : tf_handlers)
	{
		QString action = tf_get_action(cursor_position, handler);
		if ( !action.isEmpty() )
		{
			handler.transforming = true;
			handler.action = action;
			handler_click = handler;
			break;
		}
	}
}

void gl_view::tf_handler_release(QPoint cursor_position)
{
	for (auto &handler : tf_handlers)
	{
		if (handler.transforming)
		{
			// tf_handler_translate(cursor_position, handler);
			tf_handler_changed(handler, true);
		}

		handler.transforming = false;
		handler.action = "";
	}
}

void gl_view::tf_handler_move(QPoint cursor_position)
{
	bool over = false;
	for (auto &handler : tf_handlers)
	{
		QString action = tf_get_action(cursor_position, handler);
		if (!over) over = !action.isEmpty();

		if ( action == "rotate" )
			this->setCursor(Qt::CrossCursor);
		else if ( action == "translate" )
			this->setCursor(Qt::SizeAllCursor);

		if (handler.transforming)
		{
			if ( handler.action == "rotate" )
				tf_handler_rotate(cursor_position, handler);
			else if ( handler.action == "translate_x" )
				tf_handler_translate_axis(cursor_position, handler, true);
			else if ( handler.action == "translate_y" )
				tf_handler_translate_axis(cursor_position, handler, false);
			else if ( handler.action == "translate" )
				tf_handler_translate(cursor_position, handler);

			tf_handler_changed(handler);
			update();
		}
	}

	if (!over)
		this->setCursor(Qt::ArrowCursor);
}

void gl_view::tf_handler_changed(tf_handler_struct handler, bool release){}
