#include <button.h>
#include <qt.h>

button::button(QWidget *parent, bool _hover)
    : QPushButton(parent)
    , hover(_hover)
    , checkable(false)
    , checked(false)
{
    if (hover)
    {
        this->setMouseTracking(true);
        this->setTabletTracking(true);
    }

    connect(this, &QPushButton::clicked, this, [=]() {
        if (checkable)
        {
            checked = !checked;
            if (checked)
                set_hover_icon();
            else
                set_normal_icon();
        }
        clicked(checked);
    });
}

button::~button() {}

void button::change_icon(QString name)
{
    QPixmap pixmap("resources/images/" + name + ".png");
    if (name.contains("/"))
        pixmap = QPixmap(name);
    QIcon ButtonIcon(pixmap);
    this->setIcon(ButtonIcon);
    this->setIconSize(QSize(size_icon, size_icon));
}

void button::set_icon(QString name, int size)
{
    size_icon = size;
    if (name.contains("/"))
    {
        normal_icon = name;
        hover_icon = name;
    }
    else
    {
        normal_icon = name + "_a";
        hover_icon = name + "_c";
    }

    change_icon(normal_icon);
}

void button::set_hover_icon()
{
    change_icon(hover_icon);
}

void button::set_normal_icon()
{
    change_icon(normal_icon);
}
