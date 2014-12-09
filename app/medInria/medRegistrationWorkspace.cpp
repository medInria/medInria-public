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
#include <medTriggerParameter.h>
#include <medViewContainerSplitter.h>

class medRegistrationWorkspacePrivate
{
public:
    QPointer<medProcessSelectorToolBox> applyTransfoToolBox;
    medViewContainer *movingContainer;
    medViewContainer *fixedContainer;

    medAbstractData *movingData;
    medAbstractData *fixedData;

    dtkSmartPointer <medAbstractApplyTransformationProcess> process;
};

medRegistrationWorkspace::medRegistrationWorkspace(QWidget *parent): medAbstractWorkspace (parent), d(new medRegistrationWorkspacePrivate)
{
    d->movingData = NULL;
    d->fixedData = NULL;
    d->process = NULL;

    d->applyTransfoToolBox = new medProcessSelectorToolBox(parent);
    d->applyTransfoToolBox->setTitle("Apply transformation");

    this->addToolBox(d->applyTransfoToolBox);

    QStringList implementations = dtkAbstractProcessFactory::instance()->implementations("medAbstractApplyTransformationProcess");
    d->applyTransfoToolBox->setAvailableProcesses(implementations);

    medViewParameterGroup *viewGroup1 = new medViewParameterGroup("View Group 1", this, this->identifier());
    viewGroup1->setLinkAllParameters(true);
    viewGroup1->removeParameter("DataList");

    medLayerParameterGroup *layerGroup1 = new medLayerParameterGroup("Layer Group 1", this,  this->identifier());
    layerGroup1->setLinkAllParameters(true);

    connect(d->applyTransfoToolBox, SIGNAL(processSelected(QString)), this, SLOT(setupProcess(QString)));
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


void medRegistrationWorkspace::setupProcess(QString process)
{
    medAbstractProcess *temp = d->process;
    d->process = dynamic_cast<medAbstractApplyTransformationProcess*>(dtkAbstractProcessFactory::instance()->create(process));
    if(d->process)
    {
        d->applyTransfoToolBox->setProcessToolbox(d->process->toolbox());
        connect(d->process->runParameter(), SIGNAL(triggered()), this, SLOT(startProcess()));
        this->tabbedViewContainers()->setSplitter(0, d->process->viewContainerSplitter());
    }

    if(d->process && temp)
    {
        d->process->retrieveInputs(temp);
    }

    delete temp;
}

bool medRegistrationWorkspace::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("filtering").size()!=0);
}

void medRegistrationWorkspace::startProcess()
{
    if(!d->process)
        return;

    d->applyTransfoToolBox->setEnabled(false);

    medRunnableProcess *runProcess = new medRunnableProcess(d->process, d->process->name());
    connect (runProcess, SIGNAL (success()),this,SLOT(enableSelectorToolBox()));
    connect (runProcess, SIGNAL (failure()),this,SLOT(enableSelectorToolBox()));

    runProcess->start();
}

void medRegistrationWorkspace::enableSelectorToolBox()
{
    d->applyTransfoToolBox->setEnabled(true);
}
