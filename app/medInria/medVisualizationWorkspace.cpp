/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medVisualizationWorkspace.h"

#include <medViewContainer.h>
#include <medTabbedViewContainers.h>
#include <medVisualizationLayoutToolBox.h>
#include <medSettingsManager.h>
#include <medToolBoxFactory.h>

class medVisualizationWorkspacePrivate
{
public:
    medVisualizationLayoutToolBox * layoutToolBox;
    medToolBox * timeToolBox;
    medToolBox * viewPropertiesToolBox;
};

medVisualizationWorkspace::medVisualizationWorkspace(QWidget *parent) : medWorkspace(parent), d(new medVisualizationWorkspacePrivate)
{
    // -- Layout toolbox --
    d->layoutToolBox = new medVisualizationLayoutToolBox(parent);

    connect (d->layoutToolBox, SIGNAL(modeChanged(const QString&)),
             this,             SIGNAL(layoutModeChanged(const QString&)));
    connect (d->layoutToolBox, SIGNAL(presetClicked(int)),
             this,             SIGNAL(layoutPresetClicked(int)));
    connect (d->layoutToolBox, SIGNAL(split(int,int)),
             this,             SIGNAL(layoutSplit(int,int)));

    connect(this,SIGNAL(setLayoutTab(const QString &)), d->layoutToolBox, SLOT(setTab(const QString &)));

    this->addToolBox( d->layoutToolBox );

    // -- View toolbox --

    d->viewPropertiesToolBox = medToolBoxFactory::instance()->createToolBox("medViewPropertiesToolBox", parent);
    d->timeToolBox           = medToolBoxFactory::instance()->createToolBox("medTimeLineToolBox", parent);

    this->addToolBox( d->viewPropertiesToolBox );
    this->addToolBox( d->timeToolBox );

    connect ( this, SIGNAL(layoutModeChanged(const QString &)),
              stackedViewContainers(), SLOT(changeCurrentContainerType(const QString &)));
}

void medVisualizationWorkspace::setupViewContainerStack()
{
    if (!stackedViewContainers()->count())
    {
        const QString description = this->description();
        QString createdTab = addDefaultTypeContainer(description);
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
