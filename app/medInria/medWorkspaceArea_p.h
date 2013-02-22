#ifndef MEDWORKSPACEAREA_P_H
#define MEDWORKSPACEAREA_P_H

#include <QtCore/QHash>
#include <medDataIndex.h>

class medToolBoxContainer;
class medDatabaseNavigator;
class medTabbedViewContainers;
class medViewerWorkspace;
class medClutEditor;
class QStackedWidget;
class QSplitter;
class medViewerToolBoxPatient;


class medWorkspaceAreaPrivate
{
public:

    medToolBoxContainer *toolbox_container;
    QFrame *navigator_container;
    QWidget *view_container;
    
    medViewerToolBoxPatient *toolboxPatient;
    
    medDatabaseNavigator *navigator;

    QPointer<medClutEditor>  transFun;

    QStackedWidget *stack;
    
    //QGridLayout *layout;
    QSplitter * splitter;
    QGridLayout *navigator_container_layout;

    QMutex mutex;
  
    QHash<QString,medViewerWorkspace*> workspaces;

    QPropertyAnimation *navigator_animation;

    void restoreSplitterSize(Qt::Orientation orientation);
    void saveSplitterSize(medViewerWorkspace::LayoutType layout);

public:
    medDataIndex current_patient;
    QHash<int, int> current_patient_container;
    QString current_workspace_name;
    medViewerWorkspace *current_workspace;
    medViewerWorkspace::LayoutType current_layout;
};

#endif
