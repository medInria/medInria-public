/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.
This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

//TODO use real shared d pointer instead ? - RDE

#pragma once

#include <QtCore/QHash>
#include <medDataIndex.h>

class medToolBoxContainer;
class medDatabaseNavigator;
class medTabbedViewContainers;
class medAbstractWorkspace;
class medClutEditor;
class QStackedWidget;
class QSplitter;
class medPatientSelectorToolBox;
class medToolBoxBody;
class medToolBoxTab;
class medDatabaseDataSource;
class medFileSystemDataSource;
class medAbstractDataSource;
class QTabWidget;
class medToolBox;


class medWorkspaceAreaPrivate
{
public:

    medToolBoxContainer *toolBoxContainer;
    QWidget *navigatorContainer;
    QWidget *viewContainer;

    medDatabaseDataSource *dbSource;
    medFileSystemDataSource *fsSource;
    QList <medAbstractDataSource*> dataSources;

    QPointer<medClutEditor> transFun;
    QStackedWidget *stack;
    QSplitter * splitter;
    QMutex mutex;
    QHash<QString,medAbstractWorkspace*> workspaces;
    medToolBox *selectionToolBox;

public:
    QString currentWorkspaceName;
    medAbstractWorkspace *currentWorkspace;
};


