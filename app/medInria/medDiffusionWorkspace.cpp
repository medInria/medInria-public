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
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>

#include <medDataManager.h>

#include <medViewContainer.h>
#include <medSingleViewContainer.h>
#include <medTabbedViewContainers.h>
#include <medToolBox.h>
#include <medToolBoxFactory.h>
#include <medDiffusionSelectorToolBox.h>

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
    connect(this, SIGNAL(newOutput(dtkAbstractData*)), d->fiberBundlingToolBox, SLOT(setInput(dtkAbstractData*)));

    
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

    connect(d->diffusionEstimationToolBox, SIGNAL(processCreated(dtkAbstractProcess *, QString)),
            this, SLOT(runProcess(dtkAbstractProcess *, QString)));
    connect(d->diffusionScalarMapsToolBox, SIGNAL(processCreated(dtkAbstractProcess *, QString)),
            this, SLOT(runProcess(dtkAbstractProcess *, QString)));
    connect(d->diffusionTractographyToolBox, SIGNAL(processCreated(dtkAbstractProcess *, QString)),
            this, SLOT(runProcess(dtkAbstractProcess *, QString)));

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

        connect(this->stackedViewContainers(),SIGNAL(currentChanged(QString)),this,SLOT(changeCurrentContainer(QString)));
    }
}

void medDiffusionWorkspace::runProcess(dtkAbstractProcess *process, QString category)
{
    if (d->processRunning)
        return;
    
    d->currentProcess = process;
    d->processRunning = true;
    
    medRunnableProcess *runProcess = new medRunnableProcess;
    runProcess->setProcess (d->currentProcess);
    
    medJobManager::instance()->registerJobItem(runProcess);
    connect(runProcess, SIGNAL(success(QObject*)), this, SLOT(getOutput()));
    
    medMessageProgress *messageProgress = medMessageController::instance()->showProgress(category);
    
    messageProgress->setProgress(0);
    connect(runProcess, SIGNAL(progressed(int)), messageProgress, SLOT(setProgress(int)));
    connect(runProcess, SIGNAL(success(QObject*)), messageProgress, SLOT(success()));
    
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));
}

void medDiffusionWorkspace::getOutput()
{
    if (!d->currentProcess->output())
        return;
    
    if (!d->diffusionContainer->view())
    {
        d->diffusionContainer->open(d->currentProcess->output());
    }
    else
    {
        d->diffusionContainer->view()->setData(d->currentProcess->output(), 0);
        d->diffusionContainer->view()->reset();
        d->diffusionContainer->view()->update();
    }
    
    emit newOutput(d->currentProcess->output());
    
    QString uuid = QUuid::createUuid().toString();
    medDataManager::instance()->importNonPersistent ( d->currentProcess->output(), uuid);
    
    d->processRunning = false;
}

void medDiffusionWorkspace::onAddTabClicked()
{
    QString name = this->identifier();
    QString realName = this->addSingleContainer(name);
    this->stackedViewContainers()->setContainer(realName);
    d->diffusionContainer = this->stackedViewContainers()->container(realName);
}

void medDiffusionWorkspace::changeCurrentContainer(QString name)
{
    d->diffusionContainer = this->stackedViewContainers()->container(name);
}

bool medDiffusionWorkspace::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("diffusion").size()!=0); 
}
