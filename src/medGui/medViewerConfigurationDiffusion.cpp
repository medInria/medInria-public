#include "medViewerConfigurationDiffusion.h"


#include "medViewerToolBoxView.h"
#include "medToolBoxDiffusion.h"
#include "medToolBoxFiberView.h"
#include "medToolBoxFiberBundling.h"
#include "medToolBoxTensor.h"
#include <medGui/medViewContainer.h>
#include <medGui/medViewContainerStack.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>

class medViewerConfigurationDiffusionPrivate
{
public:
    medViewerToolBoxView    *viewToolBox;
    medToolBoxFiberView     *fiberViewToolBox;
    medToolBoxFiberBundling *fiberBundlingToolBox;
    medToolBoxDiffusion     *diffusionToolBox;    
    medToolBoxTensor        *tensorToolBox;
    
    QList<dtkAbstractView *> views;
};

medViewerConfigurationDiffusion::medViewerConfigurationDiffusion(QWidget *parent) : medViewerConfiguration(parent), d(new medViewerConfigurationDiffusionPrivate)
{
    // -- Layout toolbox --
    setLayoutToolBoxVisibility(false);

    // -- View toolbox --

    d->viewToolBox = new medViewerToolBoxView(parent);

    // -- Bundling  toolbox --
    
    d->fiberBundlingToolBox = new medToolBoxFiberBundling(parent);
    
    // -- Diffusion toolbox --

    d->diffusionToolBox = new medToolBoxDiffusion(parent);

    connect(d->diffusionToolBox, SIGNAL(addToolBox(medToolBox *)),
            this, SLOT(addToolBox(medToolBox *)));
    connect(d->diffusionToolBox, SIGNAL(removeToolBox(medToolBox *)),
            this, SLOT(removeToolBox(medToolBox *)));
    
    // -- Tensor tb --
    d->tensorToolBox = new medToolBoxTensor(parent);

    // -- Fiber view tb --
    d->fiberViewToolBox = new medToolBoxFiberView(parent);

    connect(d->fiberViewToolBox, SIGNAL(fiberColorModeChanged(int)), this, SLOT(onFiberColorModeChanged(int)));
    connect(d->fiberViewToolBox, SIGNAL(GPUActivated(bool)),         this, SLOT(onGPUActivated(bool)));
    connect(d->fiberViewToolBox, SIGNAL(lineModeSelected(bool)),     this, SLOT(onLineModeSelected(bool)));
    connect(d->fiberViewToolBox, SIGNAL(ribbonModeSelected(bool)),   this, SLOT(onRibbonModeSelected(bool)));
    connect(d->fiberViewToolBox, SIGNAL(tubeModeSelected(bool)),     this, SLOT(onTubeModeSelected(bool)));
    
    connect(d->fiberBundlingToolBox, SIGNAL(showBundles(bool)),                this, SLOT(onShowBundles(bool)));
    connect(d->fiberBundlingToolBox, SIGNAL(fiberSelectionValidated(QString)), this, SLOT(refreshInteractors()));
    
    connect(d->diffusionToolBox, SIGNAL(success()),                  this, SLOT(onTBDiffusionSuccess()));

    this->addToolBox( d->viewToolBox );
    this->addToolBox( d->tensorToolBox );
    this->addToolBox( d->fiberViewToolBox );
    this->addToolBox( d->diffusionToolBox );
    this->addToolBox( d->fiberBundlingToolBox );


    this->setViewLayoutType (-1); // single
}

medViewerConfigurationDiffusion::~medViewerConfigurationDiffusion(void)
{
    delete d;
    d = NULL;
}

QString medViewerConfigurationDiffusion::description(void) const
{
    return "Diffusion";
}

void medViewerConfigurationDiffusion::setupViewContainerStack(medViewContainerStack *container_stack)
{
    if (!container_stack) {
        return;
    }

    d->views.clear();
    
    medViewContainer *diffusionContainer = container_stack->customContainer("Diffusion");

    if (!diffusionContainer) {
      medViewContainerCustom *custom = new medViewContainerCustom (container_stack);
      custom->setPreset (medViewContainerCustom::E);
      connect (custom, SIGNAL (viewAdded (dtkAbstractView*)),   this, SLOT (onViewAdded (dtkAbstractView*)));
      connect (custom, SIGNAL (viewRemoved (dtkAbstractView*)), this, SLOT (onViewRemoved (dtkAbstractView*)));
      diffusionContainer = custom;

      container_stack->addCustomContainer ("Diffusion", diffusionContainer);
    }

    d->views << diffusionContainer->views();
    
    container_stack->setCustomContainer ("Diffusion");
}

