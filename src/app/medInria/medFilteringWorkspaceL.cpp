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
    connect(this->tabbedViewContainers(), SIGNAL(containersSelectedChanged()),
            selectorToolBox(), SIGNAL(inputChanged()));
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

    connect(d->inputContainer, SIGNAL(viewContentChanged()), this, SLOT(changeToolBoxInput()));
    connect(d->inputContainer, SIGNAL(viewRemoved()), this, SLOT(changeToolBoxInput()));
    connect(d->inputContainer, SIGNAL(viewRemoved()), this, SLOT(resetDefaultWidgetInputContainer()));
    connect(d->outputContainer, SIGNAL(viewRemoved()), this, SLOT(resetDefaultWidgetOutputContainer()));

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

            foreach ( QString metaData, inputData->metaDataList() )
            {
                if (!d->filterOutput->hasMetaData(metaData))
                {
                    d->filterOutput->setMetaData ( metaData, inputData->metaDataValues ( metaData ) );
                }
            }

            foreach ( QString property, inputData->propertyList() )
            {
                d->filterOutput->addProperty ( property,inputData->propertyValues ( property ) );
            }

            QString generatedID = QUuid::createUuid().toString().replace("{","").replace("}","");
            d->filterOutput->setMetaData ( medMetaDataKeys::SeriesID.key(), generatedID );

            medDataManager::instance()->importData(d->filterOutput);

            d->outputContainer->addData(d->filterOutput);
        }
    }
}

bool medFilteringWorkspaceL::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("Filtering Legacy").size()!=0);
}

void medFilteringWorkspaceL::open(const medDataIndex &index)
{
    if(!index.isValidForSeries() || !d->inputContainer->isSelected())
        return;

    d->inputContainer->addData(medDataManager::instance()->retrieveData(index));
}
