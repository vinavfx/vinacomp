#include <vinacomp.h>
#include <global.h>
#include <path_utils.h>

QJsonArray vinacomp::get_recent_projects()
{
    QString recent_projects_path = VINACOMP_CONF_PATH + "/recent_projects.json";
    QJsonArray recent_projects = jread(recent_projects_path)["projects"].toArray();

    return recent_projects;
}

void vinacomp::update_recent_projects()
{
    QJsonArray recent_projects = get_recent_projects();
    recent_projects_menu->clear();

    for (int i = 0; i < recent_projects.count(); i++)
    {
        QString project_path = recent_projects[i].toString();
        QAction *project_action = new QAction(project_path);
        project_action->setShortcut(QString("Alt+Shift+" + QString::number(i + 1)));

        connect(project_action, &QAction::triggered, this, [=]() { open_project(project_path); });

        recent_projects_menu->addAction(project_action);
    }
}

void vinacomp::recorder_recent_projects(QString project_path)
{
    // guarda el proyecto en la lista de proyectos recientes

    QString recent_projects_path = VINACOMP_CONF_PATH + "/recent_projects.json";
    int max_projects = 7;

    QJsonArray recent_projects = get_recent_projects();
    QJsonArray projects = {};

    // si el proyecto ya existe en la lista, lo elimina
    int index_delete = -1;
    for (int i = 0; i < recent_projects.count(); i++)
        if (recent_projects[i].toString() == project_path)
            index_delete = i;

    if (index_delete >= 0)
        recent_projects.removeAt(index_delete);
    //
    //
    recent_projects.push_front(project_path);

    if (recent_projects.count() < max_projects)
        max_projects = recent_projects.count();

    for (int i = 0; i < max_projects; i++)
        projects.push_back(recent_projects[i].toString());

    QJsonObject new_recent_projects;

    new_recent_projects.insert("projects", projects);
    jwrite(recent_projects_path, new_recent_projects);

    update_recent_projects();
}

void vinacomp::open_project_dialog()
{
    _file_dialog->set_default();
    _file_dialog->set_name_filter("VinaComp Project", {"vina"});
    _file_dialog->set_init_directory(os::dirname(current_project));

    if (!_file_dialog->exec())
        return;

    QString project_path = _file_dialog->get_files().first();

    open_project(project_path);
}

void vinacomp::open_project(QString project_path)
{
    if (!close_project())
        return;

    QString ext = path_util::get_ext(project_path);
    if (ext != "vina")
        return;

    project_opened = true;
    current_project = project_path;
    recorder_recent_projects(project_path);

    project->load(project_path);

    // actualiza componentes con el proyecto cargado
    _node_graph->restore_tree();
    _node_graph->restore_scene_data();
    // _script_editor->open_script_from_project();

    this->setWindowTitle(os::basename(project_path) + " - VinaComp");
}

bool vinacomp::close_project()
{
    if (!project_opened)
        return true;

    QMessageBox msgBox(this);
    msgBox.setText("There is a project open");
    msgBox.setInformativeText(
        "You want to save it before loading the new one?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                              QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();

    if (ret == QMessageBox::Save)
    {
        to_save_project();
        _node_graph->clear_tree();
        project->clear_nodes();
        project_opened = false;
        return true;
    }
    else if (ret == QMessageBox::Discard)
    {
        _node_graph->clear_tree();
        project->clear_nodes();
        project_opened = false;
        return true;
    }
    else
        return false;
}

void vinacomp::new_project()
{
    close_project();
}

void vinacomp::to_save_project()
{
    if (!project_opened)
        save_as();
    else
        save_project(current_project);
}

void vinacomp::save_as()
{
    _file_dialog->set_default();
    _file_dialog->set_save_mode();
    _file_dialog->set_name_filter("VinaComp Project", {"vina"});
    _file_dialog->set_init_directory(os::dirname(current_project));

    if (_file_dialog->exec())
    {
        QString project_path = _file_dialog->get_files().first();
        QString ext = path_util::get_ext(project_path);

        if (ext != "vina")
            project_path += ".vina";

        save_project(project_path);
    }
}

void vinacomp::save_project(QString project_path)
{
    _node_graph->save_nodes_attributes_to_project();
    _node_graph->save_scene_data();

    recorder_recent_projects(project_path);
    project->save(project_path);

    project_opened = true;
    current_project = project_path;

    this->setWindowTitle(os::basename(project_path) + " - VinaComp");
}