void medViewerConfigurationDiffusion::patientChanged(int patientId)
{
    d->diffusionToolBox->clear();
    d->fiberBundlingToolBox->clear();
}

void medViewerConfigurationDiffusion::onViewAdded (dtkAbstractView *view)
{
    if (!view)
        return;

    if (d->views.contains (view))
        return;

    d->views.append (view);

    view->enableInteractor ("v3dViewFiberInteractor");

    if (dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewFiberInteractor"))
        connect(d->fiberViewToolBox, SIGNAL(fiberRadiusSet(int)), interactor, SLOT(onRadiusSet(int)));
    
    view->setData( d->diffusionToolBox->output(), 0 );
}

void medViewerConfigurationDiffusion::onViewRemoved (dtkAbstractView *view)
{
   if (!view)
        return;

   if (!d->views.contains (view))
      return;

   // view->disableInteractor ("v3dViewFiberInteractor");
   if (dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewFiberInteractor"))
        disconnect(d->fiberViewToolBox, SIGNAL(fiberRadiusSet(int)), interactor, SLOT(onRadiusSet(int)));

   d->views.removeOne (view);
}

void medViewerConfigurationDiffusion::onFiberColorModeChanged(int index)
{
    foreach (dtkAbstractView *view, d->views) {
        if (dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewFiberInteractor")) {
            if (index==0)
                interactor->setProperty("ColorMode","local");
            if (index==1)
                interactor->setProperty("ColorMode","global");
            if (index==2)
                interactor->setProperty("ColorMode","fa");
            
            view->update();
        }
    }
}

void medViewerConfigurationDiffusion::onGPUActivated (bool value)
{
    foreach (dtkAbstractView *view, d->views) {
        if (dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewFiberInteractor")) {
            if (value)
                interactor->setProperty ("GPUMode", "true");
            else
                interactor->setProperty ("GPUMode", "false");
            
            view->update();
        }
    }
}

void medViewerConfigurationDiffusion::onLineModeSelected (bool value)
{
    foreach (dtkAbstractView *view, d->views) {
        if (value)
            if(dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewFiberInteractor")) {
                interactor->setProperty ("RenderingMode", "lines");
                
                view->update();
            }
    }
}

void medViewerConfigurationDiffusion::onRibbonModeSelected (bool value)
{
    foreach (dtkAbstractView *view, d->views) {
        if (value)
            if(dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewFiberInteractor")) {
                interactor->setProperty ("RenderingMode", "ribbons");
                
                view->update();
            }
    }
}

void medViewerConfigurationDiffusion::onTubeModeSelected (bool value)
{
    foreach (dtkAbstractView *view, d->views) {
        if (value)
            if(dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewFiberInteractor")) {
                interactor->setProperty ("RenderingMode", "tubes");
                
                view->update();
            }
    }
}

void medViewerConfigurationDiffusion::onShowBundles (bool value)
{
    foreach (dtkAbstractView *view, d->views) {
      if(dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewFiberInteractor")) {
          if (value)
              interactor->enable();
          else
              interactor->disable();
          
          view->update();
      }
    }
}

void medViewerConfigurationDiffusion::onTBDiffusionSuccess(void)
{
    foreach (dtkAbstractView *view, d->views) {
      view->setData( d->diffusionToolBox->output(), 0 );
        view->reset();
        view->update();
    }
    
    if (d->diffusionToolBox->output()->description()=="v3dDataFibers")
      d->fiberBundlingToolBox->setData( d->diffusionToolBox->output() );
}

void medViewerConfigurationDiffusion::refreshInteractors (void)
{
    foreach (dtkAbstractView *view, d->views) {
      if(dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewFiberInteractor")) {
	  // interactor->update(); // TO BE IMPLEMENTED
          view->update();
      }
    }
}

medViewerConfiguration *createMedViewerConfigurationDiffusion(void)
{
    return new medViewerConfigurationDiffusion;
}
