/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once


#include <QtCore>

#include <medAbstractWorkspace.h>

class medTabbedViewContainers;
class medRegistrationWorkspacePrivate;
class medAbstractData;
class medAbstractJob;

class medRegistrationWorkspace : public medAbstractWorkspace
{
    Q_OBJECT
    MED_WORKSPACE_INTERFACE("Registration",
                            "Register a moving image to a fixed image.")
public:
     medRegistrationWorkspace(QWidget *parent = 0);
    ~medRegistrationWorkspace();

    static bool isUsable();
    void setupTabbedViewContainer ();

protected slots:
    void setupProcess(QString);
    void startProcess();
    void enableSelectorToolBox();

private:
    medRegistrationWorkspacePrivate *d;
};


