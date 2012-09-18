/* medViewerArea_p.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Jun 28 15:18:12 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 15:36:45 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 17
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWERAREA_P_H
#define MEDVIEWERAREA_P_H

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


class medViewerAreaPrivate
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
