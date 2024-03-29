#include <knob_slider.h>
#include <vinacomp.h>
#include "./node.h"
#include <animation.h>
#include <trim_panel.h>

knob_slider::knob_slider(knob_props props, float min, float max,
                         float _default_value, bool _bidimensional,
                         bool floating, bool centered_handler)
    : knob(props)
    , value_2_edit(nullptr)
    , bidimensional(_bidimensional)
    , show_dimensions(nullptr)
    , empty_widget(nullptr)
    , default_value(_default_value)

{
    this->setObjectName("knob_slider");
    layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(init_space);
    this->set_knob_layout(layout);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    if (_bidimensional)
        knob::set_dimensions(2);

    // value 1
    value_1_label = new QLabel(get_dimension_name(0));
    value_1_label->hide();

    value_1_edit = new number_box(this, 0);
    value_1_edit->set_menu(curve_menu);
    connect(value_1_edit, &number_box::changed, this, [=](float value) {
        values.first = value;

        if (!floating)
            values.first = round(values.first);

        set_value(values.first, 0);
    });

    value_1_edit->setMaximumWidth(50);

    // slider
    _slider = new slider(min, max, default_value, floating, centered_handler);
    connect(_slider, &slider::moved, this, [=](float value) {
        values = {value, value};

        value_1_edit->set_value(value);
        if (value_2_edit)
            value_2_edit->setText(QString::number(value));

        to_emmit_signal();
    });

    layout->addWidget(value_1_label);
    layout->addWidget(value_1_edit);

    if (bidimensional)
    {
        // value 2
        value_2_label = new QLabel(get_dimension_name(1));
        value_2_label->hide();

        value_2_edit = new number_box(this, 1);
        value_2_edit->set_menu(curve_menu);
        value_2_edit->hide();

        connect(value_2_edit, &number_box::changed, this, [=](float value) {
            values.second = value;
            if (!floating)
                values.second = round(values.second);

            set_value(values.second, 1);
        });
        value_2_edit->setMaximumWidth(50);

        // buton de dimensiones
        show_dimensions = new button();
        show_dimensions->setText("2");
        connect(show_dimensions, &QPushButton::clicked, this,
                [=]() { set_separate_dimensions(!separate_dimensions); });

        empty_widget = new QWidget();
        empty_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        empty_widget->hide();

        layout->addWidget(value_2_label);
        layout->addWidget(value_2_edit);
        layout->addWidget(empty_widget);
    }

    layout->addWidget(_slider);
    if (bidimensional)
        layout->addWidget(show_dimensions);

    set_value(default_value, 0, false);
    set_value(default_value, 1, false);

}

knob_slider::~knob_slider()
{
    delete value_1_edit;
    delete value_1_label;
    delete _slider;
    delete layout;
    if (bidimensional)
    {
        delete value_2_edit;
        delete value_2_label;
        delete empty_widget;
    }
}

void knob_slider::restore_default()
{
    knob::restore_default();
    set_animated(false);
    set_value(default_value, 0, false);
    set_value(default_value, 1, false);
    set_separate_dimensions(false);
}

void knob_slider::restore_param()
{
    knob::restore_param();

    float value_1, value_2;

    if (animated)
    {
        value_1 = anim::get_value(get_curve(0), project->frame);
        value_2 = anim::get_value(get_curve(1), project->frame);
    }
    else
    {
        QJsonArray param_value = get_param_value().toArray();

        QJsonValue _value_1 = param_value[0];
        QJsonValue _value_2 = param_value[1];

        value_1 = param_value[0].toDouble();
        value_2 = param_value[1].toDouble();
    }

    set_values({value_1, value_2}, false);
}

void knob_slider::set_animated(bool animated, int dimension)
{
    QList<number_box *> number_boxs;

    if (!bidimensional)
        number_boxs = {value_1_edit};
    else if (dimension == -1)
        number_boxs = {value_1_edit, value_2_edit};
    else if (dimension == 0)
        number_boxs = {value_1_edit};
    else if (dimension == 1)
        number_boxs = {value_2_edit};

    for (number_box *edit : number_boxs)
    {
        qt::set_property(edit, "animated", animated);
        if (!animated)
            qt::set_property(edit, "keyframe", false);
    }

    knob::set_animated(animated, dimension);
}

void knob_slider::set_disable(bool disable)
{
    value_1_edit->setDisabled(disable);
    if (value_2_edit)
        value_2_edit->setDisabled(disable);

    _slider->set_disable(disable);

    qt::set_property(value_1_edit, "disable", disable);

    knob::set_disable(disable);
}

void knob_slider::set_error(bool error)
{
    qt::set_property(value_1_edit, "error", error);
    if (error)
        value_1_edit->setText("Error");
}

void knob_slider::set_has_expression(bool expression)
{
    qt::set_property(value_1_edit, "expression", expression);
    _slider->set_disable(expression);
}

void knob_slider::to_emmit_signal(int dimension)
{
    update_handler();
    changed(values.first, values.second); // Signal

    if (bidimensional)
        update_value(QJsonArray{values.first, values.second}, dimension);
    else
        update_value(QJsonArray{values.first}, dimension);

    update_linked_knobs();
    to_node_panel(this);
}

void knob_slider::update_linked_knobs()
{
    // actualiza los knob linkeados
    for (knob *_knob : get_slaves_knobs())
    {
        knob_slider *_knob_slider = static_cast<knob_slider *>(_knob);
        _knob_slider->set_values(values, false);
    }
}

void knob_slider::update_animated()
{
    int frame = project->frame;

    for (int dimension = 0; dimension < dimensions; dimension++)
    {
        if (!is_animated(dimension))
            continue;

        QString curve = get_curve(dimension);
        bool keyframe = false;
        float value = anim::get_value(curve, frame, &keyframe);

        if (dimension == 0)
            qt::set_property(value_1_edit, "keyframe", keyframe);
        else
            qt::set_property(value_2_edit, "keyframe", keyframe);

        set_value(value, dimension, false);
    }
}

void knob_slider::set_separate_dimensions(bool separate)
{
    if (separate == separate_dimensions || !bidimensional)
        return;

    qt::set_property(show_dimensions, "active", separate);

    _slider->setVisible(!separate);
    value_1_label->setVisible(separate);
    value_2_edit->setVisible(separate);
    value_2_label->setVisible(separate);
    empty_widget->setVisible(separate);

    separate_dimensions = separate;

    // si las dimensiones no son separadas, calcula el promedio de las 2
    // dimensiones
    if (!separate)
    {
        float average = (values.first + values.second) / 2;
        values = {average, average};
        value_1_edit->set_value(average);
        value_2_edit->set_value(average);
        _slider->set_value(average);
    }

    update_knob_in_curve_editor();
}

void knob_slider::set_value(float value, int dimension, bool emmit_signal)
{
    if (!bidimensional && dimension >= 1)
        return;

    if (dimension == 0)
    {
        values.first = value;
        value_1_edit->set_value(value);
        _slider->set_value(value);
    }
    else
    {
        values.second = value;
        value_2_edit->set_value(value);
    }

    if (emmit_signal)
        to_emmit_signal(dimension);
}

void knob_slider::set_values(pair<float, float> _values, bool emmit_signal)
{
    values = _values;

    set_separate_dimensions(values.first != values.second);

    value_1_edit->setText(QString::number(values.first));
    _slider->set_value(_values.first);

    if (value_2_edit)
        value_2_edit->setText(QString::number(values.second));

    if (emmit_signal)
        to_emmit_signal();
}

float knob_slider::get_value(int dimension) const
{
    if (dimension == 0)
        return values.first;
    else
        return values.second;
}
