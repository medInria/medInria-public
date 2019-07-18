/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medFilteringWorkspaceL.h>


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
#include <medViewParameterGroupL.h>
#include <medLayerParameterGroupL.h>

class medFilteringWorkspaceLPrivate
{
public:
    QPointer<medFilteringSelectorToolBox> filteringToolBox;
    medViewContainer *inputContainer;
    medViewContainer *outputContainer;

    medAbstractData *filterOutput;
};

medFilteringWorkspaceL::medFilteringWorkspaceL(QWidget *parent) :
    medAbstractWorkspaceLegacy(parent), d(new medFilteringWorkspaceLPrivate)
{
    d->filteringToolBox = new medFilteringSelectorToolBox(parent);
    connect(d->filteringToolBox,SIGNAL(processFinished()),this,SLOT(onProcessSuccess()));
    this->addToolBox(d->filteringToolBox);

    medViewParameterGroupL *viewGroup1 = new medViewParameterGroupL("View Group 1", this, this->identifier());
    viewGroup1->setLinkAllParameters(true);
    viewGroup1->removeParameter("DataList");

    medLayerParameterGroupL *layerGroup1 = new medLayerParameterGroupL("Layer Group 1", this,  this->identifier());
    layerGroup1->setLinkAllParameters(true);
}

medFilteringWorkspaceL::~medFilteringWorkspaceL()
{
    delete d;
    d = nullptr;
}

/**
 * @brief sets up all the signal/slot connections when Viewer is switched to this workspace
 */
void medFilteringWorkspaceL::setupTabbedViewContainer()
{
    if ( !this->tabbedViewContainers()->count() )
    {
        d->inputContainer = this->tabbedViewContainers()->addContainerInTabNamed(this->name());
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

        this->tabbedViewContainers()->lockTabs();
        this->tabbedViewContainers()->hideTabBar();
        d->inputContainer->setSelected(true);
        d->outputContainer->setSelected(false);
    }
}

void medFilteringWorkspaceL::changeToolBoxInput()
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
void medFilteringWorkspaceL::onProcessSuccess()
{
    if(d->filteringToolBox.isNull())
        return;

    d->filterOutput = d->filteringToolBox->currentToolBox()->processOutput();
    if ( !d->filterOutput )
        return;

    qDebug() << "d->filterOutput->identifier()" << d->filterOutput->identifier();

    medAbstractData *inputData(d->filteringToolBox->data());

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

    medDataManager::instance()->importData(d->filterOutput);

    d->outputContainer->addData(d->filterOutput);
}

bool medFilteringWorkspaceL::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("filtering").size()!=0);
}

void medFilteringWorkspaceL::open(const medDataIndex &index)
{
    if(!index.isValidForSeries() || !d->inputContainer->isSelected())
        return;

    d->inputContainer->addData(medDataManager::instance()->retrieveData(index));
}
