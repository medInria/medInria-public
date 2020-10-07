/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractWorkspaceLegacy.h>

class medVisualizationWorkspace : public medAbstractWorkspaceLegacy
{
    Q_OBJECT
    MED_WORKSPACE_INTERFACE("Visualization",
                            "Workspace to visualise images, meshes and other data types.",
                            "Basic")
public:
    medVisualizationWorkspace(QWidget *parent = nullptr): medAbstractWorkspaceLegacy(parent){}

    static bool isUsable();

    void setupTabbedViewContainer() override;
};
