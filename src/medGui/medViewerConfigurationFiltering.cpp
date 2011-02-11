/* medViewerConfigurationFiltering.cpp --- 
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

#include "medViewerConfigurationFiltering.h"

#include "medViewerToolBoxView.h"

#include "medToolBoxFiltering.h"

class medViewerConfigurationFilteringPrivate
{
public:
    medViewerToolBoxView   *viewToolBox;
    medToolBoxFiltering *filteringToolBox;
};

medViewerConfigurationFiltering::medViewerConfigurationFiltering(QWidget *parent) : medViewerConfiguration(parent), d(new medViewerConfigurationFilteringPrivate)
{


    d->viewToolBox = new medViewerToolBoxView(parent);
    
    this->addToolBox( d->viewToolBox );
    
    d->filteringToolBox = new medToolBoxFiltering(parent);
   
    this->addToolBox( d->filteringToolBox );    
    
    //TO DO : connections with filteringToolBox
    connect(d->filteringToolBox, SIGNAL(addToolBox(medToolBox *)), this, SLOT(addToolBox(medToolBox *)));
    connect(d->filteringToolBox, SIGNAL(removeToolBox(medToolBox *)), this, SLOT(removeToolBox(medToolBox *)));    
    
}

medViewerConfigurationFiltering::~medViewerConfigurationFiltering(void)
{
    delete d;
    d = NULL;
}

// TO DO : method to complete
void medViewerConfigurationFiltering::setupViewContainerStack(medViewContainerStack *container)
{
    if (!container) {
        return;
    }
}

void medViewerConfigurationFiltering::patientChanged(int patientId)
{
    d->filteringToolBox->clear();
}


QString medViewerConfigurationFiltering::description(void) const
{
    return "Filtering";
}

medViewerConfiguration *createMedViewerConfigurationFiltering(void)
{
    return new medViewerConfigurationFiltering;
}
