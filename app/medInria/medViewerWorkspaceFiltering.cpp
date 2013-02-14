/* medViewerWorkspaceFiltering.cpp ---
 *
 * @author Clément Philipot <clement.philipot@inria.fr>
 *
 */

#include "medViewerWorkspaceFiltering.h"
#include <medViewContainerFiltering.h>

#include <medDatabaseNonPersistentController.h>
#include <medMetaDataKeys.h>
#include <medStorage.h>

#include <medViewerToolBoxViewProperties.h>
#include <medToolBoxFiltering.h>
#include <medToolBoxFactory.h>
#include <medViewContainer.h>
#include <medViewContainerMulti.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFilteringCustom.h>


#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medCore/medDataManager.h>

class medViewerWorkspaceFilteringPrivate
{
public:
    medViewerToolBoxViewProperties      *viewPropertiesToolBox;
    medToolBoxFiltering *filteringToolBox;
    dtkSmartPointer<dtkAbstractData> filterOutput;
    QString importUuid;
};

medViewerWorkspaceFiltering::medViewerWorkspaceFiltering ( QWidget *parent ) : medViewerWorkspace ( parent ), d ( new medViewerWorkspaceFilteringPrivate )
{
    d->viewPropertiesToolBox = new medViewerToolBoxViewProperties ( parent );

    this->addToolBox ( d->viewPropertiesToolBox );

    d->filteringToolBox = new medToolBoxFiltering ( parent );

    connect(d->filteringToolBox, SIGNAL(addToolBox(medToolBox *)), this, SLOT(addToolBox(medToolBox *)));
    connect(d->filteringToolBox, SIGNAL(removeToolBox(medToolBox *)), this, SLOT(removeToolBox(medToolBox *)));
    connect(d->filteringToolBox,SIGNAL(processFinished()),this,SLOT(onProcessSuccess()));

    this->addToolBox( d->filteringToolBox );
}

medViewerWorkspaceFiltering::~medViewerWorkspaceFiltering ( void )
{
    delete d;
    d = NULL;
}

void medViewerWorkspaceFiltering::setupViewContainerStack()
{
    if ( !this->stackedViewContainers()->count() )
    {
        medViewContainerFiltering *filteringContainer = new medViewContainerFiltering ( this->stackedViewContainers() );

        connect(filteringContainer,SIGNAL(droppedInput(medDataIndex)), d->filteringToolBox,SLOT(onInputSelected(medDataIndex)));
        connect(this,SIGNAL(outputDataChanged(dtkAbstractData *)),
                filteringContainer,SLOT(updateOutput(dtkAbstractData *)));
        connect(filteringContainer, SIGNAL(inputViewRemoved()),
                this, SLOT(onViewRemoved()));

        this->stackedViewContainers()->addContainer ( "Filtering",filteringContainer );

        setCurrentViewContainer ( "Filtering" );

        this->stackedViewContainers()->lockTabs();
        this->stackedViewContainers()->hideTabBar();
    }
}

void medViewerWorkspaceFiltering::patientChanged ( int patientId )
{
    d->filteringToolBox->clear();
}

void medViewerWorkspaceFiltering::onProcessSuccess()
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

//     QString newSeriesDescription = d->filterOutput->metadata ( medMetaDataKeys::SeriesDescription.key() );
//     newSeriesDescription += " filtered";

//     d->filterOutput->setMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

    QString generatedID = QUuid::createUuid().toString().replace("{","").replace("}","");
    d->filterOutput->setMetaData ( medMetaDataKeys::SeriesID.key(), generatedID );

    //     d->filteringToolBox->setDataIndex(medDataManager::instance()->importNonPersistent(d->filterOutput));

    QObject::connect ( medDatabaseNonPersistentController::instance(),
                       SIGNAL ( updated ( const medDataIndex&,const QString& ) ),
                       this, SLOT ( onOutputImported ( const medDataIndex&,const QString& ) ) );

    //Create a uniqueId for the request.
    d->importUuid = QUuid::createUuid().toString();
    medDataManager::instance()->importNonPersistent ( d->filterOutput, d->importUuid );

    emit outputDataChanged ( d->filterOutput );
}

void medViewerWorkspaceFiltering::onOutputImported ( const medDataIndex& dataIndex,
        const QString& uuid )
{
    if ( !uuid.isEmpty() && uuid == d->importUuid )
    {
        d->filteringToolBox->setDataIndex ( dataIndex );
        d->importUuid = QString();
    }
}

QString medViewerWorkspaceFiltering::identifier() const {
    return "Filtering";
}

void medViewerWorkspaceFiltering::onViewRemoved ()
{
    d->filteringToolBox->clear();
}


QString medViewerWorkspaceFiltering::description(void) const
{
    return "Filtering";
}

bool medViewerWorkspaceFiltering::isUsable(){
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("filtering").size()!=0); 
}