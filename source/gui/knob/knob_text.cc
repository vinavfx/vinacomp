#include <knob_text.h>

knob_text::knob_text(knob_props props, QString _default_text)
    : knob(props)
    , default_text(_default_text)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(init_space);
    this->setObjectName("knob_text");

    text = new QLineEdit(default_text);
    connect(text, &QLineEdit::editingFinished, this, [=]() {
        changed(text->text());
        update_value(text->text());
    });

    connect(text, &QLineEdit::textChanged, this, &knob_text::text_changed);

    layout->addWidget(text);
}

knob_text::~knob_text() {}

void knob_text::restore_default()
{
    knob::restore_default();
    text->setText(default_text);
}

void knob_text::restore_param()
{
    text->setText(get_param_value().toString());
}

