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


class medVisualizationWorkspacePrivate;

class medVisualizationWorkspace : public medAbstractWorkspace
{
    Q_OBJECT
    MED_WORKSPACE_INTERFACE("Visualization",
                            "Visualize images, Mesh and other data types.")
public:
     medVisualizationWorkspace(QWidget *parent = 0);
    ~medVisualizationWorkspace();

    virtual void setupTabbedViewContainer();

    static bool isUsable();

private:
    medVisualizationWorkspacePrivate *d;
};


