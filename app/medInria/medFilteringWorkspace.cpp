/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medFilteringWorkspace.h>


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
#include <medViewParameterGroup.h>
#include <medLayerParameterGroup.h>

class medFilteringWorkspacePrivate
{
public:
    QPointer<medFilteringSelectorToolBox> filteringToolBox;
    medViewContainer *inputContainer;
    medViewContainer *outputContainer;


    medAbstractData *filterOutput;
};

medFilteringWorkspace::medFilteringWorkspace(QWidget *parent): medAbstractWorkspace (parent), d(new medFilteringWorkspacePrivate)
{
    d->filteringToolBox = new medFilteringSelectorToolBox(parent);
    connect(d->filteringToolBox,SIGNAL(processFinished()),this,SLOT(onProcessSuccess()));
    this->addToolBox(d->filteringToolBox);

    medViewParameterGroup *viewGroup1 = new medViewParameterGroup("View Group 1", this, this->identifier());
    viewGroup1->setLinkAllParameters(true);
    viewGroup1->removeParameter("DataList");

    medLayerParameterGroup *layerGroup1 = new medLayerParameterGroup("Layer Group 1", this,  this->identifier());
    layerGroup1->setLinkAllParameters(true);
}

medFilteringWorkspace::~medFilteringWorkspace()
{
    delete d;
    d = NULL;
}

/**
 * @brief sets up all the signal/slot connections when Viewer is switched to this workspace
 */
void medFilteringWorkspace::setupViewContainerStack()
{
    if ( !this->stackedViewContainers()->count() )
    {
        d->inputContainer = this->stackedViewContainers()->addContainerInTab(this->name());
        QLabel *inputLabel = new QLabel("INPUT");
        inputLabel->setAlignment(Qt::AlignCenter);
        d->inputContainer->setDefaultWidget(inputLabel);

        d->inputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
        d->inputContainer->setUserSplittable(false);
        d->inputContainer->setMultiLayered(false);

        d->outputContainer = d->inputContainer->splitVertically();
        QLabel *outputLabel = new QLabel("OUTPUT");
        outputLabel->setAlignment(Qt::AlignCenter);
        d->outputContainer->setDefaultWidget(outputLabel);
        d->outputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
        d->outputContainer->setUserSplittable(false);
        d->outputContainer->setMultiLayered(false);
        d->outputContainer->setUserOpenable(false);

        connect(d->inputContainer, SIGNAL(viewContentChanged()), this, SLOT(changeToolBoxInput()));
        connect(d->inputContainer, SIGNAL(viewRemoved()), this, SLOT(changeToolBoxInput()));

        this->stackedViewContainers()->lockTabs();
        this->stackedViewContainers()->hideTabBar();
        d->inputContainer->setSelected(true);
        d->outputContainer->setSelected(false);
    }
}

void medFilteringWorkspace::changeToolBoxInput()
{
    if(d->filteringToolBox.isNull())
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

/**
 * @brief adds metadata to the output and emits a signal outputDataChanged(medAbstractData *)
 */
void medFilteringWorkspace::onProcessSuccess()
{
    if(d->filteringToolBox.isNull()) return;

    d->filterOutput = d->filteringToolBox->currentToolBox()->processOutput();
    if ( !d->filterOutput ) return;

    medAbstractData *inputData(d->filteringToolBox->data());

    if (inputData) // filters without data in "Input" container
    {
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
    }

    QString generatedID = QUuid::createUuid().toString().replace("{","").replace("}","");
    d->filterOutput->setMetaData ( medMetaDataKeys::SeriesID.key(), generatedID );

    medDataManager::instance()->importData(d->filterOutput);

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

    d->inputContainer->addData(medDataManager::instance()->retrieveData(index));
}
