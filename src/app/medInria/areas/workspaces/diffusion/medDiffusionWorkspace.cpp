/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDiffusionWorkspace.h"

#include <medAbstractLayeredView.h>
#include <medAbstractImageData.h>
#include <medAbstractDiffusionModelImageData.h>
#include <medDataManager.h>

#include <medViewContainer.h>
#include <medViewContainerManager.h>
#include <medTabbedViewContainers.h>

#include <medToolBoxFactory.h>

class medDiffusionWorkspacePrivate
{
public:
    medToolBox *fiberBundlingToolBox;
    QPointer<medViewContainer> diffusionContainer;

    medDiffusionSelectorToolBox *diffusionEstimationToolBox;
    medDiffusionSelectorToolBox *diffusionScalarMapsToolBox;
    medDiffusionSelectorToolBox *diffusionTractographyToolBox;

    bool processRunning;
};

medDiffusionWorkspace::medDiffusionWorkspace(QWidget *parent) : medAbstractWorkspaceLegacy(parent), d(new medDiffusionWorkspacePrivate)
{
    d->diffusionContainer = nullptr;

    // -- Bundling toolbox --
    d->fiberBundlingToolBox = medToolBoxFactory::instance()->createToolBox("medFiberBundlingToolBox", parent);

    // -- Estimation toolbox --
    d->diffusionEstimationToolBox = new medDiffusionSelectorToolBox(parent,medDiffusionSelectorToolBox::Estimation);
    d->diffusionEstimationToolBox->setTitle("Model Estimation");

    connect(d->diffusionEstimationToolBox,SIGNAL(jobRunning(bool)),this,SLOT(updateRunningFlags(bool)));
    connect(d->diffusionEstimationToolBox,SIGNAL(jobFinished(medAbstractJob::medJobExitStatus)),this,SLOT(getEstimationOutput(medAbstractJob::medJobExitStatus)));

    // -- Scalar maps toolbox --
    d->diffusionScalarMapsToolBox = new medDiffusionSelectorToolBox(parent,medDiffusionSelectorToolBox::ScalarMaps);
    d->diffusionScalarMapsToolBox->setTitle("Diffusion Scalar Maps");

    connect(d->diffusionScalarMapsToolBox,SIGNAL(jobRunning(bool)),this,SLOT(updateRunningFlags(bool)));
    connect(d->diffusionScalarMapsToolBox,SIGNAL(jobFinished(medAbstractJob::medJobExitStatus)),this,SLOT(getScalarMapsOutput(medAbstractJob::medJobExitStatus)));

    // -- Tractography toolbox --
    d->diffusionTractographyToolBox = new medDiffusionSelectorToolBox(parent,medDiffusionSelectorToolBox::Tractography);
    d->diffusionTractographyToolBox->setTitle("Tractography");

    connect(d->diffusionTractographyToolBox,SIGNAL(jobRunning(bool)),this,SLOT(updateRunningFlags(bool)));
    connect(d->diffusionTractographyToolBox,SIGNAL(jobFinished(medAbstractJob::medJobExitStatus)),this,SLOT(getTractographyOutput(medAbstractJob::medJobExitStatus)));

    // -- View toolboxes --
    QList<QString> toolboxNames = medToolBoxFactory::instance()->toolBoxesFromCategory("view");
    for(QString toolbox : toolboxNames)
    {
       addToolBox( medToolBoxFactory::instance()->createToolBox(toolbox, parent) );
    }

    this->addToolBox( d->diffusionEstimationToolBox );
    this->addToolBox( d->diffusionScalarMapsToolBox );
    this->addToolBox( d->diffusionTractographyToolBox );

    d->processRunning = false;
}

medDiffusionWorkspace::~medDiffusionWorkspace()
{
    delete d;
    d = nullptr;
}

void medDiffusionWorkspace::setupTabbedViewContainer()
{
    //the stack has been instantiated in constructor
    if ( ! this->tabbedViewContainers()->count())
    {
        d->diffusionContainer = this->tabbedViewContainers()->addContainerInTabNamed(this->name());

        d->diffusionContainer->setClosingMode(medViewContainer::CLOSE_CONTAINER);
        d->diffusionContainer->setUserSplittable(false);
        d->diffusionContainer->setMultiLayered(true);

        connect (d->diffusionContainer,SIGNAL(viewContentChanged()), this, SLOT(updateToolBoxesInputs()));
        connect(this->tabbedViewContainers(),SIGNAL(containersSelectedChanged()),this,SLOT(changeCurrentContainer()));
    }
    this->tabbedViewContainers()->setKeepLeastOne(true);
}

