/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
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
#include <medParameterPoolManagerToolBox.h>

class medVisualizationWorkspacePrivate
{
public:

};

medVisualizationWorkspace::medVisualizationWorkspace(QWidget *parent) : medAbstractWorkspace(parent), d(new medVisualizationWorkspacePrivate)
{
    medParameterPoolManagerToolBox *poolTb = new medParameterPoolManagerToolBox;
    this->addWorkspaceToolBox(poolTb);
}

void medVisualizationWorkspace::setupViewContainerStack()
{
    if (!stackedViewContainers()->count())
    {
        const QString description = this->description();
        this->stackedViewContainers()->addContainerInTab(description);
    }
    this->stackedViewContainers()->unlockTabs();
}

medVisualizationWorkspace::~medVisualizationWorkspace(void)
{
    delete d;
    d = NULL;
}

QString medVisualizationWorkspace::identifier() const {
    return "Visualization";
}

QString medVisualizationWorkspace::description() const {
    return tr("Visualization");
}

bool medVisualizationWorkspace::isUsable(){
    return true; // for the time being, no test is defined.
}
