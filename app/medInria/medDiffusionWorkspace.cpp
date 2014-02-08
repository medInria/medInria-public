/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDiffusionWorkspace.h"

#include <dtkCore/dtkSmartPointer.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include <medAbstractView.h>
#include <medAbstractDataImage.h>
#include <medDataManager.h>

#include <medViewContainer.h>
#include <medSingleViewContainer.h>
#include <medTabbedViewContainers.h>
#include <medToolBox.h>
#include <medToolBoxFactory.h>

#include <medRunnableProcess.h>
#include <medJobManager.h>
#include <medMessageController.h>

class medDiffusionWorkspacePrivate
{
public:

    medToolBox *fiberBundlingToolBox;
	medViewContainer * diffusionContainer;

    medDiffusionSelectorToolBox *diffusionEstimationToolBox;
    medDiffusionSelectorToolBox *diffusionScalarMapsToolBox;
    medDiffusionSelectorToolBox *diffusionTractographyToolBox;

    dtkSmartPointer <dtkAbstractProcess> currentProcess;
    bool processRunning;
};

medDiffusionWorkspace::medDiffusionWorkspace(QWidget *parent) : medWorkspace(parent), d(new medDiffusionWorkspacePrivate)
{
    d->diffusionContainer = 0;

    // -- Bundling  toolbox --
    d->fiberBundlingToolBox = medToolBoxFactory::instance()->createToolBox("medFiberBundlingToolBox", parent);
    
    // -- Estimation toolbox --
    d->diffusionEstimationToolBox = new medDiffusionSelectorToolBox(parent,medDiffusionSelectorToolBox::Estimation);
    d->diffusionEstimationToolBox->setTitle("Model Estimation");

    connect(d->diffusionEstimationToolBox, SIGNAL(addToolBox(medToolBox *)),
            this, SLOT(addToolBox(medToolBox *)));
    connect(d->diffusionEstimationToolBox, SIGNAL(removeToolBox(medToolBox *)),
            this, SLOT(removeToolBox(medToolBox *)));
    
    // -- Scalar maps toolbox --
    d->diffusionScalarMapsToolBox = new medDiffusionSelectorToolBox(parent,medDiffusionSelectorToolBox::ScalarMaps);
    d->diffusionScalarMapsToolBox->setTitle("Diffusion Scalar Maps");
    
    connect(d->diffusionScalarMapsToolBox, SIGNAL(addToolBox(medToolBox *)),
            this, SLOT(addToolBox(medToolBox *)));
    connect(d->diffusionScalarMapsToolBox, SIGNAL(removeToolBox(medToolBox *)),
            this, SLOT(removeToolBox(medToolBox *)));
    
    // -- Tractography toolbox --
    d->diffusionTractographyToolBox = new medDiffusionSelectorToolBox(parent,medDiffusionSelectorToolBox::Tractography);
    d->diffusionTractographyToolBox->setTitle("Tractography");
    
    connect(d->diffusionTractographyToolBox, SIGNAL(addToolBox(medToolBox *)),
            this, SLOT(addToolBox(medToolBox *)));
    connect(d->diffusionTractographyToolBox, SIGNAL(removeToolBox(medToolBox *)),
            this, SLOT(removeToolBox(medToolBox *)));

    // -- View toolboxes --

    QList<QString> toolboxNames = medToolBoxFactory::instance()->toolBoxesFromCategory("view");
    if(toolboxNames.contains("medViewPropertiesToolBox"))
    {
        // we want the medViewPropertiesToolBox to be the first "view" toolbox
        toolboxNames.move(toolboxNames.indexOf("medViewPropertiesToolBox"),0);
    }
    foreach(QString toolbox, toolboxNames)
    {
       addToolBox( medToolBoxFactory::instance()->createToolBox(toolbox, parent) );
    }

    connect(d->diffusionEstimationToolBox, SIGNAL(processRequested(QString, QString)), this, SLOT(runProcess(QString, QString)));
    connect(d->diffusionScalarMapsToolBox, SIGNAL(processRequested(QString, QString)), this, SLOT(runProcess(QString, QString)));
    connect(d->diffusionTractographyToolBox, SIGNAL(processRequested(QString, QString)), this, SLOT(runProcess(QString, QString)));
    
    connect(d->diffusionEstimationToolBox, SIGNAL(processCancelled()), this, SLOT(cancelProcess()));
    connect(d->diffusionTractographyToolBox, SIGNAL(processCancelled()), this, SLOT(cancelProcess()));
    
    this->addToolBox( d->diffusionEstimationToolBox );
    this->addToolBox( d->diffusionScalarMapsToolBox );
    this->addToolBox( d->diffusionTractographyToolBox );
    this->addToolBox( d->fiberBundlingToolBox );
    
    d->processRunning = false;
}

