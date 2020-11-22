#ifndef ACTION_HPP
#define ACTION_HPP

#include <QAction>

#include <util.h>
#include <qt.h>

class action : public QAction
{
private:
    QString key;
    QShortcut *_shortcut;
    QString icon_name;
    QString tool_tip;
    QString label;

public:
    action(QString label, QString shortcut_key, QString icon_name);
    ~action();

    QString get_icon_name() const;
    void set_tool_tip(QString tip);
    QString get_tool_tip() const;
    QString get_label() const;

    template <class T, typename Func>
    void connect_to(T *obj, Func func)
    {
        // conecta la funcion a la accion
        connect(this, &QAction::triggered, obj, [=]() {
            func();
        });

        // ShortCut, y solo funciona si el puntero esta sobre el widget
        _shortcut = new QShortcut(QKeySequence(key), obj);
        QObject::connect(_shortcut, &QShortcut::activated, obj, [=]() {
            if (obj->underMouse())
                func();
        });
    }
};

#endif // ACTION_HPP
