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

#include <medGui/medViewContainer.h>
#include <medGui/medViewContainerStack.h>


#include "medViewerToolBoxView.h"
#include "medViewerToolBoxTime.h"

class medViewerConfigurationVisualizationPrivate
{
public:
    medViewerToolBoxView   *viewToolBox;
    medViewerToolBoxTime   *timeToolBox;
};

medViewerConfigurationVisualization::medViewerConfigurationVisualization(QWidget *parent) : medViewerConfiguration(parent), d(new medViewerConfigurationVisualizationPrivate)
{
    // -- Layout toolbox --
    showLayoutToolBox();
    // -- View toolbox --
    
    d->viewToolBox = new medViewerToolBoxView(parent);
    d->timeToolBox = new medViewerToolBoxTime(parent);

    this->addToolBox( d->viewToolBox );
    this->addToolBox( d->timeToolBox );    
}

medViewerConfigurationVisualization::~medViewerConfigurationVisualization(void)
{
    delete d;
    d = NULL;
}

void medViewerConfigurationVisualization::setupViewContainerStack(medViewContainerStack *container_stack)
{
  if (!container_stack) {
    return;
  }
  
  medViewContainer *visualizationContainer = container_stack->customContainer("Visualization");
  
  if (!visualizationContainer) {
    medViewContainerCustom *custom = new medViewContainerCustom (container_stack);
    custom->setPreset (medViewContainerCustom::E);
    connect (custom, SIGNAL (viewAdded (dtkAbstractView*)),   d->timeToolBox, SLOT (onViewAdded (dtkAbstractView*)));
    connect (custom, SIGNAL (viewRemoved (dtkAbstractView*)), d->timeToolBox, SLOT (onViewRemoved (dtkAbstractView*)));
    visualizationContainer = custom;
    container_stack->addCustomContainer ("Visualization", visualizationContainer);
  }
  
  container_stack->setCustomContainer ("Visualization");
}


QString medViewerConfigurationVisualization::description(void) const
{
  return "Visualization";
}

medViewerConfiguration *createMedViewerConfigurationVisualization(void)
{
  return new medViewerConfigurationVisualization;
}
