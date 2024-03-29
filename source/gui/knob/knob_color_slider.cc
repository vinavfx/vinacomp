#include <knob_color_slider.h>
#include <color_picker.h>
#include <util.h>

knob_color_slider::knob_color_slider(float min, float max, float default_value,
                                     bool centered_handler, QString _label,
                                     QWidget *knob, int dimension,
                                     QMenu *curve_menu)
    : layout(new QHBoxLayout(this))
    , edit(new number_box(knob, dimension))
    , _slider(new slider(min, max, default_value, true, centered_handler))
    , label(new QLabel(_label))
    , _colored_slider(nullptr)
    , value(default_value)
{
    edit->set_menu(curve_menu);

    layout->setMargin(0);

    label->setFixedWidth(20);
    edit->setMaximumWidth(50);
    edit->set_value(default_value);

    // Conecciones
    connect(_slider, &slider::moved, this, [this](float _value) {
        value = _value;
        changed(value);
        edit->set_value(value);

        if (_colored_slider)
            _colored_slider->set_value(value);
    });

    connect(edit, &QLineEdit::editingFinished, this, [=]() {
        value = edit->text().toDouble();
        _slider->set_value(value);
        changed(value);

        if (_colored_slider)
            _colored_slider->set_value(value);
    });

    // Layout
    if (_label.isEmpty())
    {
        layout->addWidget(edit);
        layout->addWidget(_slider);
    }
    else
    {
        layout->addWidget(label);
        layout->addWidget(_slider);
        layout->addWidget(edit);
    }
}

knob_color_slider::~knob_color_slider()
{
    delete layout;
    delete edit;
    delete _colored_slider;
    delete _slider;
}

void knob_color_slider::set_colored_slider(QString hsl_type)
{
    _colored_slider = new colored_slider(_slider, hsl_type);
}

void knob_color_slider::set_value(float _value, bool emmit_signal)
{
    value = _value;
    _slider->set_value(value);
    edit->set_value(value);

    if (_colored_slider)
        _colored_slider->set_value(value);
}

void knob_color_slider::set_hsl(float hue, float sat, float level)
{
    if (!_colored_slider)
        return;

    _colored_slider->set_hsl(hue, sat, level);
}

void knob_color_slider::set_default_value(float _default_value)
{
    _slider->set_default_value(_default_value);
}

colored_slider::colored_slider(slider *__slider, QString hsl_type)
    : _slider(__slider)
    , hue(hsl_type == "hue")
    , sat(hsl_type == "sat")
    , level(hsl_type == "level")
{
    this->setObjectName("colored_slider");
    QHBoxLayout *layout = new QHBoxLayout(_slider->get_slider_center());
    layout->setContentsMargins(1, 5, 1, 0);
    this->setFixedHeight(1);

    layout->addWidget(this);
}

void colored_slider::set_hsl(float _hue, float _sat, float _level)
{
    hue_value = _hue;
    sat_value = _sat;
    level_value = _level;

    color = color_picker::hsl_to_rgb(hue_value);

    update_handler_color(hue_value, sat_value, level_value);
}

void colored_slider::set_value(float value)
{
    if (sat)
        update_handler_color(hue_value, value, level_value);
    else if (level)
        update_handler_color(hue_value, sat_value, value);
}

void colored_slider::update_handler_color(float _hue, float _sat, float _level)
{
    QString style = "background: rgb(%1, %2, %3);";

    QColor handler_color;
    if (hue)
        handler_color = color;
    else if (sat)
    {
        sat_value = _sat;
        handler_color = color_picker::hsl_to_rgb(hue_value, sat_value);
    }
    else if (level)
    {
        level_value = _level;
        float sat = sat_value;
        sat -= level_value - 0.5;
        handler_color = color_picker::hsl_to_rgb(hue_value, sat, level_value);
    }

    _slider->get_handler()->setStyleSheet(style.arg(handler_color.red())
                                              .arg(handler_color.green())
                                              .arg(handler_color.blue()));
    update();
}

void colored_slider::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QLinearGradient ramp(0, 0, width(), 0);
    if (hue)
    {
        float section = 1.0 / 6;

        ramp.setColorAt(0.0, Qt::red);
        ramp.setColorAt(section, Qt::yellow);
        ramp.setColorAt(section * 2, Qt::green);
        ramp.setColorAt(section * 3, Qt::cyan);
        ramp.setColorAt(section * 4, Qt::blue);
        ramp.setColorAt(section * 5, Qt::magenta);
        ramp.setColorAt(1.0, Qt::red);
    }
    else if (sat)
    {
        ramp.setColorAt(0.0, Qt::gray);
        ramp.setColorAt(1.0, color);
    }
    else if (level)
    {
        QColor hue_and_sat = color_picker::hsl_to_rgb(hue_value, sat_value);
        ramp.setColorAt(0.0, Qt::black);
        ramp.setColorAt(0.5, hue_and_sat);
        ramp.setColorAt(1.0, Qt::white);
    }

    painter.fillRect(0, 0, width(), height(), QBrush(ramp));
}
