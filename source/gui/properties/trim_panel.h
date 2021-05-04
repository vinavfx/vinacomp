#ifndef TRIM_PANEL_HPP
#define TRIM_PANEL_HPP

#include <QHBoxLayout>
#include <QJsonArray>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QGraphicsItem>

// Gui
#include <knob.h>
#include <knob_button.h>
#include <nodes_load.h>
#include <properties.h>
#include <tab_widget.h>
#include <node_gui.h>

// Engine
#include <project_struct.h>

class trim_panel : public QWidget
{
private:
    QVBoxLayout *layout;
    QMap<QString, knob *> *knobs;

    QMap<QString, knob_button *> connected_buttons;

    knob_editor *_knob_editor;
    QGraphicsItem *this_node;
    nodes_load *nodes_loaded;
    QPushButton *restart_button;
    QWidget *_vinacomp;
    QWidget *_curve_editor;
    QList<QWidget *> *viewers_gl;

    project_struct *project;
    QWidget *_node_view;

    tab_widget *tabs;
    bool is_maximize;
    properties *_properties;

    QWidget *controls_tab;
    QVBoxLayout *controls_layout;
    QWidget *node_tab;
    QVBoxLayout *node_tab_layout;
    QLineEdit *name_edit;
    node_gui *_node_gui;
    QWidget *buttons;

    QString name;
    QString type;
    QString icon_name;
    QColor color;

    QJsonObject *data;

    void setup_gui_panels(QJsonArray _knobs);
    QWidget *top_buttons_setup_ui();
    tab_widget *tabs_ui();
    void setup_knobs(QJsonArray _knobs, QVBoxLayout *layout,
                     QList<QWidget *> *viewers);
    void setup_shared_params();
    void update_render();

public:
    QJsonArray custom_knobs;

    trim_panel(properties *_properties, QString _name, QString _type,
               QColor color, QString _icon_name, nodes_load *nodes_loaded,
               project_struct *project, QWidget *_vinacomp,
               QGraphicsItem *_node, QWidget *_node_view);
    ~trim_panel();

    void update_controls_knobs(QJsonArray _knobs);
    void maximize(bool _maximize);
    inline QString get_name() const;
    inline QString get_type() const;
    inline QColor get_color() const;
    inline void set_name(QString _name);
    inline knob *get_knob(QString name) const;
    inline bool maximized() const;
    inline QMap<QString, knob *> *get_knobs() const;
    inline QWidget *get_controls_tab() const;
};

inline QWidget *trim_panel::get_controls_tab() const
{
    return controls_tab;
}

inline knob *trim_panel::get_knob(QString name) const
{
    return knobs->value(name);
}

inline QMap<QString, knob *> *trim_panel::get_knobs() const
{
    return knobs;
}

inline QString trim_panel::get_name() const
{
    return name;
}

inline QString trim_panel::get_type() const
{
    return type;
}

inline QColor trim_panel::get_color() const
{
    return color;
}

inline void trim_panel::set_name(QString _name)
{
    name = _name;
    name_edit->setText(name);
}

inline bool trim_panel::maximized() const
{
    return is_maximize;
}

#endif // TRIM_PANEL_HPP
