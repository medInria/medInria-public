/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medFilteringWorkspaceL.h>

#include <medAbstractLayeredView.h>
#include <medAbstractSelectableToolBox.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <medSelectorToolBox.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medViewContainer.h>
#include <medWorkspaceFactory.h>

class medFilteringWorkspaceLPrivate
{
public:
    medViewContainer *inputContainer;
    medViewContainer *outputContainer;

    medAbstractData *filterOutput;
};

medFilteringWorkspaceL::medFilteringWorkspaceL(QWidget *parent)
    : medSelectorWorkspace (parent, staticName()), d(new medFilteringWorkspaceLPrivate)
{
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
    medAbstractWorkspaceLegacy::setupTabbedViewContainer();

    d->inputContainer = this->tabbedViewContainers()->containersInTab(0).at(0);
    resetDefaultWidgetInputContainer();

    d->outputContainer = d->inputContainer->splitVertically();
    resetDefaultWidgetOutputContainer();

    connect(d->inputContainer,  SIGNAL(viewContentChanged()), this, SLOT(changeToolBoxInput()), Qt::UniqueConnection);
    connect(d->inputContainer,  SIGNAL(viewRemoved()),        this, SLOT(changeToolBoxInput()), Qt::UniqueConnection);
    connect(d->inputContainer,  SIGNAL(viewRemoved()), this, SLOT(resetDefaultWidgetInputContainer()),  Qt::UniqueConnection);
    connect(d->outputContainer, SIGNAL(viewRemoved()), this, SLOT(resetDefaultWidgetOutputContainer()), Qt::UniqueConnection);

    d->inputContainer->setSelected(true);
    d->outputContainer->setSelected(false);
}

void medFilteringWorkspaceL::changeToolBoxInput()
{
    if(selectorToolBox()) //null when users close the software
    {
        if(!d->inputContainer->view())
        {
            selectorToolBox()->clear();
        }
        else
        {
            // Emits the inputChanged() signal caught by selector toolboxes to trigger an update.
            // The signal is only emitted for a change in the input container, not the output one.
            medAbstractLayeredView *layeredView = dynamic_cast<medAbstractLayeredView *>(d->inputContainer->view());
            selectorToolBox()->onInputSelected(layeredView->layerData(layeredView->currentLayer()));
        }
    }
}

void medFilteringWorkspaceL::resetDefaultWidgetInputContainer()
{
    if(selectorToolBox()) //null when users close the software
    {
        QLabel *inputLabel = new QLabel("INPUT");
        inputLabel->setAlignment(Qt::AlignCenter);
        d->inputContainer->setDefaultWidget(inputLabel);
        d->inputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
        d->inputContainer->setUserSplittable(false);
        d->inputContainer->setMultiLayered(false);
    }
}

void medFilteringWorkspaceL::resetDefaultWidgetOutputContainer()
{
    if(selectorToolBox()) //null when users close the software
    {
        QLabel *outputLabel = new QLabel("OUTPUT");
        outputLabel->setAlignment(Qt::AlignCenter);
        d->outputContainer->setDefaultWidget(outputLabel);
        d->outputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
        d->outputContainer->setUserSplittable(false);
        d->outputContainer->setMultiLayered(false);
        d->outputContainer->setUserOpenable(false);
    }
}

/**
 * @brief adds metadata to the output and emits a signal outputDataChanged(medAbstractData *)
 */
void medFilteringWorkspaceL::importProcessOutput()
{
    if(selectorToolBox()->data())
    {
        d->filterOutput = selectorToolBox()->currentToolBox()->processOutput();
        if ( d->filterOutput )
        {
            medAbstractData *inputData(selectorToolBox()->data());

            if (! d->filterOutput->hasMetaData(medMetaDataKeys::SeriesDescription.key()))
            {
                QString newSeriesDescription = inputData->metadata ( medMetaDataKeys::SeriesDescription.key() );
                newSeriesDescription += " filtered";
                d->filterOutput->setMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
            }

            for( QString metaData : inputData->metaDataList() )
            {
                if (!d->filterOutput->hasMetaData(metaData))
                {
                    d->filterOutput->setMetaData ( metaData, inputData->metaDataValues ( metaData ) );
                }
            }

            for( QString property : inputData->propertyList() )
            {
                d->filterOutput->addProperty ( property,inputData->propertyValues ( property ) );
            }

            QString generatedID = QUuid::createUuid().toString().replace("{","").replace("}","");
            d->filterOutput->setMetaData ( medMetaDataKeys::SeriesID.key(), generatedID );

            medDataManager::instance().importData(d->filterOutput);

            d->outputContainer->addData(d->filterOutput);
        }
    }
}

bool medFilteringWorkspaceL::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("Filtering").size()!=0);
}

bool medFilteringWorkspaceL::registered()
{
    return medWorkspaceFactory::instance()->registerWorkspace <medFilteringWorkspaceL>();
}

void medFilteringWorkspaceL::open(const medDataIndex &index)
{
    if(!index.isValidForSeries() || !d->inputContainer->isSelected())
    {
        return;
    }
    d->inputContainer->addData(medDataManager::instance().retrieveData(index));
}