void medDiffusionWorkspace::getEstimationOutput(medAbstractJob::medJobExitStatus status)
{
    if (status != medAbstractJob::MED_JOB_EXIT_SUCCESS)
        return;

    medAbstractData *outputData = d->diffusionEstimationToolBox->processOutput();

    if (!outputData)
        return;

    d->diffusionContainer->addData(outputData);
}

void medDiffusionWorkspace::getScalarMapsOutput(medAbstractJob::medJobExitStatus status)
{
    if (status != medAbstractJob::MED_JOB_EXIT_SUCCESS)
        return;

    medAbstractData *outputData = d->diffusionScalarMapsToolBox->processOutput();

    if (!outputData)
        return;

    d->diffusionContainer->addData(outputData);
}

void medDiffusionWorkspace::getTractographyOutput(medAbstractJob::medJobExitStatus status)
{
    if (status != medAbstractJob::MED_JOB_EXIT_SUCCESS)
        return;

    medAbstractData *outputData = d->diffusionTractographyToolBox->processOutput();

    if (!outputData)
        return;

    d->diffusionContainer->addData(outputData);
}

void medDiffusionWorkspace::updateRunningFlags(bool running)
{
    d->processRunning = running;
    this->tabbedViewContainers()->setEnabled(!running);

    QObject* senderToolbox = sender();
    if (senderToolbox != d->diffusionEstimationToolBox)
        d->diffusionEstimationToolBox->setEnabled(!running);

    d->diffusionScalarMapsToolBox->setEnabled(!running);
    d->diffusionTractographyToolBox->setEnabled(!running);
}

void medDiffusionWorkspace::changeCurrentContainer()
{
    QList <QUuid> containersSelectedList = this->tabbedViewContainers()->containersSelected();
    if (!containersSelectedList.size())
        return;

    // This cannot happen while a process is running, container stack is disabled at that point

    // For security, disconnect current connections
    if (!d->diffusionContainer.isNull())
        disconnect (d->diffusionContainer,SIGNAL(viewContentChanged()), this, SLOT(updateToolBoxesInputs()));

    // Now connect new container
    d->diffusionContainer = medViewContainerManager::instance().container(containersSelectedList.first());
    if (!d->diffusionContainer.isNull())
    {
        d->diffusionContainer->setClosingMode(medViewContainer::CLOSE_CONTAINER);
        d->diffusionContainer->setUserSplittable(false);
        d->diffusionContainer->setMultiLayered(true);

        connect (d->diffusionContainer,SIGNAL(viewContentChanged()), this, SLOT(updateToolBoxesInputs()));
    }

    this->resetToolBoxesInputs();
}

void medDiffusionWorkspace::updateToolBoxesInputs()
{
    medAbstractLayeredView *medView = dynamic_cast <medAbstractLayeredView *> (d->diffusionContainer->view());

    if (!medView)
        return;

    unsigned int layersCount = medView->layersCount();
    for (unsigned int i = 0;i < layersCount;++i)
        this->addToolBoxInput(medView->layerData(i));
}

void medDiffusionWorkspace::addToolBoxInput(medAbstractData *data)
{
    medAbstractImageData *medData = dynamic_cast <medAbstractImageData *> (data);
    if (!medData)
        return;

    if (medData->Dimension() == 4)
        d->diffusionEstimationToolBox->addInputImage(medData);

    if (dynamic_cast<medAbstractDiffusionModelImageData*>(medData) != nullptr)
    {
        d->diffusionScalarMapsToolBox->addInputImage(medData);
        d->diffusionTractographyToolBox->addInputImage(medData);
    }
}

void medDiffusionWorkspace::resetToolBoxesInputs()
{
    d->diffusionEstimationToolBox->clearInputs();
    d->diffusionScalarMapsToolBox->clearInputs();
    d->diffusionTractographyToolBox->clearInputs();

    this->updateToolBoxesInputs();
}

bool medDiffusionWorkspace::isUsable()
{
    return true;
}
