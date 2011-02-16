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

#include <medGui/medViewContainerStack.h>
#include <medGui/medViewContainer.h>
#include <medGui/medToolBoxFilteringCustom.h>

#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

class medViewerConfigurationFilteringPrivate
{
public:
    medViewerToolBoxView   *viewToolBox;
    medToolBoxFiltering *filteringToolBox;
    dtkAbstractData* filterOutput;
    QList<dtkAbstractView *> views;
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
    connect(d->filteringToolBox,SIGNAL(processFinished()),this,SLOT(onProcessSuccess()));
    
}

medViewerConfigurationFiltering::~medViewerConfigurationFiltering(void)
{
    delete d;
    d = NULL;
}

// TO DO : method to complete
void medViewerConfigurationFiltering::setupViewContainerStack(medViewContainerStack *container_stack)
{
    if (!container_stack) {
        return;
    }

    d->views.clear();

    medViewContainer *filteringContainer = container_stack->customContainer("Filtering");

    if (!filteringContainer) {
      medViewContainerCustom *custom = new medViewContainerCustom (container_stack);
      custom->setPreset (medViewContainerCustom::E);
      filteringContainer = custom;

      container_stack->addCustomContainer ("Filtering", filteringContainer);
    }

    d->views << filteringContainer->views();

    container_stack->setCustomContainer ("Filtering");
}

void medViewerConfigurationFiltering::patientChanged(int patientId)
{
    d->filteringToolBox->clear();
}

void medViewerConfigurationFiltering::onProcessSuccess()
{

        //affiche resultat dans la vue principale

        d->filterOutput = d->filteringToolBox->customToolbox()->processOutput();

        dtkAbstractView* view = dtkAbstractViewFactory::instance()->create("v3dView");
        view->setData(d->filterOutput);

        d->views.append(view); //d->views.clear() ?

}



QString medViewerConfigurationFiltering::description(void) const
{
    return "Filtering";
}

medViewerConfiguration *createMedViewerConfigurationFiltering(void)
{
    return new medViewerConfigurationFiltering;
}
