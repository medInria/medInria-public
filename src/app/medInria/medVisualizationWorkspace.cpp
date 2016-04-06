/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVisualizationWorkspace.h>

#include <medViewContainer.h>
#include <medTabbedViewContainers.h>
#include <medSettingsManager.h>
#include <medToolBoxFactory.h>
#include <medViewParameterGroupL.h>
#include <medLayerParameterGroupL.h>

class medVisualizationWorkspacePrivate
{
public:

};

medVisualizationWorkspace::medVisualizationWorkspace(QWidget *parent) : medAbstractWorkspaceLegacy(parent), d(new medVisualizationWorkspacePrivate)
{
    medViewParameterGroupL *viewGroup1 = new medViewParameterGroupL("View Group 1", this, this->identifier());
    viewGroup1->setLinkAllParameters(true);
    viewGroup1->removeParameter("DataList");

    medLayerParameterGroupL *layerGroup1 = new medLayerParameterGroupL("Layer Group 1", this, this->identifier());
    layerGroup1->setLinkAllParameters(true);
}

void medVisualizationWorkspace::setupTabbedViewContainer()
{
    if (!tabbedViewContainers()->count()) {
        this->tabbedViewContainers()->addContainerInTab(this->name());
    }
    this->tabbedViewContainers()->unlockTabs();
}

medVisualizationWorkspace::~medVisualizationWorkspace(void)
{
    delete d;
    d = NULL;
}

bool medVisualizationWorkspace::isUsable(){
    return true; // for the time being, no test is defined.
}