medDiffusionWorkspace::~medDiffusionWorkspace()
{
    delete d;
    d = NULL;
}

QString medDiffusionWorkspace::identifier() const {
    return "Diffusion";
}

QString medDiffusionWorkspace::description() const {
    return "Diffusion";
}

void medDiffusionWorkspace::setupViewContainerStack()
{
    //the stack has been instantiated in constructor
    if ( ! this->stackedViewContainers()->count())
    {
        medSingleViewContainer *singleViewContainer = new medSingleViewContainer ();

        //ownership of singleViewContainer is transferred to the stackedWidget.
        this->stackedViewContainers()->addContainer (identifier(), singleViewContainer);
        d->diffusionContainer = singleViewContainer;
        
        connect(singleViewContainer,SIGNAL(viewRemoved(dtkAbstractView *)),this,SLOT(resetToolBoxesInputs(dtkAbstractView *)));
        connect(singleViewContainer,SIGNAL(viewAdded(dtkAbstractView *)),this,SLOT(connectCurrentViewSignals(dtkAbstractView *)));

        connect(this->stackedViewContainers(),SIGNAL(currentChanged(QString)),this,SLOT(changeCurrentContainer(QString)));
    }
}

void medDiffusionWorkspace::runProcess(QString processName, QString category)
{
    if (d->processRunning)
        return;
    
    medRunnableProcess *runProcess = new medRunnableProcess;
    
    medDiffusionSelectorToolBox * originToolbox = dynamic_cast <medDiffusionSelectorToolBox *> (this->sender());
    
    if (!originToolbox)
        return;
    
    d->currentProcess = dtkAbstractProcessFactory::instance()->create(processName);
    originToolbox->setProcessParameters(d->currentProcess);
    
    runProcess->setProcess(d->currentProcess);
    
    d->processRunning = true;
    this->stackedViewContainers()->setEnabled(false);
    
    medJobManager::instance()->registerJobItem(runProcess);
    connect(runProcess, SIGNAL(success(QObject*)), this, SLOT(getOutput()));
    connect(runProcess, SIGNAL(failure(QObject*)), this, SLOT(resetRunningFlags()));
    
    medMessageProgress *messageProgress = medMessageController::instance()->showProgress(category);
    
    messageProgress->setProgress(0);
    connect(runProcess, SIGNAL(progressed(int)), messageProgress, SLOT(setProgress(int)));
    connect(runProcess, SIGNAL(success(QObject*)), messageProgress, SLOT(success()));
    connect(runProcess, SIGNAL(failure(QObject*)), messageProgress, SLOT(failure()));
    
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));
}

void medDiffusionWorkspace::cancelProcess()
{
    d->currentProcess->cancel();
    this->resetRunningFlags();
}

void medDiffusionWorkspace::getOutput()
{
    this->stackedViewContainers()->setEnabled(true);
    dtkSmartPointer <dtkAbstractData> outputData = d->currentProcess->output();
    
    if (!outputData)
        return;
    
    if (!d->diffusionContainer->view())
    {
        d->diffusionContainer->open(outputData);
    }
    else
    {
        d->diffusionContainer->view()->setData(outputData, 0);
        d->diffusionContainer->view()->reset();
        d->diffusionContainer->view()->update();
    }
    
    QString uuid = QUuid::createUuid().toString();
    medDataManager::instance()->importNonPersistent (outputData, uuid);
    
    this->resetRunningFlags();
}

void medDiffusionWorkspace::resetRunningFlags()
{
    d->processRunning = false;
    this->stackedViewContainers()->setEnabled(true);
    
    d->diffusionEstimationToolBox->resetButtons();
    d->diffusionTractographyToolBox->resetButtons();
}

