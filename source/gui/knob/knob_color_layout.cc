#include <knob_color.h>

void knob_color::setup_ui()
{
    top_layout->setMargin(0);
    content_layout->setMargin(0);
    content_layout->setSpacing(15);
    main_layout->setMargin(0);

    // Caja de colores separados
    separate_colors_box_layout->setMargin(0);

    red_hedit->setObjectName("red_edit");
    green_hedit->setObjectName("green_edit");
    blue_hedit->setObjectName("blue_edit");
    alpha_hedit->setObjectName("alpha_edit");

    rgba_box_layout->setMargin(0);
    rgba_box_layout->setSpacing(5);

    hsl_box_layout->setMargin(0);

    red_slider->setObjectName("red_slider");
    green_slider->setObjectName("green_slider");
    blue_slider->setObjectName("blue_slider");
    alpha_slider->setObjectName("alpha_slider");

    // Opciones de botones
    color_sample_button->setAutoFillBackground(true);
    color_sample_button->setSizePolicy(QSizePolicy::Fixed,
                                       QSizePolicy::Expanding);

    palette_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    mono_color_button->setSizePolicy(QSizePolicy::Fixed,
                                     QSizePolicy::Expanding);
    picker_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    mono_color_button->setText("4");
    rgb_button->setText("RGB");
    hsl_button->setText("HSL");

    palette_button->set_icon("color", icon_size);
    picker_button->set_icon("picker", icon_size);

    rgb_button->setObjectName("small_button");
    color_sample_button->setObjectName("color_sample_button");
    palette_button->setObjectName("small_button");
    mono_color_button->setObjectName("small_button");
    _color_picker->setObjectName("color_picker");
    hsl_button->setObjectName("small_button");
    picker_button->setObjectName("small_button");

    palette_button->set_checkable();
    mono_color_button->set_checkable();
    rgb_button->set_checkable();
    hsl_button->set_checkable();
    picker_button->set_checkable();

    rgb_button->set_checked(true);

    // Visibilidad Inicial
    rgb_button->hide();
    hsl_button->hide();
    picker_button->hide();
    _color_picker->hide();
    separate_colors_box->hide();
    rgba_box->hide();
    hsl_box->hide();

    // Aliniamiento de Layouts
    content_layout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    top_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    rgba_box->setSizePolicy(QSizePolicy::Expanding,
                                          QSizePolicy::Fixed);
    hsl_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _color_picker->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Layout
    separate_colors_box_layout->addWidget(red_hedit);
    separate_colors_box_layout->addWidget(green_hedit);
    separate_colors_box_layout->addWidget(blue_hedit);
    separate_colors_box_layout->addWidget(alpha_hedit);

    rgba_box_layout->addWidget(red_slider);
    rgba_box_layout->addWidget(green_slider);
    rgba_box_layout->addWidget(blue_slider);
    rgba_box_layout->addWidget(alpha_slider);

    hsl_box_layout->addWidget(hue_slider);
    hsl_box_layout->addWidget(sat_slider);
    hsl_box_layout->addWidget(level_slider);

    top_layout->addWidget(mono_slider);
    top_layout->addWidget(separate_colors_box);
    top_layout->addWidget(color_sample_button);
    top_layout->addWidget(picker_button);
    top_layout->addWidget(hsl_button);
    top_layout->addWidget(rgb_button);
    top_layout->addWidget(palette_button);
    top_layout->addWidget(mono_color_button);

    content_layout->addWidget(top_widget);
    content_layout->addWidget(_color_picker);
    content_layout->addWidget(hsl_box);
    content_layout->addWidget(rgba_box);

    main_layout->addWidget(init_space);
    main_layout->addWidget(content_widget);
}

void knob_color::connections()
{
    connect(mono_slider, &knob_color_slider::changed, this,
            [=](float value) { set_color(value, value, value, value); });

    connect(red_slider, &knob_color_slider::changed, this, [=](float value) {
        set_color(value, green, blue, alpha, true, false);
        update_hsl_sliders();
        update_color_picker();
    });

    connect(green_slider, &knob_color_slider::changed, this, [=](float value) {
        set_color(red, value, blue, alpha, true, false);
        update_hsl_sliders();
        update_color_picker();
    });

    connect(blue_slider, &knob_color_slider::changed, this, [=](float value) {
        set_color(red, green, value, alpha, true, false);
        update_hsl_sliders();
        update_color_picker();
    });

    connect(alpha_slider, &knob_color_slider::changed, this, [=](float value) {
        set_color(red, green, blue, value, true, false);
        update_hsl_sliders();
        update_color_picker();
    });

    connect(hue_slider, &knob_color_slider::changed, this, [this](float value) {
        set_hsl(value, sat_slider->get_value(), level_slider->get_value(), true,
                false);

        update_rgb_sliders();
        update_color_picker();
    });

    connect(sat_slider, &knob_color_slider::changed, this, [this](float value) {
        set_hsl(hue_slider->get_value(), value, level_slider->get_value(), true,
                false);

        update_rgb_sliders();
        update_color_picker();
    });

    connect(level_slider, &knob_color_slider::changed, this,
            [this](float value) {
                set_hsl(hue_slider->get_value(), sat_slider->get_value(), value,
                        true, false);

                update_rgb_sliders();
                update_color_picker();
            });

    // Edits Horizontales
    connect(red_hedit, &QLineEdit::editingFinished, this, [=]() {
        float value = red_hedit->text().toDouble();
        set_color(value, green, blue, alpha);
    });
    connect(green_hedit, &QLineEdit::editingFinished, this, [=]() {
        float value = green_hedit->text().toDouble();
        set_color(red, value, blue, alpha);
    });
    connect(blue_hedit, &QLineEdit::editingFinished, this, [=]() {
        float value = blue_hedit->text().toDouble();
        set_color(red, green, value, alpha);
    });
    connect(alpha_hedit, &QLineEdit::editingFinished, this, [=]() {
        float value = alpha_hedit->text().toDouble();
        set_color(red, green, blue, value);
    });
    //
    //

    connect(palette_button, &QPushButton::clicked, this,
            [this]() { toogle_advanced_options(); });

    connect(mono_color_button, &QPushButton::clicked, this,
            [this]() { toggle_mono_color(); });

    connect(picker_button, &QPushButton::clicked, this,
            [this]() { toogle_color_picker_option(); });

    connect(rgb_button, &QPushButton::clicked, this,
            [this]() { toogle_rgb_option(); });

    connect(hsl_button, &QPushButton::clicked, this,
            [this]() { toogle_hsl_option(); });
    //

    connect(_color_picker, &color_picker::changed, this,
            [this](QColor color, float hue, float sat, float level) {
                sat_slider->set_value(sat);
                level_slider->set_value(level);
                set_color(color, true, false);
                update_rgb_sliders();
            });
}
