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

class medViewerToolBoxPatient;
class medViewerToolBoxLayout;
class medViewerToolBoxView;
class medToolBoxRegistration;
class medToolBoxContainer;

class medDatabaseNavigator;

class medViewerAreaStack;
class medViewerToolBoxConfiguration;
class medViewerToolBoxDiffusion;

class medClutEditor;

class QStackedWidget;

#include <QtCore/QHash>

class medViewerAreaPrivate
{
public:
    medViewerToolBoxPatient *patientToolBox;
    medViewerToolBoxConfiguration *configurationToolBox;
    medViewerToolBoxLayout *layoutToolBox;
    medViewerToolBoxView *viewToolBox;
    medViewerToolBoxDiffusion *diffusionToolBox;

    medToolBoxRegistration *registrationToolBox;
    medToolBoxContainer *toolbox_container;

    medDatabaseNavigator *navigator;

    medClutEditor * transFun;

    QStackedWidget *stack;

    QMutex mutex;
  
    QHash<int, medViewerAreaStack *> view_stacks;

public:
    int current_patient;
    int current_container;
    int current_container_preset;
};

#endif
