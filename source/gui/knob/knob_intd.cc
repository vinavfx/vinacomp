#include <knob_intd.h>

knob_intd::knob_intd(QList <int> default_values)
{
	this->setObjectName("knob_dimensions");
    QHBoxLayout *layout = new QHBoxLayout(this);
	this->set_knob_layout(layout);
	layout->setMargin(0);

    layout->addWidget(init_space);

	int dimensions = default_values.count();
	for ( int i = 0; i < dimensions; i++ )
	{
		int value = default_values.value(i);
		QLineEdit *dimension_edit = new QLineEdit(QString::number(value));

		connect(dimension_edit, &QLineEdit::editingFinished, this, [=](){
			QList <int> values;
			for ( int i = 0; i < dimensions; i++ )
				values.push_back(get_value(i));

			changed(values); // Signal
		});

		dimension_edit->setMaximumWidth(50);
		layout->addWidget(dimension_edit);
		dimensions_edits.push_back(dimension_edit);
	}

    layout->addStretch();
}

knob_intd::~knob_intd()
{
	for (QLineEdit *edit : dimensions_edits)
		delete edit;
}

int knob_intd::get_value(int dimension) const
{
	if (dimension >= dimensions_edits.count())
		return 0;

	return dimensions_edits.value(dimension)->text().toInt();
}

void knob_intd::set_value(int value, int dimension)
{
	if (dimension >= dimensions_edits.count())
		return;

	dimensions_edits.value(dimension)->setText(QString::number(value));
}

void knob_intd::set_value(QList <int> values)
{
	for (int i = 0; i < dimensions_edits.count(); i++)
		set_value(values.value(i), i);

	changed(values); // Signal
}