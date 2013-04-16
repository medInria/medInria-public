/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

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


