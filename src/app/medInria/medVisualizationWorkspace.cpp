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


medVisualizationWorkspace::medVisualizationWorkspace(QWidget *parent) : medAbstractWorkspaceLegacy(parent)
{
    medViewParameterGroupL *viewGroup1 = new medViewParameterGroupL("View Group 1", this, this->identifier());
    viewGroup1->setLinkAllParameters(true);
    viewGroup1->removeParameter("DataList");

    medLayerParameterGroupL *layerGroup1 = new medLayerParameterGroupL("Layer Group 1", this, this->identifier());
    layerGroup1->setLinkAllParameters(true);
}

void medVisualizationWorkspace::setupTabbedViewContainer()
{
    if (!tabbedViewContainers()->count())
    {
        this->tabbedViewContainers()->addContainerInTabNamed(this->name());
    }
    this->tabbedViewContainers()->unlockTabs();
    this->tabbedViewContainers()->setKeepLeastOne(true);
}

medVisualizationWorkspace::~medVisualizationWorkspace(void)
{
}

bool medVisualizationWorkspace::isUsable()
{
    return true; // for the time being, no test is defined.
}

