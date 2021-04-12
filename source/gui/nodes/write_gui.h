#ifndef WRITE_GUI_H
#define WRITE_GUI_H

#include <node_gui.h>
#include <render_data.h>
#include <renderer.h>

class write_gui : public node_gui
{
private:
    void changed(QString param_name) override;
    renderer *_renderer;

    int first_frame, last_frame, frame;
    QString filename;

    void start_render();
    void finished_render(render_data _rdata);

public:
    write_gui(project_struct *project);
    ~write_gui();
};

#endif // WRITE_GUI_H
