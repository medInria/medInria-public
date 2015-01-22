/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medRegistrationWorkspace.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <medMetaDataKeys.h>
#include <medProcessSelectorToolBox.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medViewContainer.h>
#include <medAbstractData.h>
#include <medDataManager.h>

#include <medJobManager.h>

#include <medAbstractView.h>
#include <medAbstractLayeredView.h>

#include <medViewParameterGroup.h>
#include <medLayerParameterGroup.h>

#include <medAbstractApplyTransformationProcess.h>
#include <medAbstractEstimateTransformationProcess.h>
#include <medTriggerParameter.h>
#include <medViewContainerSplitter.h>

class medRegistrationWorkspacePrivate
{
public:
    QPointer<medProcessSelectorToolBox> estimateTransfoToolBox;
    QPointer<medProcessSelectorToolBox> applyTransfoToolBox;
    medViewContainer *movingContainer;
    medViewContainer *fixedContainer;

    medAbstractData *movingData;
    medAbstractData *fixedData;

    dtkSmartPointer <medAbstractApplyTransformationProcess> applyTransfoProcess;
    dtkSmartPointer <medAbstractEstimateTransformationProcess> estimateTransfoProcess;

    QPointer<medViewContainerSplitter> currentSplitter;
};

medRegistrationWorkspace::medRegistrationWorkspace(QWidget *parent): medAbstractWorkspace (parent), d(new medRegistrationWorkspacePrivate)
{
    d->movingData = NULL;
    d->fixedData = NULL;
    d->applyTransfoProcess = NULL;
    d->estimateTransfoProcess = NULL;

    d->estimateTransfoToolBox = new medProcessSelectorToolBox(parent);
    d->estimateTransfoToolBox->setTitle("Estimate transformation");

    d->applyTransfoToolBox = new medProcessSelectorToolBox(parent);
    d->applyTransfoToolBox->setTitle("Apply transformation");

    this->addToolBox(d->estimateTransfoToolBox);
    this->addToolBox(d->applyTransfoToolBox);

    QStringList implementations = dtkAbstractProcessFactory::instance()->implementations("medAbstractEstimateTransformationProcess");
    d->estimateTransfoToolBox->setAvailableProcesses(implementations);

    implementations = dtkAbstractProcessFactory::instance()->implementations("medAbstractApplyTransformationProcess");
    d->applyTransfoToolBox->setAvailableProcesses(implementations);

    medViewParameterGroup *viewGroup1 = new medViewParameterGroup("View Group 1", this, this->identifier());
    viewGroup1->setLinkAllParameters(true);
    viewGroup1->removeParameter("DataList");

    medLayerParameterGroup *layerGroup1 = new medLayerParameterGroup("Layer Group 1", this,  this->identifier());
    layerGroup1->setLinkAllParameters(true);

    connect(d->estimateTransfoToolBox, SIGNAL(processSelected(QString)), this, SLOT(setupEstimationProcess(QString)));
    connect(d->applyTransfoToolBox, SIGNAL(processSelected(QString)), this, SLOT(setupApplicationProcess(QString)));
}

medRegistrationWorkspace::~medRegistrationWorkspace()
{
    delete d;
    d = NULL;
}

/**
 * @brief sets up all the signal/slot connections when Viewer is switched to this workspace
 */
void medRegistrationWorkspace::setupTabbedViewContainer()
{
//    if ( !this->tabbedViewContainers()->count() )
//    {
//        d->inputContainer = this->tabbedViewContainers()->addContainerInTab(this->name());
//        QLabel *inputLabel = new QLabel("INPUT");
//        inputLabel->setAlignment(Qt::AlignCenter);
//        d->inputContainer->setDefaultWidget(inputLabel);

//        d->inputContainer->setClosingMode(medViewContainer::CLOSE_VIEW_ONLY);
//        d->inputContainer->setUserSplittable(false);
//        d->inputContainer->setMultiLayered(false);

//        d->outputContainer = d->inputContainer->splitVertically();
//        QLabel *outputLabel = new QLabel("OUTPUT");
//        outputLabel->setAlignment(Qt::AlignCenter);
//        d->outputContainer->setDefaultWidget(outputLabel);
//        d->outputContainer->setClosingMode(medViewContainer::CLOSE_VIEW_ONLY);
//        d->outputContainer->setUserSplittable(false);
//        d->outputContainer->setMultiLayered(false);
//        d->outputContainer->setUserOpenable(false);

//        connect(d->inputContainer, SIGNAL(viewContentChanged()), this, SLOT(updateInput()));
//        connect(d->inputContainer, SIGNAL(viewRemoved()), this, SLOT(updateInput()));

//        this->tabbedViewContainers()->lockTabs();
//        this->tabbedViewContainers()->hideTabBar();
//        d->inputContainer->setSelected(true);
//        d->outputContainer->setSelected(false);
//    }
}


