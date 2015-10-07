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
#include <medViewParameterGroup.h>
#include <medLayerParameterGroup.h>

class medVisualizationWorkspacePrivate
{
public:

};

medVisualizationWorkspace::medVisualizationWorkspace(QWidget *parent) : medAbstractWorkspaceLegacy(parent), d(new medVisualizationWorkspacePrivate)
{
    medViewParameterGroup *viewGroup1 = new medViewParameterGroup("View Group 1", this, this->identifier());
    viewGroup1->setLinkAllParameters(true);
    viewGroup1->removeParameter("DataList");

    medLayerParameterGroup *layerGroup1 = new medLayerParameterGroup("Layer Group 1", this, this->identifier());
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
