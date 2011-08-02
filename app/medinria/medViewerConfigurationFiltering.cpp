/* medViewerConfigurationFiltering.cpp --- 
 * 
 * @author Clément Philipot <clement.philipot@inria.fr>
 *
 */

#include "medViewerConfigurationFiltering.h"
#include <medGui/medViewContainerFiltering.h>

#include <medGui/medViewerToolBoxView.h>

#include <medGui/medToolBoxFiltering.h>

#include <medGui/medViewContainer.h>
#include <medGui/medViewContainerMulti.h>
#include <medGui/medStackedViewContainers.h>
#include <medGui/medToolBoxFilteringCustom.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include <medCore/medDataManager.h>

class medViewerConfigurationFilteringPrivate
{
public:
    medViewerToolBoxView   *viewToolBox;
    medToolBoxFiltering *filteringToolBox;
    dtkAbstractData* filterOutput;
//  QList<dtkAbstractView *> views;
};

medViewerConfigurationFiltering::medViewerConfigurationFiltering(QWidget *parent) : medViewerConfiguration(parent), d(new medViewerConfigurationFilteringPrivate)
{
  setLayoutToolBoxVisibility(false);
	
  d->viewToolBox = new medViewerToolBoxView(parent);
  
  this->addToolBox( d->viewToolBox );
  
  d->filteringToolBox = new medToolBoxFiltering(parent);
  
  this->addToolBox( d->filteringToolBox );    
  
  
  connect(d->filteringToolBox, SIGNAL(addToolBox(medToolBox *)), this, SLOT(addToolBox(medToolBox *)));
  connect(d->filteringToolBox, SIGNAL(removeToolBox(medToolBox *)), this, SLOT(removeToolBox(medToolBox *)));
  connect(d->filteringToolBox,SIGNAL(dataSelected(dtkAbstractData *)),this,SIGNAL(inputDataChanged(dtkAbstractData *)));
  connect(d->filteringToolBox,SIGNAL(processFinished()),this,SLOT(onProcessSuccess()));

  //this->setViewLayoutType (-1);
}

medViewerConfigurationFiltering::~medViewerConfigurationFiltering(void)
{
    delete d;
    d = NULL;
}

void medViewerConfigurationFiltering::setupViewContainerStack()
{
        if (!this->stackedViewContainers()->count())
        {
            medViewContainerFiltering *filteringContainer = new medViewContainerFiltering(this->stackedViewContainers());

            //connect(this,SIGNAL(inputDataChanged(dtkAbstractData *)),filteringContainer,SLOT(updateInput(dtkAbstractData *)));
            //
            connect(filteringContainer,SIGNAL(droppedInput(medDataIndex)), d->filteringToolBox,SLOT(onInputSelected(medDataIndex)));
            //
            connect(this,SIGNAL(outputDataChanged(dtkAbstractData *)),filteringContainer,SLOT(updateOutput(dtkAbstractData *)));

            this->stackedViewContainers()->addContainer("Filtering",filteringContainer);

            setCurrentViewContainer("Filtering");
        }
}

void medViewerConfigurationFiltering::patientChanged(int patientId)
{
   d->filteringToolBox->clear();
}

void medViewerConfigurationFiltering::onProcessSuccess()
{
	d->filterOutput = d->filteringToolBox->customToolbox()->processOutput();
	if(!d->filterOutput)
		return;

    QString newSeriesDescription = d->filterOutput->metadata(tr("SeriesDescription"));
    newSeriesDescription += " filtered";
    
	d->filterOutput->setMetaData(tr("SeriesDescription"),newSeriesDescription);

	d->filteringToolBox->setDataIndex(medDataManager::instance()->importNonPersistent(d->filterOutput));

	emit outputDataChanged(d->filterOutput);
}

QString medViewerConfigurationFiltering::description(void) const
{
    return "Filtering";
}

medViewerConfiguration *createMedViewerConfigurationFiltering(void)
{
    return new medViewerConfigurationFiltering;
}
