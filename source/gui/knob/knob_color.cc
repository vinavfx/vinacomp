#include <knob_color.h>

knob_color::knob_color(knob_props props, float min, float max, float r, float g,
                       float b, float a, bool centered_handler)
    : knob(props)
    , red(r)
    , green(g)
    , blue(b)
    , alpha(a)
    , default_red(r)
    , default_green(g)
    , default_blue(b)
    , default_alpha(a)

    , mono_color(false)
    , sliders_colors(false)
{
    this->setObjectName("knob_color");

    layout = new QHBoxLayout(this);
    this->set_knob_layout(layout);

    separate_colors_box = new QWidget(this);
    separate_colors_slider = new QWidget(this);
    separate_colors_box_layout = new QHBoxLayout(separate_colors_box);
    separate_colors_slider_layout = new QVBoxLayout(separate_colors_slider);

    mono_edit = new QLineEdit(this);
    mono_slider = new slider(min, max, default_red, true, centered_handler);

    red_widget = new QWidget(this);
    green_widget = new QWidget(this);
    blue_widget = new QWidget(this);
    alpha_widget = new QWidget(this);
    red_layout = new QHBoxLayout(red_widget);
    green_layout = new QHBoxLayout(green_widget);
    blue_layout = new QHBoxLayout(blue_widget);
    alpha_layout = new QHBoxLayout(alpha_widget);

    red_hedit = new QLineEdit(this);
    green_hedit = new QLineEdit(this);
    blue_hedit = new QLineEdit(this);
    alpha_hedit = new QLineEdit(this);

    red_vedit = new QLineEdit(this);
    green_vedit = new QLineEdit(this);
    blue_vedit = new QLineEdit(this);
    alpha_vedit = new QLineEdit(this);

    red_slider = new slider(min, max, default_red, true, centered_handler);
    green_slider = new slider(min, max, default_green, true, centered_handler);
    blue_slider = new slider(min, max, default_blue, true, centered_handler);
    alpha_slider = new slider(min, max, default_alpha, true, centered_handler);

    picker_button = new QPushButton(this);
    palette_button = new QPushButton(this);
    mono_color_button = new QPushButton(this);

    connections();
    setup_ui();
    init_colors();
}

knob_color::~knob_color()
{
    delete mono_edit;
    delete mono_slider;

    delete red_vedit;
    delete green_vedit;
    delete blue_vedit;
    delete alpha_vedit;

    delete red_hedit;
    delete green_hedit;
    delete blue_hedit;
    delete alpha_hedit;

    delete red_slider;
    delete green_slider;
    delete blue_slider;
    delete alpha_slider;

    delete red_layout;
    delete green_layout;
    delete blue_layout;
    delete alpha_layout;
    delete red_widget;
    delete green_widget;
    delete blue_widget;
    delete alpha_widget;

    delete picker_button;
    delete palette_button;
    delete mono_color_button;

    delete separate_colors_box_layout;
    delete separate_colors_slider_layout;
    delete separate_colors_box;
    delete separate_colors_slider;

    delete layout;
}

void knob_color::init_colors()
{
    // si los colores son todos iguale activa el 'knob' mono cromatico
    if (red == green && red == blue && red == alpha)
    {
        mono_color = false;
        set_visible_mono_color(true, false);
    }
    else
    {
        mono_color = true;
        set_visible_mono_color(false, false);
    }
    //
    //
    set_color(red, green, blue, alpha, true, false);
}

void knob_color::restore_default()
{
    knob::restore_default();
    set_color(default_red, default_green, default_blue, default_alpha, true,
              false);
}

void knob_color::restore_param()
{
    QJsonArray param_value = get_param_value().toArray();
    QJsonArray _default = get_default().toArray();;

    default_red = _default.at(0).toDouble();
    default_green = _default.at(1).toDouble();
    default_blue = _default.at(2).toDouble();
    default_alpha = _default.at(3).toDouble();

    red = param_value.at(0).toDouble();
    green = param_value.at(1).toDouble();
    blue = param_value.at(2).toDouble();
    alpha = param_value.at(3).toDouble();

    mono_slider->set_default_value(default_red);

    red_slider->set_default_value(default_red);
    green_slider->set_default_value(default_green);
    blue_slider->set_default_value(default_blue);
    alpha_slider->set_default_value(default_alpha);

    init_colors();
}

void knob_color::update()
{
    set_color(red, green, blue, alpha);
}

void knob_color::set_visible_mono_color(bool visible, bool emmit_signal)
{
    if (sliders_colors)
    {
        visible = false;
        separate_colors_box->setVisible(false);
    }
    else
    {
        separate_colors_box->setVisible(!visible);
    }

    mono_slider->setVisible(visible);
    mono_edit->setVisible(visible);

    if (visible)
    {
        // promedia todos los colores para el mono color
        float color = (red + green + blue) / 3.0;

        mono_edit->setText(QString::number(color));
        mono_slider->set_value(color);

        set_color(color, color, color, color, true, emmit_signal);
    }

    qt::set_property(mono_color_button, "active", !visible);
    if (emmit_signal)
        update();
}

void knob_color::toggle_mono_color()
{
    mono_color = !mono_color;
    set_visible_mono_color(!mono_color);
}

void knob_color::set_visible_sliders_colors(bool visible)
{
    separate_colors_slider->setVisible(visible);

    // al activar el cuadro se colors sliders se desabilita el mono color
    mono_color = !visible;
    set_visible_mono_color(false);
    //

    qt::set_property(palette_button, "active", visible);
    qt::set_property(mono_color_button, "disable", visible);
    update();
}

void knob_color::toggle_sliders_colors()
{
    sliders_colors = !sliders_colors;
    set_visible_sliders_colors(sliders_colors);
}

void knob_color::set_color(float _red, float _green, float _blue, float _alpha,
                           bool set_sliders, bool emmit_signal)
{
    red = _red;
    green = _green;
    blue = _blue;
    alpha = _alpha;

    if (red == green && red == blue && red == alpha)
    {
        mono_edit->setText(QString::number(red));
        if (set_sliders)
            mono_slider->set_value(red);
    }

    red_vedit->setText(QString::number(red));
    red_hedit->setText(QString::number(red));

    green_vedit->setText(QString::number(green));
    green_hedit->setText(QString::number(green));

    blue_vedit->setText(QString::number(blue));
    blue_hedit->setText(QString::number(blue));

    alpha_vedit->setText(QString::number(alpha));
    alpha_hedit->setText(QString::number(alpha));

    if (set_sliders)
    {
        red_slider->set_value(red);
        green_slider->set_value(green);
        blue_slider->set_value(blue);
        alpha_slider->set_value(alpha);
    }

    if (emmit_signal)
    {
        changed(red, green, blue, alpha); // Signal
        update_value(QJsonArray{red, green, blue, alpha});
    }

    if (_red < 0)
        _red = 0;
    if (_red > 1)
        _red = 1;

    if (_green < 0)
        _green = 0;
    if (_green > 1)
        _green = 1;

    if (_blue < 0)
        _blue = 0;
    if (_blue > 1)
        _blue = 1;

    // Cambia el color del boton de picker
    QString __red = QString::number(_red * 255);
    QString __green = QString::number(_green * 255);
    QString __blue = QString::number(_blue * 255);

    picker_button->setStyleSheet("background: rgb(" + __red + "," + __green + "," + __blue + ");");
    picker_button->update();
}
