#ifndef KNOB_TEXT_HPP
#define KNOB_TEXT_HPP

#include <QWidget>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>

#include <knob.h>

class knob_text : public knob
{
	Q_OBJECT
private:
    /* data */
public:
    knob_text(QString default_text = "");
    ~knob_text();
signals:
	void changed(QString text);
};

#endif //KNOB_TEXT_HPP
