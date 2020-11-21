#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>

#include <action.h>
#include <util.h>
#include <qt.h>

class tools : public QWidget
{
private:
    QHBoxLayout *layout;

public:
    tools(/* args */);
    ~tools();

    void add_action(action *_action);
    void add_separator();
    void add_stretch();
    void add_widget(QWidget *widget);
};

#endif // TOOLS_HPP