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

#include <medGui/medViewerToolBoxView.h>

#include <medGui/medToolBoxFiltering.h>

#include <medGui/medViewContainer.h>
#include <medGui/medViewContainerMulti.h>
#include <medGui/medStackedViewContainers.h>
#include <medGui/medToolBoxFilteringCustom.h>

#include <dtkCore/dtkAbstractData.h>
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
  setLayoutToolBoxVisibility(false);
  
	//d->filteringContainer = NULL;
	
  d->viewToolBox = new medViewerToolBoxView(parent);
  
  this->addToolBox( d->viewToolBox );
  
  d->filteringToolBox = new medToolBoxFiltering(parent);
  
  this->addToolBox( d->filteringToolBox );    
  
  
  connect(d->filteringToolBox, SIGNAL(addToolBox(medToolBox *)), this, SLOT(addToolBox(medToolBox *)));
  connect(d->filteringToolBox, SIGNAL(removeToolBox(medToolBox *)), this, SLOT(removeToolBox(medToolBox *)));
  connect(d->filteringToolBox,SIGNAL(processFinished()),this,SLOT(onProcessSuccess()));
	connect(d->filteringToolBox,SIGNAL(dataSelected(dtkAbstractData *)),this,SLOT(onInputDataSelected(dtkAbstractData *)));
	
  //this->setViewLayoutType (-1);
}

medViewerConfigurationFiltering::~medViewerConfigurationFiltering(void)
{
    delete d;
    d = NULL;
}

void medViewerConfigurationFiltering::setupViewContainerStack()
{

    d->views.clear();

		medViewContainer *filteringContainer;

    if (!this->stackedViewContainers()->count()) {
      medViewContainerMulti *custom = new medViewContainerMulti ();
      //custom->setPreset (medViewContainerCustom::A);
			custom->setAcceptDrops(false);
      filteringContainer = custom;


      this->stackedViewContainers()->addContainer (description(), filteringContainer);

    }
    else
      filteringContainer = this->stackedViewContainers()->container(description());
  
	d->views = filteringContainer->views();

	
	if (d->views.size() < 2)
	{
		unsigned int oldSize = d->views.size();
		for (unsigned int i = oldSize;i < 2;++i)
		{
			d->views.append(dtkAbstractViewFactory::instance()->create("v3dView"));
			//d->views[oldSize + i]->clear();
			//d->views[oldSize + i]->reset();
			filteringContainer->setView(d->views[oldSize + i]);
		}
	}


  this->stackedViewContainers()->setContainer (description());
}

void medViewerConfigurationFiltering::patientChanged(int patientId)
{
   d->filteringToolBox->clear();
}

void medViewerConfigurationFiltering::onInputDataSelected(dtkAbstractData *data)
{
	d->views[0]->setData(data,0);
	d->views[0]->reset();
	d->views[0]->update();
	
	d->views[1]->reset();
}

void medViewerConfigurationFiltering::onProcessSuccess()
{
	//show result in main view
	
	d->filterOutput = d->filteringToolBox->customToolbox()->processOutput();
	if(!d->filterOutput)
		return;

	d->views[1]->setData(d->filterOutput,0);
	d->views[1]->reset();
	d->views[1]->update();
}

QString medViewerConfigurationFiltering::description(void) const
{
    return "Filtering";
}

medViewerConfiguration *createMedViewerConfigurationFiltering(void)
{
    return new medViewerConfigurationFiltering;
}
