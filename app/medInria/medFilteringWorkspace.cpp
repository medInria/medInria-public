/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medFilteringWorkspace.h"
#include <medFilteringViewContainer.h>

#include <medDatabaseNonPersistentController.h>
#include <medMetaDataKeys.h>
#include <medStorage.h>

#include <medFilteringSelectorToolBox.h>
#include <medToolBoxFactory.h>
#include <medViewContainer.h>
#include <medMultiViewContainer.h>
#include <medTabbedViewContainers.h>
#include <medFilteringAbstractToolBox.h>


#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medCore/medDataManager.h>

class medFilteringWorkspacePrivate
{
public:
    medToolBox *viewPropertiesToolBox;
    medFilteringSelectorToolBox *filteringToolBox;
    dtkSmartPointer<dtkAbstractData> filterOutput;
    QString importUuid;
};

medFilteringWorkspace::medFilteringWorkspace ( QWidget *parent ) : medWorkspace ( parent ), d ( new medFilteringWorkspacePrivate )
{
    d->viewPropertiesToolBox = medToolBoxFactory::instance()->createToolBox("medViewPropertiesToolBox", parent);

    this->addToolBox ( d->viewPropertiesToolBox );

    d->filteringToolBox = new medFilteringSelectorToolBox ( parent );

    connect(d->filteringToolBox, SIGNAL(addToolBox(medToolBox *)), this, SLOT(addToolBox(medToolBox *)));
    connect(d->filteringToolBox, SIGNAL(removeToolBox(medToolBox *)), this, SLOT(removeToolBox(medToolBox *)));
    connect(d->filteringToolBox,SIGNAL(processFinished()),this,SLOT(onProcessSuccess()));

    this->addToolBox( d->filteringToolBox );
}

medFilteringWorkspace::~medFilteringWorkspace()
{
    delete d;
    d = NULL;
}

void medFilteringWorkspace::setupViewContainerStack()
{
    if ( !this->stackedViewContainers()->count() )
    {
        medFilteringViewContainer *filteringViewContainer = new medFilteringViewContainer ( this->stackedViewContainers() );

        connect(filteringViewContainer,SIGNAL(droppedInput(medDataIndex)), d->filteringToolBox,SLOT(onInputSelected(medDataIndex)));
        connect(this,SIGNAL(outputDataChanged(dtkAbstractData *)),
                filteringViewContainer,SLOT(updateOutput(dtkAbstractData *)));
        connect(filteringViewContainer, SIGNAL(viewRemoved(dtkAbstractView *)),
                this, SLOT(onViewRemoved()));

        this->stackedViewContainers()->addContainer ( "Filtering",filteringViewContainer );

        setCurrentViewContainer ( "Filtering" );

        this->stackedViewContainers()->lockTabs();
        this->stackedViewContainers()->hideTabBar();
    }
}

void medFilteringWorkspace::patientChanged ( int patientId )
{
    d->filteringToolBox->clear();
}

void medFilteringWorkspace::onProcessSuccess()
{
    d->filterOutput = d->filteringToolBox->customToolbox()->processOutput();
    if ( !d->filterOutput )
        return;

    dtkSmartPointer<dtkAbstractData> inputData(d->filteringToolBox->data());

    if (! d->filterOutput->hasMetaData(medMetaDataKeys::SeriesDescription.key()))
      {
        QString newSeriesDescription = inputData->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " filtered";
        d->filterOutput->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
      }

    foreach ( QString metaData, inputData->metaDataList() )
      if (!d->filterOutput->hasMetaData(metaData))
        d->filterOutput->addMetaData ( metaData, inputData->metaDataValues ( metaData ) );

    foreach ( QString property, inputData->propertyList() )
      d->filterOutput->addProperty ( property,inputData->propertyValues ( property ) );

    QString generatedID = QUuid::createUuid().toString().replace("{","").replace("}","");
    d->filterOutput->setMetaData ( medMetaDataKeys::SeriesID.key(), generatedID );

    QObject::connect ( medDatabaseNonPersistentController::instance(),
                       SIGNAL ( updated ( const medDataIndex&,const QString& ) ),
                       this, SLOT ( onOutputImported ( const medDataIndex&,const QString& ) ) );

    //Create a uniqueId for the request.
    d->importUuid = QUuid::createUuid().toString();
    medDataManager::instance()->importNonPersistent ( d->filterOutput, d->importUuid );

    emit outputDataChanged ( d->filterOutput );
}

void medFilteringWorkspace::onOutputImported ( const medDataIndex& dataIndex,
        const QString& uuid )
{
    if ( !uuid.isEmpty() && uuid == d->importUuid )
    {
        d->filteringToolBox->setDataIndex ( dataIndex );
        d->importUuid = QString();
    }
}

QString medFilteringWorkspace::identifier() const {
    return "Filtering";
}

void medFilteringWorkspace::onViewRemoved ()
{
    d->filteringToolBox->clear();
}


QString medFilteringWorkspace::description(void) const
{
    return "Filtering";
}

bool medFilteringWorkspace::isUsable(){
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("filtering").size()!=0); 
}