void medDiffusionWorkspace::onAddTabClicked()
{
    QString name = this->identifier();
    QString realName = this->addSingleContainer(name);
    this->stackedViewContainers()->setContainer(realName);
    
    disconnect(d->diffusionContainer,SIGNAL(viewRemoved(dtkAbstractView *)),this,SLOT(resetToolBoxesInputs(dtkAbstractView *)));
    disconnect(d->diffusionContainer,SIGNAL(viewAdded(dtkAbstractView *)),this,SLOT(connectCurrentViewSignals(dtkAbstractView *)));
    this->disconnectCurrentViewSignals(d->diffusionContainer->view());
    
    d->diffusionContainer = this->stackedViewContainers()->container(realName);
    
    connect(d->diffusionContainer,SIGNAL(viewRemoved(dtkAbstractView *)),this,SLOT(resetToolBoxesInputs(dtkAbstractView *)));
    connect(d->diffusionContainer,SIGNAL(viewAdded(dtkAbstractView *)),this,SLOT(connectCurrentViewSignals(dtkAbstractView *)));
}

void medDiffusionWorkspace::changeCurrentContainer(QString name)
{
    // This cannot happen while a process is running, container stack is disabled at that point
    
    // For security, disconnect current connections
    disconnect(d->diffusionContainer,SIGNAL(viewRemoved(dtkAbstractView *)),this,SLOT(resetToolBoxesInputs(dtkAbstractView *)));
    disconnect(d->diffusionContainer,SIGNAL(viewAdded(dtkAbstractView *)),this,SLOT(connectCurrentViewSignals(dtkAbstractView *)));
    this->disconnectCurrentViewSignals(d->diffusionContainer->view());

    // Now connect new container
    d->diffusionContainer = this->stackedViewContainers()->container(name);
    connect(d->diffusionContainer,SIGNAL(viewRemoved(dtkAbstractView *)),this,SLOT(resetToolBoxesInputs(dtkAbstractView *)));
    connect(d->diffusionContainer,SIGNAL(viewAdded(dtkAbstractView *)),this,SLOT(connectCurrentViewSignals(dtkAbstractView *)));
    
    d->diffusionEstimationToolBox->clearInput();
    d->diffusionScalarMapsToolBox->clearInput();
    d->diffusionTractographyToolBox->clearInput();
    
    this->resetToolBoxesInputs(d->diffusionContainer->view());
    this->connectCurrentViewSignals(d->diffusionContainer->view());
}

void medDiffusionWorkspace::connectCurrentViewSignals(dtkAbstractView *view)
{
    dtkSmartPointer <medAbstractView> medView = dynamic_cast <medAbstractView *> (view);
    
    if (!medView)
        return;
    
    unsigned int layerCount = medView->layerCount();
    for (unsigned int i = 0;i < layerCount;++i)
    {
        this->addToolBoxInput(medView->dataInList(i));
    }
    
    connect(medView,SIGNAL(dataAdded(dtkAbstractData *)),this,SLOT(addToolBoxInput(dtkAbstractData *)));
}

void medDiffusionWorkspace::disconnectCurrentViewSignals(dtkAbstractView *view)
{
    dtkSmartPointer <medAbstractView> medView = dynamic_cast <medAbstractView *> (view);
    
    if (!medView)
        return;
    
    disconnect(medView,SIGNAL(dataAdded(dtkAbstractData *)),this,SLOT(addToolBoxInput(dtkAbstractData *)));
}

void medDiffusionWorkspace::addToolBoxInput(dtkAbstractData *data)
{
    dtkSmartPointer <medAbstractDataImage> medData = dynamic_cast <medAbstractDataImage *> (data);
    if (!medData)
        return;
    
    if (medData->Dimension() == 4)
        d->diffusionEstimationToolBox->setInputImage(medData);
    
    if ((medData->identifier().startsWith("itkDataTensorImage"))||
        (medData->identifier().startsWith("itkDataSHImage")))
    {
        d->diffusionScalarMapsToolBox->setInputImage(medData);
        d->diffusionTractographyToolBox->setInputImage(medData);
    }
}

void medDiffusionWorkspace::resetToolBoxesInputs(dtkAbstractView *view)
{
    DTK_UNUSED(view);
    
    d->diffusionEstimationToolBox->clearInput();
    d->diffusionScalarMapsToolBox->clearInput();
    d->diffusionTractographyToolBox->clearInput();
}

bool medDiffusionWorkspace::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("diffusion").size()!=0); 
}
