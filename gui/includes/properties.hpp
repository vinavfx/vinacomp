#ifndef PROPERTIES_HPP
#define PROPERTIES_HPP

// QT5
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

#include <trim_panel.hpp>
#include <qt.hpp>

class properties : public QWidget
{
private:
    void setup_ui();
    QWidget *top_buttons_setup_ui();

public:
    properties(/* args */);
    ~properties();
};

#endif // PROPERTIES_HPP