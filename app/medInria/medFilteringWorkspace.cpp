/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medFilteringWorkspace.h>

#include <medAbstractLayeredView.h>
#include <medAbstractSelectableToolBox.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <medSelectorToolBox.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medViewContainer.h>

class medFilteringWorkspacePrivate
{
public:
    medViewContainer *inputContainer;
    medViewContainer *outputContainer;

    medAbstractData *filterOutput;
};

medFilteringWorkspace::medFilteringWorkspace(QWidget *parent): medSelectorWorkspace (parent, staticName()), d(new medFilteringWorkspacePrivate)
{
    connect(this->stackedViewContainers(), SIGNAL(containersSelectedChanged()),
            selectorToolBox(), SIGNAL(inputChanged()));
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
    medAbstractWorkspace::setupViewContainerStack();

    d->inputContainer = this->stackedViewContainers()->containersInTab(0).at(0);
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

void medFilteringWorkspace::changeToolBoxInput()
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

void medFilteringWorkspace::resetDefaultWidgetInputContainer()
{
    QLabel *inputLabel = new QLabel("INPUT");
    inputLabel->setAlignment(Qt::AlignCenter);
    d->inputContainer->setDefaultWidget(inputLabel);
    d->inputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    d->inputContainer->setUserSplittable(false);
    d->inputContainer->setMultiLayered(false);
}

void medFilteringWorkspace::resetDefaultWidgetOutputContainer()
{
    QLabel *outputLabel = new QLabel("OUTPUT");
    outputLabel->setAlignment(Qt::AlignCenter);
    d->outputContainer->setDefaultWidget(outputLabel);
    d->outputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    d->outputContainer->setUserSplittable(false);
    d->outputContainer->setMultiLayered(false);
    d->outputContainer->setUserOpenable(false);
}

/**
 * @brief adds metadata to the output and emits a signal outputDataChanged(medAbstractData *)
 */
void medFilteringWorkspace::importProcessOutput()
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

bool medFilteringWorkspace::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("Filtering").size()!=0);
}

void medFilteringWorkspace::open(const medDataIndex &index)
{
    if(!index.isValidForSeries() || !d->inputContainer->isSelected())
        return;

    d->inputContainer->addData(medDataManager::instance()->retrieveData(index));
}
