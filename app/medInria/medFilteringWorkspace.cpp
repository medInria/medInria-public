/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medFilteringWorkspace.h>

#include <dtkCore/dtkSmartPointer.h>


#include <medDatabaseNonPersistentController.h>
#include <medMetaDataKeys.h>
#include <medStorage.h>
#include <medFilteringSelectorToolBox.h>
#include <medToolBoxFactory.h>
#include <medViewContainer.h>
#include <medTabbedViewContainers.h>
#include <medFilteringAbstractToolBox.h>
#include <medAbstractData.h>
#include <medDataManager.h>
#include <medAbstractView.h>
#include <medAbstractLayeredView.h>

class medFilteringWorkspacePrivate
{
public:
    medFilteringSelectorToolBox *filteringToolBox;
    medViewContainer *inputContainer;
    medViewContainer *outputContainer;


    dtkSmartPointer<medAbstractData> filterOutput;
    QString importUuid;
};

medFilteringWorkspace::medFilteringWorkspace(QWidget *parent): medAbstractWorkspace (parent), d(new medFilteringWorkspacePrivate)
{
    d->filteringToolBox = new medFilteringSelectorToolBox(parent);
    connect(d->filteringToolBox,SIGNAL(processFinished()),this,SLOT(onProcessSuccess()));
    connect(d->filteringToolBox, SIGNAL(destroyed()), this, SLOT(removeInternSelectorToolBox()));
    this->addToolBox(d->filteringToolBox);
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
        d->inputContainer = this->stackedViewContainers()->addContainerInTab(this->name());
        QLabel *inputLabel = new QLabel("INPUT");
        inputLabel->setAlignment(Qt::AlignCenter);
        d->inputContainer->setDefaultWidget(inputLabel);

        d->inputContainer->setUserClosable(false);
        d->inputContainer->setUserSplittable(false);
        d->inputContainer->setMultiLayered(false);

        d->outputContainer = d->inputContainer->splitVertically();
        QLabel *outputLabel = new QLabel("OUTPUT");
        outputLabel->setAlignment(Qt::AlignCenter);
        d->outputContainer->setDefaultWidget(outputLabel);
        d->outputContainer->setUserClosable(false);
        d->outputContainer->setUserSplittable(false);
        d->outputContainer->setMultiLayered(false);
        d->outputContainer->setAcceptDrops(false);

        connect(d->inputContainer, SIGNAL(viewContentChanged()), this, SLOT(changeToolBoxInput()));
        connect(d->inputContainer, SIGNAL(viewRemoved()), this, SLOT(changeToolBoxInput()));

        this->stackedViewContainers()->lockTabs();
        this->stackedViewContainers()->hideTabBar();
    }
}

void medFilteringWorkspace::changeToolBoxInput()
{
    if(!d->filteringToolBox)
        return;

    if(!d->inputContainer->view())
    {
        d->filteringToolBox->clear();
        return;
    }

    medAbstractLayeredView *layeredView = dynamic_cast<medAbstractLayeredView *>(d->inputContainer->view());
    if(!layeredView)
    {
        qWarning() << "Non layered view are not supported in filtering workspace yet.";
        d->filteringToolBox->clear();
        return;
    }
    d->filteringToolBox->onInputSelected(layeredView->layerData(layeredView->currentLayer()));
}

void medFilteringWorkspace::onProcessSuccess()
{
    if(!d->filteringToolBox)
        return;

    d->filterOutput = d->filteringToolBox->currentToolBox()->processOutput();
    if ( !d->filterOutput )
        return;

    qDebug() << "d->filterOutput->identifier()" << d->filterOutput->identifier();

    dtkSmartPointer<medAbstractData> inputData(d->filteringToolBox->data());

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

    //Create a uniqueId for the request.
    d->importUuid = QUuid::createUuid().toString();
    medDataManager::instance()->importNonPersistent(d->filterOutput, d->importUuid);

    d->outputContainer->addData(d->filterOutput);
}

bool medFilteringWorkspace::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("filtering").size()!=0); 
}

void medFilteringWorkspace::open(const medDataIndex &index)
{
    if(!index.isValidForSeries() || !d->inputContainer->isSelected())
        return;

    d->inputContainer->addData(medDataManager::instance()->data(index));
}


void medFilteringWorkspace::removeInternSelectorToolBox()
{
    d->filteringToolBox = NULL;
}
