/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
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
class medAbstractDataSource;
class QTabWidget;
class medToolBox;

class medWorkspaceAreaPrivate
{
public:

    medToolBoxContainer *toolBoxContainer;
    QWidget *navigatorContainer;
    QWidget *viewContainer;

    QStackedWidget *stack;
    QSplitter * splitter;
    QHash<QString, medAbstractWorkspaceLegacy*> workspaces;
    QPointer<medToolBox> selectionToolBox;

    QString currentWorkspaceName;
    medAbstractWorkspaceLegacy *currentWorkspace;
};
