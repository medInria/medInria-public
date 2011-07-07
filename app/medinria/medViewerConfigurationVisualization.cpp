/* medViewerConfigurationVisualization.cpp ---
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

#include "medViewerConfigurationVisualization.h"

#include <medGui/medViewerToolBoxViewProperties.h>
#include <medGui/medViewContainer.h>
#include <medGui/medStackedViewContainers.h>
#include <medGui/medViewerToolBoxLayout.h>
#include <medGui/medViewerToolBoxView.h>
#include <medGui/medViewerToolBoxTime.h>

class medViewerConfigurationVisualizationPrivate
{
public:
    medViewerToolBoxLayout              *layoutToolBox;
    medViewerToolBoxTime                *timeToolBox;
    medViewerToolBoxView                *viewToolBox;
    medViewerToolBoxViewProperties      *viewPropertiesToolBox;

};

medViewerConfigurationVisualization::medViewerConfigurationVisualization(QWidget *parent) : medViewerConfiguration(parent), d(new medViewerConfigurationVisualizationPrivate)
{
    // -- Layout toolbox --
    d->layoutToolBox = new medViewerToolBoxLayout(parent);
    
    connect (d->layoutToolBox, SIGNAL(modeChanged(const QString&)),
             this,             SIGNAL(layoutModeChanged(const QString&)));
    connect (d->layoutToolBox, SIGNAL(presetClicked(int)),
             this,             SIGNAL(layoutPresetClicked(int)));
    connect (d->layoutToolBox, SIGNAL(split(int,int)),
             this,             SIGNAL(layoutSplit(int,int)));    

    this->addToolBox( d->layoutToolBox );
    
    // -- View toolbox --
    
    d->viewToolBox           = new medViewerToolBoxView(parent);
    d->viewPropertiesToolBox = new medViewerToolBoxViewProperties(parent);
    d->timeToolBox           = new medViewerToolBoxTime(parent);
    this->addToolBox( d->viewToolBox );
    this->addToolBox( d->viewPropertiesToolBox );
    this->addToolBox( d->timeToolBox );
}

void medViewerConfigurationVisualization::setupViewContainerStack()
{
    if (!stackedViewContainers()->count())
    {
        //Containers:
        addSingleContainer();
        addMultiContainer();
        addCustomContainer();
        connect(stackedViewContainers()->container("Single"),SIGNAL(viewAdded(dtkAbstractView*)),
            d->timeToolBox,SLOT(onViewAdded(dtkAbstractView*)));
        connect(stackedViewContainers()->container("Multi"),SIGNAL(viewAdded(dtkAbstractView*)),
			d->timeToolBox,SLOT(onViewAdded(dtkAbstractView*)));
        connect(stackedViewContainers()->container("Custom"),SIGNAL(viewAdded(dtkAbstractView*)),
			d->timeToolBox,SLOT(onViewAdded(dtkAbstractView*)));
        connect(stackedViewContainers()->container("Single"),SIGNAL(viewRemoved(dtkAbstractView*)),
            d->timeToolBox,SLOT(onViewRemoved(dtkAbstractView*)));
        connect(stackedViewContainers()->container("Multi"),SIGNAL(viewRemoved(dtkAbstractView*)),
                d->timeToolBox,SLOT(onViewRemoved(dtkAbstractView*)));
        connect(stackedViewContainers()->container("Custom"),SIGNAL(viewRemoved(dtkAbstractView*)),
                d->timeToolBox,SLOT(onViewRemoved(dtkAbstractView*)));
    }

}

medViewerConfigurationVisualization::~medViewerConfigurationVisualization(void)
{
    delete d;
    d = NULL;
}


QString medViewerConfigurationVisualization::description(void) const
{
    return "Visualization";
}

medViewerConfiguration *createMedViewerConfigurationVisualization(void)
{
    return new medViewerConfigurationVisualization;
}
