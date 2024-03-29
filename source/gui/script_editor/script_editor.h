#ifndef SCRIPT_EDITOR_HPP
#define SCRIPT_EDITOR_HPP

// los .h de Qt definen "slots" para otra cosa,
// si esta definido 'slots' , 'Python.h' da conflicto
#ifdef slots
#undef slots
#endif
#include "Python.h"
//

#include <stdio.h>

// QT5
#include <QHBoxLayout>
#include <QJsonObject>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

// Libs
#include <QCodeEditor.h>
#include <QPythonHighlighter.h>

#include <action.h>
#include <button.h>
#include <general_settings.h>


class script_editor : public QWidget
{
private:
    void setup_ui();
    QWidget *tools_setup_ui();
    QCodeEditor *code_editor();

    QCodeEditor *editor;
    QJsonObject *project;
    QTextEdit *output;
    QWidget *node_graph;
    QString script;
    general_settings *settings;
    QWidget *_vinacomp;

    // expression
    bool expression_editor;
    QWidget *expression_editor_bar;
    QLabel *exp_label;
    QWidget *current_knob;

    void expression_ok();
    void expression_cancel();
    void finish_expression_edit();

    // Acciones
    action *run_script_action, *save_action, *exit_action;

    // Group Edit
    QWidget *current_group;
    QWidget *group_box;
    QLabel *current_group_label;
    button *save_exit_button, *cancel_exit_button, *group_cancel_button;
    QWidget *group_separator_1, *group_separator_2;
    void save_script();
    void exit_script();
    void save_and_exit();
    void cancel_exit();

    void run_expression();
    void append_output(QString text, QColor color = Qt::white);
    QWidget *set_expression_editor_bar();

    // python
    void python_initialize();
    void append_sys_path(QString path);
    std::string std_out_err;
    PyObject *python_module;

public:
    script_editor(QJsonObject *_project, QWidget *_node_graph,
                  bool expression_editor = false,
                  general_settings *settings = 0, QWidget *_vinacomp = 0);
    ~script_editor();

    void run_script_from_editor(bool output_log = true);
    void run_script(QString script, bool only_output = false,
                    bool input_script_log = true);
    QString python_run(QString command);

    void open_script_from_project();
    void set_knob(QWidget *knob);
    void set_group_edit(QWidget *_group_gui);
    void exit_group_edit();
    void load_module(QString module_path);
};

#endif // SCRIPT_EDITOR_HPP
