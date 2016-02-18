/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "reformatWorkspace.h"

#include <medSegmentationAbstractToolBox.h>

#include <medWorkspaceFactory.h>
#include <medToolBoxFactory.h>
#include <medTabbedViewContainers.h>

// /////////////////////////////////////////////////////////////////
// reformatWorkspacePrivate
// /////////////////////////////////////////////////////////////////

class reformatWorkspacePrivate
{
public:
    medToolBox * viewPropertiesToolBox;
};

// /////////////////////////////////////////////////////////////////
// reformat
// /////////////////////////////////////////////////////////////////

reformatWorkspace::reformatWorkspace(QWidget *parent) : medAbstractWorkspace(parent), d(new reformatWorkspacePrivate)
{
    // -- View toolboxes --
    QList<QString> toolboxNames = medToolBoxFactory::instance()->toolBoxesFromCategory("reformat");
    if(toolboxNames.contains("medViewPropertiesToolBox"))
    {
        // we want the medViewPropertiesToolBox to be the first "view" toolbox
        toolboxNames.move(toolboxNames.indexOf("medViewPropertiesToolBox"),0);
    }

    foreach(QString toolbox, toolboxNames)
    {
        if (toolbox=="medViewPropertiesToolBox")
        {
            d->viewPropertiesToolBox = medToolBoxFactory::instance()->createToolBox(toolbox, parent);
            addToolBox(d->viewPropertiesToolBox);
        }
        else
        {
            medToolBox* toolBox = medToolBoxFactory::instance()->createToolBox(toolbox, parent);
            addToolBox(toolBox);
            toolBox->setWorkspace(this);
            toolBox->switchMinimize();
        }
    }
}

reformatWorkspace::~reformatWorkspace()
{
    
}

void reformatWorkspace::setupViewContainerStack()
{
    if (!stackedViewContainers()->count()) {
        this->stackedViewContainers()->addContainerInTab(this->name());
    }
    this->stackedViewContainers()->unlockTabs();
}

bool reformatWorkspace::isUsable()
{
    // TODO: you can add some conditions here to check if your workspace is ready to be used
    // (successfully initialized, ...)
    return true;
}

bool reformatWorkspace::registered()
{
    return medWorkspaceFactory::instance()->registerWorkspace<reformatWorkspace>();
}

void reformatWorkspace::showViewPropertiesToolBox(bool val)
{
    if (val)
        d->viewPropertiesToolBox->show();
    else
        d->viewPropertiesToolBox->hide();
}
