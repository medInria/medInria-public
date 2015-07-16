/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
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
class medAbstractWorkspaceLegacy;
class medClutEditor;
class QStackedWidget;
class QSplitter;
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
    QHash<QString, medAbstractWorkspaceLegacy*> workspaces;
    QPointer<medToolBox> selectionToolBox;

public:
    QString currentWorkspaceName;
    medAbstractWorkspaceLegacy *currentWorkspace;
};


