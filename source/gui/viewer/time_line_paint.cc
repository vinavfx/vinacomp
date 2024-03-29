#include <time_line.h>
#include <qt.h>

void time_line::initializeGL()
{
    initializeOpenGLFunctions();
    QColor color = palette["b5"].toString();
    glClearColor(color.red() / 255.0, color.green() / 255.0, color.blue() / 255.0, 1);
}

void time_line::paintGL()
{
    gl_view::paintGL();

    top_y = get_coords({0, 0}).y();
    number_y = get_coords({0, 32}).y();
    mid_y1 = get_coords({0, 7}).y();
    mid_y2 = get_coords({0, 42}).y();

    draw_coordinate_numbers();
    draw_guide_frames();
    draw_in_out();
    draw_cursor();
    draw_selector();
}

void time_line::draw_guide_frames()
{
    auto horizontal_numbers = [=](float separation) {
        QColor color = Qt::gray;
        for (float value :
             generate_coord_range(separation, Qt::Horizontal, color, {0.0, 10}, false, 700))
        {
            draw_line({value, 100000}, {value, mid_y1}, color);
            draw_line({value, -100000}, {value, mid_y2}, color);
        }
    };

    QList<float> x_separations = {1, 2, 10, 20, 100, 200, 1000, 10000, 100000};
    for (float separation : x_separations)
        horizontal_numbers(separation);
}

void time_line::draw_selector()
{
    if (!selector_visible)
        return;

    draw_box({{selector.first, -100000}, {selector.second, 100000}}, {50, 50, 50}, {100, 100, 100});
}

void time_line::draw_in_out()
{
    QColor red = {200, 0, 0};
    QColor range_color = palette["b12"].toString();
    QColor white = palette["b50"].toString();

    // linea del primer y ultimo frame
    draw_line({first_frame, 1000000}, {first_frame, -1000000}, white, 1);
    draw_line({last_frame, 1000000}, {last_frame, -1000000}, white, 1);
    //

    if (in_out_visible)
    {
        QColor outside_color = palette["b9"].toString();

        // Manejadores entrada y salida
        draw_line({input, 1000000}, {input, -1000000}, red, 2);
        draw_line({output, 1000000}, {output, -1000000}, red, 2);

        // Background del Rango
        draw_box({{first_frame, -100000}, {input, 100000}}, outside_color);
        draw_box({{output, -100000}, {last_frame, 100000}}, outside_color);
        draw_box({{input, -100000}, {output, 100000}}, range_color);

        // Texto del 'frame' de la entrada y salida
        float input_x = get_position({input, 0}).x();
        float output_x = get_position({output, 0}).x();
        draw_text(QString::number(input), red, {0, 0}, {input_x - 3, 7}, 10, Qt::AlignLeft);
        draw_text(QString::number(output), red, {0, 0}, {output_x + 5, 7}, 10, Qt::AlignRight);
    }
    else
    {
        draw_box({{first_frame, -100000}, {last_frame, 100000}}, range_color);
    }
}

void time_line::draw_cursor()
{
    float y2 = get_coords({0, 22}).y();

    if (!qt::control() && !qt::alt())
    {
        if (ghost_frame_visible && !right_button)
        {
            QColor ghost_frame_color = palette["ghost_base"].toString();
            draw_line({ghost_frame, top_y}, {ghost_frame, y2}, ghost_frame_color);
            draw_line({ghost_frame, -100000}, {ghost_frame, mid_y2}, ghost_frame_color);
            draw_triangle({ghost_frame, top_y}, 7, ghost_frame_color);
            draw_text(QString::number(ghost_frame), ghost_frame_color, {ghost_frame, number_y});
        }
    }

    QColor frame_color = palette["base"].toString();
    draw_line({frame, top_y}, {frame, y2}, frame_color);
    draw_line({frame, -100000}, {frame, mid_y2}, frame_color);
    draw_triangle({frame, top_y}, 7, frame_color);
    draw_text(QString::number(frame), frame_color, {frame, number_y});
}

void time_line::draw_coordinate_numbers()
{
    auto horizontal_numbers = [=](float separation) {
        QColor color = Qt::gray;
        for (float value : generate_coord_range(separation, Qt::Horizontal, color, {0.0, 10}))
            draw_text(QString::number(value), color, {value, 0}, {-1, 32});
    };

    QList<float> x_separations = {1, 10, 100, 1000, 10000, 100000};
    for (float separation : x_separations)
        horizontal_numbers(separation);
}
