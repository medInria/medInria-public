/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once


#include <QtCore>

#include <medAbstractWorkspaceLegacy.h>

class medTabbedViewContainers;
class medRegistrationWorkspacePrivate;
class medAbstractData;

class medRegistrationWorkspace : public medAbstractWorkspaceLegacy
{
    Q_OBJECT
    MED_WORKSPACE_INTERFACE("Registration",
                            "Workspace to compute the spatial transformation between two images in order to register them.",
                            "Methodology")
public:
     medRegistrationWorkspace(QWidget *parent = 0);
    ~medRegistrationWorkspace();

    static bool isUsable();
    void setupTabbedViewContainer ();

protected slots:
    void updateFromFixedContainer();
    void updateFromMovingContainer();
    void updateUserLayerClosable(int tabIndex);
    void updateFromRegistrationSuccess(medAbstractData *output);

private slots:
    void removeSlectorInternToolBox();

private:
    medRegistrationWorkspacePrivate *d;
};