void medRegistrationWorkspace::setupApplicationProcess(QString process)
{
    medAbstractProcess *temp = d->applyTransfoProcess;
    d->applyTransfoProcess = dynamic_cast<medAbstractApplyTransformationProcess*>(dtkAbstractProcessFactory::instance()->create(process));
    if(d->applyTransfoProcess)
    {
        d->applyTransfoToolBox->setProcessToolbox(d->applyTransfoProcess->toolbox());
        connect(d->applyTransfoProcess->runParameter(), SIGNAL(triggered()), this, SLOT(startApplicationProcess()));
        this->tabbedViewContainers()->setSplitter(0, d->applyTransfoProcess->viewContainerSplitter());
        d->currentSplitter = d->applyTransfoProcess->viewContainerSplitter();
    }

    if(d->applyTransfoProcess && temp)
    {
        d->applyTransfoProcess->retrieveInputs(temp);
    }

    delete temp;
}

void medRegistrationWorkspace::setupEstimationProcess(QString process)
{
    medAbstractProcess *temp = d->estimateTransfoProcess;
    d->estimateTransfoProcess = dynamic_cast<medAbstractEstimateTransformationProcess*>(dtkAbstractProcessFactory::instance()->create(process));
    if(d->estimateTransfoProcess)
    {
        d->estimateTransfoToolBox->setProcessToolbox(d->estimateTransfoProcess->toolbox());
        connect(d->estimateTransfoProcess->runParameter(), SIGNAL(triggered()), this, SLOT(startEstimationProcess()));
        this->tabbedViewContainers()->setSplitter(0, d->estimateTransfoProcess->viewContainerSplitter());
        d->currentSplitter = d->estimateTransfoProcess->viewContainerSplitter();
    }

    if(d->estimateTransfoProcess && temp)
    {
        d->estimateTransfoProcess->retrieveInputs(temp);
    }

    delete temp;
}


bool medRegistrationWorkspace::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("filtering").size()!=0);
}

void medRegistrationWorkspace::startApplicationProcess()
{
    if(!d->applyTransfoProcess)
        return;

    d->applyTransfoToolBox->setEnabled(false);

    medRunnableProcess *runProcess = new medRunnableProcess(d->applyTransfoProcess, d->applyTransfoProcess->name());
    connect (runProcess, SIGNAL (success()),this,SLOT(enableSelectorToolBox()));
    connect (runProcess, SIGNAL (failure()),this,SLOT(enableSelectorToolBox()));

    runProcess->start();
}

void medRegistrationWorkspace::startEstimationProcess()
{
    if(!d->estimateTransfoProcess)
        return;

    d->estimateTransfoToolBox->setEnabled(false);

    medRunnableProcess *runProcess = new medRunnableProcess(d->estimateTransfoProcess, d->estimateTransfoProcess->name());
    connect (runProcess, SIGNAL (success()),this,SLOT(enableSelectorToolBox()));
    connect (runProcess, SIGNAL (success()),this,SLOT(applyTransformation()));
    connect (runProcess, SIGNAL (failure()),this,SLOT(enableSelectorToolBox()));

    runProcess->start();
}

void medRegistrationWorkspace::enableSelectorToolBox()
{
    d->applyTransfoToolBox->setEnabled(true);
    d->estimateTransfoToolBox->setEnabled(true);
}

void medRegistrationWorkspace::applyTransformation()
{
    if(d->currentSplitter != d->applyTransfoProcess->viewContainerSplitter())
    {
        this->tabbedViewContainers()->setSplitter(0, d->applyTransfoProcess->viewContainerSplitter());
        d->currentSplitter = d->applyTransfoProcess->viewContainerSplitter();
    }

    medAbstractTransformation *transfo = d->estimateTransfoProcess->output<medAbstractTransformation>(0);

    d->applyTransfoProcess->retrieveInputs(d->estimateTransfoProcess);

    d->applyTransfoProcess->addTransformation(transfo);
}
