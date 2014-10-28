/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medFilteringWorkspace.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <medMetaDataKeys.h>
#include <medProcessSelectorToolBox.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medViewContainer.h>
#include <medFilteringAbstractToolBox.h>
#include <medAbstractData.h>
#include <medDataManager.h>

#include <medRunnableProcess.h>
#include <medJobManager.h>

#include <medAbstractView.h>
#include <medAbstractLayeredView.h>

#include <medViewParameterGroup.h>
#include <medLayerParameterGroup.h>

#include <medAbstractFilteringProcess.h>

class medFilteringWorkspacePrivate
{
public:
    QPointer<medProcessSelectorToolBox> filteringToolBox;
    medViewContainer *inputContainer;
    medViewContainer *outputContainer;

    medAbstractData *filterInput;
    medAbstractData *filterOutput;

    dtkSmartPointer <medAbstractFilteringProcess> process;
};

medFilteringWorkspace::medFilteringWorkspace(QWidget *parent): medAbstractWorkspace (parent), d(new medFilteringWorkspacePrivate)
{
    d->filterInput = NULL;
    d->filterOutput = NULL;

    d->filteringToolBox = new medProcessSelectorToolBox(parent);
    d->filteringToolBox->setTitle("Filtering");

    this->addToolBox(d->filteringToolBox);

    QStringList implementations = dtkAbstractProcessFactory::instance()->implementations("medAbstractFilteringProcess");
    d->filteringToolBox->setAvailableProcesses(implementations);

    medViewParameterGroup *viewGroup1 = new medViewParameterGroup("View Group 1", this, this->identifier());
    viewGroup1->setLinkAllParameters(true);
    viewGroup1->removeParameter("DataList");

    medLayerParameterGroup *layerGroup1 = new medLayerParameterGroup("Layer Group 1", this,  this->identifier());
    layerGroup1->setLinkAllParameters(true);

    connect(d->filteringToolBox, SIGNAL(processSelected(QString)), this, SLOT(setupProcess(QString)));
    connect(d->filteringToolBox, SIGNAL(startProcessRequested()), this, SLOT(startProcess()));
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

        connect(d->inputContainer, SIGNAL(viewContentChanged()), this, SLOT(updateInput()));
        connect(d->inputContainer, SIGNAL(viewRemoved()), this, SLOT(updateInput()));

        this->stackedViewContainers()->lockTabs();
        this->stackedViewContainers()->hideTabBar();
        d->inputContainer->setSelected(true);
        d->outputContainer->setSelected(false);
    }
}


void medFilteringWorkspace::setupProcess(QString process)
{
    d->process = dynamic_cast<medAbstractFilteringProcess*>(dtkAbstractProcessFactory::instance()->create(process));
    if(d->process)
    {
        d->filteringToolBox->setProcessToolbox(d->process->toolbox());
        d->process->setInputImage(d->filterInput);
    }
}

void medFilteringWorkspace::updateInput()
{
    if(!d->inputContainer->view())
    {
        d->filterInput = NULL;
        return;
    }
    medAbstractLayeredView *inputView = dynamic_cast<medAbstractLayeredView *>(d->inputContainer->view());
    if(!inputView)
    {
        qWarning() << "Non layered view are not supported in filtering workspace yet.";
        d->filterInput = NULL;
        return;
    }

    d->filterInput = inputView->layerData(inputView->currentLayer());

    if(d->process)
        d->process->setInputImage(d->filterInput);
}

/**
 * @brief adds metadata to the output and emits a signal outputDataChanged(medAbstractData *)
 */
void medFilteringWorkspace::handleProcessOutput()
{
    if(d->filteringToolBox.isNull())
        return;

    d->filterOutput = d->process->output();
    if ( !d->filterOutput )
        return;

    if (! d->filterOutput->hasMetaData(medMetaDataKeys::SeriesDescription.key()))
    {
        QString newSeriesDescription = d->filterInput->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " filtered";
        d->filterOutput->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }

    foreach ( QString metaData, d->filterInput->metaDataList() )
        if (!d->filterOutput->hasMetaData(metaData))
            d->filterOutput->addMetaData ( metaData, d->filterInput->metaDataValues ( metaData ) );

    foreach ( QString property, d->filterInput->propertyList() )
        d->filterOutput->addProperty ( property,d->filterInput->propertyValues ( property ) );

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


void medFilteringWorkspace::startProcess()
{
    if(!d->process)
        return;

    medRunnableProcess *runProcess = new medRunnableProcess;

    runProcess->setProcess (d->process);

//    d->progressionStack->addJobItem(runProcess, tr("Progress:"));
//    d->progressionStack->setActive(runProcess,true);
//    d->progressionStack->disableCancel(runProcess);

    connect (runProcess, SIGNAL (success  (QObject*)),  this, SIGNAL (success ()));
    connect (runProcess, SIGNAL (failure  (QObject*)),  this, SIGNAL (failure ()));
    connect (runProcess, SIGNAL (cancelled (QObject*)), this, SIGNAL (failure ()));

    connect (runProcess, SIGNAL (success  (QObject*)),this,SLOT(enableSelectorToolBox()));
    connect (runProcess, SIGNAL (failure()),this,SLOT(enableSelectorToolBox()));

    connect (runProcess, SIGNAL (success  (QObject*)),this,SLOT(handleProcessOutput()));

    //First have the moving progress bar,
    //and then display the remaining % when known
//    connect (runProcess, SIGNAL(activate(QObject*,bool)),
//             d->progressionStack, SLOT(setActive(QObject*,bool)));

    medJobManager::instance()->registerJobItem(runProcess,d->process->identifier());
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));
}
