#ifndef PANEL_HPP
#define PANEL_HPP

// QT5
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMenu>
#include <QSplitter>
#include <QTabWidget>
#include <QTabBar>

#include <qt.hpp>
#include <util.hpp>
#include <node_graph.hpp>
#include <viewer.hpp>

class panel : public QWidget
{
private:
    void setup_top_buttons(QWidget *top_buttons);
    void close_panel();
    void add_tab(QString name);
    QString get_tab_label(QString name);

    QList<QSplitter *> *splitters;

    QWidget *container;
    QTabWidget *tab_section;
    node_graph *_node_graph;
    viewer *_viewer;
    QWidget *panels_layout;

public:
    panel(QWidget *_panels_layout,
          QList<QSplitter *> *_splitters,
          node_graph *_node_graph,
          viewer *_viewer);
    ~panel();

    QStringList tabs_list;

    panel *split(Qt::Orientation orientation);
    QSplitter *get_splitter();
    void add_tabs(QStringList tabs_list);
    void remove_tab(QString name);
};

#endif // PANEL_HPP