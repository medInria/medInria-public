/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDiffusionWorkspace.h>

#include <dtkCore/dtkSmartPointer.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include <medAbstractView.h>
#include <medAbstractDataImage.h>
#include <medDataManager.h>

#include <medViewContainer.h>
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

medDiffusionWorkspace::medDiffusionWorkspace(QWidget *parent) : medAbstractWorkspace(parent), d(new medDiffusionWorkspacePrivate)
{
//    d->diffusionContainer = 0;

//    d->diffusionToolBox =  new medDiffusionSelectorToolBox(parent);

//    d->fiberBundlingToolBox = medToolBoxFactory::instance()->createToolBox("medFiberBundlingToolBox", parent);

//    connect(d->diffusionToolBox, SIGNAL(addToolBox(medToolBox *)),
//            this, SLOT(addToolBox(medToolBox *)));
//    connect(d->diffusionToolBox, SIGNAL(removeToolBox(medToolBox *)),
//            this, SLOT(removeToolBox(medToolBox *)));

//    connect(d->diffusionToolBox, SIGNAL(newOutput(medAbstractData*)), d->fiberBundlingToolBox, SLOT(setInput(medAbstractData*)));
//    connect(d->diffusionToolBox, SIGNAL(newOutput(medAbstractData*)), this, SLOT(addToView(medAbstractData*)));

//    // -- View toolboxes --

//    QList<QString> toolboxNames = medToolBoxFactory::instance()->toolBoxesFromCategory("view");
//    if(toolboxNames.contains("medViewPropertiesToolBox"))
//    {
//        // we want the medViewPropertiesToolBox to be the first "view" toolbox
//        toolboxNames.move(toolboxNames.indexOf("medViewPropertiesToolBox"),0);
//    }
//    foreach(QString toolbox, toolboxNames)
//    {
//       this->addWorkspaceToolBox(medToolBoxFactory::instance()->createToolBox(toolbox, parent));
//    }

//    this->addWorkspaceToolBox( d->diffusionToolBox );
//    this->addWorkspaceToolBox( d->fiberBundlingToolBox );

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
//    //the stack has been instantiated in constructor
//    if ( ! this->stackedViewContainers()->count())
//    {
//        medViewContainer *viewContainer = new medViewContainer();

//        //ownership of singleViewContainer is transferred to the stackedWidget.
//        this->stackedViewContainers()->addContainerInTab (identifier());

//        d->diffusionContainer = viewContainer;

//        this->stackedViewContainers()->lockTabs();
//        this->stackedViewContainers()->hideTabBar();
//    }
//    else
//    {
//        d->diffusionContainer = this->currentViewContainer();
//        //TODO: maybe clear views here too?
//    }

//    if ( ! d->diffusionContainer)
//        return;
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
    bool workspaceUsable = (tbFactory->toolBoxesFromCategory("diffusion-estimation").size()!=0)||
                           (tbFactory->toolBoxesFromCategory("diffusion-scalar-maps").size()!=0)||
                           (tbFactory->toolBoxesFromCategory("diffusion-tractography").size()!=0);
    
    return workspaceUsable;
}
