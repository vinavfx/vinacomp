#ifndef NODE_GUI_H
#define NODE_GUI_H

#include <QWidget>
#include <QJsonObject>

#include <knob.h>
#include <project_struct.h>
#include <renderer.h>

class node_gui : public QWidget
{
private:
    QWidget *_trim_panel;
    QJsonObject knob_data;
    QJsonObject *params;
    QWidget *_vinacomp;

public:
    node_gui();
    ~node_gui();

    void setup(QWidget *_trim_panel, QWidget *_vinacomp, QJsonObject *params,
               QJsonObject knob_data, QString name);

    virtual void restore_param();
    virtual void changed(QString param_name);

protected:
    renderer *_renderer;
    project_struct *project;
    QString name;

    knob *get_knob(QString name) const;
    void update_value(QJsonValue value);
    QJsonValue get_param_value() const;
    inline QJsonValue get_default() const;
};

inline QJsonValue node_gui::get_default() const
{
    return knob_data.value("default");
}

#endif // NODE_GUI_H
