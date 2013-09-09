/*=========================================================================

 medInria

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
class medToolBoxBody;
class medToolBoxTab;
class medDatabaseDataSource;
class medFileSystemDataSource;
class medAbstractDataSource;


class medWorkspaceAreaPrivate
{
public:

    medToolBoxContainer *toolBoxContainer;
    
    medToolBoxBody *navigatorBody;
    medToolBoxTab  *navigatorTab;
    
    QWidget *viewContainer;  
    
    medDatabaseDataSource *dbSource;
    medFileSystemDataSource *fsSource;
    QList <medAbstractDataSource*> dataSources;
    
    QPointer<medClutEditor>  transFun;
    QStackedWidget *stack; 
    QSplitter * splitter;
    QMutex mutex;
    QHash<QString,medWorkspace*> workspaces;
    QPropertyAnimation *navigatorAnimation;

    void restoreSplitterSize(Qt::Orientation orientation);
    void saveSplitterSize(medWorkspace::LayoutType layout);

public:
    QString currentWorkspaceName;
    medWorkspace *currentWorkspace;
    medWorkspace::LayoutType currentLayout;
};


