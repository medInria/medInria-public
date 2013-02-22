/* medVisualizationWorkspace.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 14:12:19 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 15 11:04:35 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 7
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medVisualizationWorkspace.h"

#include <medViewPropertiesToolBox.h>
#include <medViewContainer.h>
#include <medTabbedViewContainers.h>
#include <medTimeLineToolBox.h>
#include <medVisualizationLayoutToolBox.h>
#include <medSettingsManager.h>

class medVisualizationWorkspacePrivate
{
public:
    medVisualizationLayoutToolBox *layoutToolBox;
    medTimeLineToolBox *timeToolBox;
    medViewPropertiesToolBox *viewPropertiesToolBox;
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

    d->viewPropertiesToolBox = new medViewPropertiesToolBox(parent);
    d->timeToolBox           = new medTimeLineToolBox(parent);


    this->addToolBox( d->viewPropertiesToolBox );
    this->addToolBox( d->timeToolBox );

    connect ( this, SIGNAL(layoutModeChanged(const QString&)),
              d->timeToolBox, SLOT(onStopButton()));
    connect ( this, SIGNAL(layoutModeChanged(const QString &)),
              stackedViewContainers(), SLOT(changeCurrentContainerType(const QString &)));
    connect ( stackedViewContainers(), SIGNAL(currentChanged(const QString &)),
              this, SLOT(connectToolboxesToCurrentContainer(const QString &)));
}

void medVisualizationWorkspace::setupViewContainerStack()
{
    if (!stackedViewContainers()->count())
    {
        const QString description = this->description();
        addDefaultTypeContainer(description);
        this->connectToolboxesToCurrentContainer(description);
    }
    this->stackedViewContainers()->unlockTabs();
}

void medVisualizationWorkspace::connectToolboxesToCurrentContainer(const QString &name)
{
    connect(stackedViewContainers()->container(name),
            SIGNAL(viewAdded(dtkAbstractView*)),
            d->timeToolBox, SLOT(onViewAdded(dtkAbstractView*)));
    connect(stackedViewContainers()->container(name),
            SIGNAL(viewRemoved(dtkAbstractView*)),
            d->timeToolBox, SLOT(onViewRemoved(dtkAbstractView*)));
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
