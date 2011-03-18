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

#include <medGui/medViewerToolBoxView.h>
#include <medGui/medViewerToolBoxViewProperties.h>
#include <medGui/medViewContainerStack.h>

class medViewerConfigurationVisualizationPrivate
{
public:
    medViewerToolBoxView                *viewToolBox;
    medViewerToolBoxViewProperties      *viewPropertiesToolBox;

};

medViewerConfigurationVisualization::medViewerConfigurationVisualization(QWidget *parent) : medViewerConfiguration(parent), d(new medViewerConfigurationVisualizationPrivate)
{
    // -- Layout toolbox --
    showLayoutToolBox();

    // d->layoutToolBox = new medViewerToolBoxLayout(parent);
    /*
    connect(d->layoutToolBox, SIGNAL(modeChanged(int)), this, SLOT(switchToContainer(int)));
    connect(d->layoutToolBox, SIGNAL(split(int, int)), this, SLOT(split(int, int)));
    connect(d->layoutToolBox, SIGNAL(presetClicked(int)), this, SLOT(switchToContainerPreset(int)));
    */
    
    // -- View toolbox --
    
    d->viewToolBox = new medViewerToolBoxView(parent);

    d->viewPropertiesToolBox = new medViewerToolBoxViewProperties(parent);
       
    this->addToolBox( d->viewToolBox );
    this->addToolBox(d->viewPropertiesToolBox);
 
    
    
    //this->setLayoutType( medViewerConfiguration::TopDbBottomTb );
    
}

void medViewerConfigurationVisualization::setupViewContainerStack()
{
    if (!stackedViewContainers()->count())
    {
        //Containers:
        addSingleContainer();
        addMultiContainer();
        addCustomContainer();   
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
