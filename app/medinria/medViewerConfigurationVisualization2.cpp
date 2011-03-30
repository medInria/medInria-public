/* medViewerConfigurationVisualization2.cpp --- 
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

#include "medViewerConfigurationVisualization2.h"

#include <medGui/medViewerToolBoxView.h>

class medViewerConfigurationVisualization2Private
{
public:
};

medViewerConfigurationVisualization2::medViewerConfigurationVisualization2(QWidget *parent) : medViewerConfiguration(parent), d(new medViewerConfigurationVisualization2Private)
{
    // -- Hide layout toolbox --
    setLayoutToolBoxVisibility(false);

    // -- View toolbox --

    this->addToolBox( new medViewerToolBoxView(parent) );

    this->setViewLayoutType  (medViewContainer::Custom);
    this->setCustomLayoutType(medViewContainerCustom::E);
}

medViewerConfigurationVisualization2::~medViewerConfigurationVisualization2(void)
{
    delete d;
    d = NULL;
}

QString medViewerConfigurationVisualization2::description(void) const
{
    return "Visualization";
}

void medViewerConfigurationVisualization2::setupViewContainerStack(medViewContainerStack *container)
{
    if (!container) {
        return;
    }
}

medViewerConfiguration *createMedViewerConfigurationVisualization2(void)
{
    return new medViewerConfigurationVisualization2;
}
