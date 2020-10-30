#ifndef TRIM_PANEL_HPP
#define TRIM_PANEL_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>

// VinaComp
#include <trim_panel.hpp>
#include <qt.hpp>

#include <knob_color.hpp>
#include <knob_check_box.hpp>
#include <knob_file.hpp>
#include <knob_choice.hpp>
#include <knob_text.hpp>
#include <knob_label.hpp>
#include <knob_button.hpp>
#include <knob_group.hpp>
#include <knob_integer.hpp>
#include <knob_floating.hpp>
#include <knob_separator.hpp>
#include <knob_position.hpp>

class trim_panel : public QWidget
{
private:
    QTabWidget *tabs;
    bool is_minimize = false;

    void setup_ui();
    QWidget *top_buttons_setup_ui();
    QTabWidget *tabs_ui();

public:
    trim_panel(/* args */);
    ~trim_panel();
};

#endif //TRIM_PANEL_HPP