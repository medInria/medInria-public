#ifndef MEDWORKSPACEAREA_P_H
#define MEDWORKSPACEAREA_P_H

#include <QtCore/QHash>
#include <medDataIndex.h>

class medToolBoxContainer;
class medDatabaseNavigator;
class medTabbedViewContainers;
class medWorkspace;
class medClutEditor;
class QStackedWidget;
class QSplitter;
class medPatientSelectorToolBox;


class medWorkspaceAreaPrivate
{
public:

    medToolBoxContainer *toolBoxContainer;
    QFrame *navigatorContainer;
    QWidget *viewContainer;
    
    medPatientSelectorToolBox *patientToolBox;
    
    medDatabaseNavigator *navigator;

    QPointer<medClutEditor>  transFun;

    QStackedWidget *stack;
    
    //QGridLayout *layout;
    QSplitter * splitter;
    QGridLayout *navigatorContainerLayout;

    QMutex mutex;
  
    QHash<QString,medWorkspace*> workspaces;

    QPropertyAnimation *navigatorAnimation;

    void restoreSplitterSize(Qt::Orientation orientation);
    void saveSplitterSize(medWorkspace::LayoutType layout);

public:
    medDataIndex currentPatient;
    QHash<int, int> currentPatientContainer;
    QString currentWorkspaceName;
    medWorkspace *currentWorkspace;
    medWorkspace::LayoutType currentLayout;
};

#endif
