#include <action.h>
#include <tools.h>
#include <qt.h>
#include <util.h>

action::action(QString _label, QString shortcut_key, QString _icon_name)
    : key(shortcut_key)
    , icon_name(_icon_name)
    , label(_label)
    , checkable(false)
    , checked(false)
    , visible(true)
    , disable(false)
    , has_shortcut(!shortcut_key.isEmpty())
    , _tools(nullptr)
    , button(nullptr)

{
    this->setText(label);
    if (!icon_name.isEmpty())
    {
        if (icon_name.contains("/"))
            this->setIcon(QIcon(icon_name));
        else
            this->setIcon(QIcon("resources/images/" + icon_name + "_normal.png"));
    }

    this->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    if (has_shortcut)
        this->setShortcut(QKeySequence(key));
}

action::~action()
{
    if (button)
        delete button;
}

void action::set_visible(bool _visible)
{
    visible = _visible;
    if (button)
        button->setVisible(visible);
}

void action::update()
{
    // actualiza la actual visibilidad
    set_visible(visible);
}

QPushButton *action::make_button(QWidget *__tools, int _icon_size, bool __one_check_at_time)
{
    _tools = __tools;
    _one_check_at_time = __one_check_at_time;
    icon_size = _icon_size;

    if (!button)
    {
        button = new QPushButton();
        if (!object_name.isEmpty())
            button->setObjectName(object_name);
        set_tool_tip(label);
        qt::set_icon(button, icon_name + "_normal", icon_size);

        connect(button, &QPushButton::clicked, this, [=]() {
            if (disable)
                return;

            this->trigger();
        });

        connect(button, &QPushButton::pressed, this, [=]() {
            qt::set_icon(button, icon_name + "_disable", icon_size);
        });

        connect(button, &QPushButton::released, this, [=]() {
            if (disable)
                return;

            if (checked)
                qt::set_icon(button, icon_name + "_checked", icon_size);
            else
                qt::set_icon(button, icon_name + "_normal", icon_size);
        });
    }

    return button;
}

void action::set_object_name(QString name)
{
    object_name = name;
}

void action::uncheck_all()
{
    if (!checkable || !_tools || !_one_check_at_time)
        return;

    static_cast<tools *>(_tools)->set_checked_all(false);
}

void action::set_checked(bool _checked, bool emmit_signal)
{
    if (!checkable)
        return;

    if (button && emmit_signal)
    {
        button->click();
        return;
    }

    checked = _checked;

    set_illuminated_button(false);

    if (checked)
        this->setIcon(QIcon("resources/images/" + icon_name + "_checked.png"));
    else
        this->setIcon(QIcon("resources/images/" + icon_name + "_normal.png"));

    this->setChecked(checked);
}

void action::set_illuminated_button(bool illuminated)
{
    if (!button)
        return;

    if (illuminated)
        qt::set_icon(button, icon_name + "_white", icon_size);
    else
    {
        if (checked)
            qt::set_icon(button, icon_name + "_checked", icon_size);
        else
            qt::set_icon(button, icon_name + "_normal", icon_size);
    }
}

QString action::get_icon_name() const
{
    return icon_name;
}

void action::set_icon(QString icon_name)
{
    this->setIcon(QIcon("resources/images/" + icon_name + ".png"));
}

void action::set_checkable(bool _checkable)
{
    checkable = _checkable;
    this->setCheckable(checkable);
}

bool action::is_checked() const
{
    return checked;
}

void action::set_tool_tip(QString tip)
{
    if (has_shortcut)
        tool_tip = tip + ":  " + key;
    else
        tool_tip = tip;

    if (button)
        button->setToolTip(tool_tip);
}

QString action::get_tool_tip() const
{
    return tool_tip;
}

QString action::get_label() const
{
    return label;
}

void action::set_disable(bool _disable)
{
    if (!button)
        return;

    disable = _disable;

    if (disable)
    {
        qt::set_icon(button, icon_name + "_disable", icon_size);
        return;
    }

    if (checked)
        qt::set_icon(button, icon_name + "_checked", icon_size);
    else
        qt::set_icon(button, icon_name + "_normal", icon_size);
}
