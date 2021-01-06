#include <knob_choice.h>

knob_choice::knob_choice(QStringList _list)
{
	this->setObjectName("knob_choice");
    QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(0);

    layout->addWidget(init_space);

    choice = new combo_box();
    layout->addWidget(choice);

    add_items(_list);

    layout->addStretch();
}

knob_choice::~knob_choice()
{
}

void knob_choice::add_items(QStringList list)
{
    choice->add_items(list);
}
