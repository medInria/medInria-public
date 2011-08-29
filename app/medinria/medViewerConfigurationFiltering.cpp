/* medViewerConfigurationFiltering.cpp ---
 *
 * @author Clément Philipot <clement.philipot@inria.fr>
 *
 */

#include "medViewerConfigurationFiltering.h"
#include <medViewContainerFiltering.h>

#include <medDatabaseNonPersistentController.h>
#include <medMetaDataKeys.h>

#include <medViewerToolBoxViewProperties.h>
#include <medToolBoxFiltering.h>

#include <medViewContainer.h>
#include <medViewContainerMulti.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFilteringCustom.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include <medCore/medDataManager.h>

class medViewerConfigurationFilteringPrivate
{
public:
    medViewerToolBoxViewProperties      *viewPropertiesToolBox;
    medToolBoxFiltering *filteringToolBox;
    dtkAbstractData *filterOutput;
};

medViewerConfigurationFiltering::medViewerConfigurationFiltering(QWidget *parent) : medViewerConfiguration(parent), d(new medViewerConfigurationFilteringPrivate)
{
    d->viewPropertiesToolBox = new medViewerToolBoxViewProperties(parent);

    this->addToolBox( d->viewPropertiesToolBox );

    d->filteringToolBox = new medToolBoxFiltering(parent);

    this->addToolBox( d->filteringToolBox );

    connect(d->filteringToolBox, SIGNAL(addToolBox(medToolBox *)), this, SLOT(addToolBox(medToolBox *)));
    connect(d->filteringToolBox, SIGNAL(removeToolBox(medToolBox *)), this, SLOT(removeToolBox(medToolBox *)));
    connect(d->filteringToolBox,SIGNAL(processFinished()),this,SLOT(onProcessSuccess()));
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

        connect(filteringContainer,SIGNAL(droppedInput(medDataIndex)), d->filteringToolBox,SLOT(onInputSelected(medDataIndex)));

        connect(this,SIGNAL(outputDataChanged(dtkAbstractData *)),filteringContainer,SLOT(updateOutput(dtkAbstractData *)));

        this->stackedViewContainers()->addContainer("Filtering",filteringContainer);

        setCurrentViewContainer("Filtering");

        this->stackedViewContainers()->lockTabs();
        this->stackedViewContainers()->hideTabBar();
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
    
    dtkAbstractData *inputData = d->filteringToolBox->data();

    foreach(QString metaData, inputData->metaDataList())
        d->filterOutput->addMetaData(metaData,inputData->metaDataValues(metaData));
    
    foreach(QString property, inputData->propertyList())
        d->filterOutput->addProperty(property,inputData->propertyValues(property));

    QString newSeriesDescription = d->filterOutput->metadata(medMetaDataKeys::SeriesDescription.key());
    newSeriesDescription += " filtered";

    d->filterOutput->setMetaData(medMetaDataKeys::SeriesDescription.key(), newSeriesDescription);
    
    //     d->filteringToolBox->setDataIndex(medDataManager::instance()->importNonPersistent(d->filterOutput));

    QObject::connect( medDatabaseNonPersistentController::instance(), SIGNAL(updated(medDataIndex)), this, SLOT(onOutputImported(medDataIndex)));
    medDatabaseNonPersistentController::instance()->import(d->filterOutput);

    emit outputDataChanged(d->filterOutput);
}

void medViewerConfigurationFiltering::onOutputImported ( const medDataIndex& dataIndex)
{
  d->filteringToolBox->setDataIndex(dataIndex);
}

QString medViewerConfigurationFiltering::description(void) const
{
    return "Filtering";
}

medViewerConfiguration *createMedViewerConfigurationFiltering(void)
{
    return new medViewerConfigurationFiltering;
}
