// QCodeEditor
#include <QLuaCompleter.h>
#include <QLanguage.h>

// Qt
#include <QStringListModel>
#include <QFile>

QLuaCompleter::QLuaCompleter(QObject *parent) :
    QCompleter(parent)
{
    // Setting up GLSL types
    QStringList list;

    
    QFile fl("libs/QCodeEditor/resources/languages/lua.xml");

    if (!fl.open(QIODevice::ReadOnly))
    {
        return;
    }

    QLanguage language(&fl);

    if (!language.isLoaded())
    {
        return;
    }

    auto keys = language.keys();
    for (auto&& key : keys)
    {
        auto names = language.names(key);
        list.append(names);
    }

    setModel(new QStringListModel(list, this));
    setCompletionColumn(0);
    setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    setCaseSensitivity(Qt::CaseSensitive);
    setWrapAround(true);
}
