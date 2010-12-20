#include "medViewerConfigurationDiffusion.h"


#include "medViewerToolBoxView.h"
#include "medToolBoxDiffusion.h"
#include <medGui/medViewContainer.h>
#include <medGui/medViewContainerStack.h>

#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>

class medViewerConfigurationDiffusionPrivate
{
public:
    medViewerToolBoxView   *viewToolBox;
    medToolBoxDiffusion    *diffusionToolBox;
    
    QList<dtkAbstractView *> views;
};

medViewerConfigurationDiffusion::medViewerConfigurationDiffusion(QWidget *parent) : medViewerConfiguration(parent), d(new medViewerConfigurationDiffusionPrivate)
{
    // -- Layout toolbox --
    setLayoutToolBoxVisibility(false);

    // -- View toolbox --

    d->viewToolBox = new medViewerToolBoxView(parent);

    // -- Registration toolbox --

    d->diffusionToolBox = new medToolBoxDiffusion(parent);

    connect(d->diffusionToolBox, SIGNAL(addToolBox(medToolBox *)),
            this, SLOT(addToolBox(medToolBox *)));
    connect(d->diffusionToolBox, SIGNAL(removeToolBox(medToolBox *)),
            this, SLOT(removeToolBox(medToolBox *)));
    
    connect(d->diffusionToolBox, SIGNAL(fiberColorModeChanged(int)), this, SLOT(onFiberColorModeChanged(int)));
    connect(d->diffusionToolBox, SIGNAL(GPUActivated(bool)),         this, SLOT(onGPUActivated(bool)));
    connect(d->diffusionToolBox, SIGNAL(lineModeSelected(bool)),     this, SLOT(onLineModeSelected(bool)));
    connect(d->diffusionToolBox, SIGNAL(ribbonModeSelected(bool)),   this, SLOT(onRibbonModeSelected(bool)));
    connect(d->diffusionToolBox, SIGNAL(tubeModeSelected(bool)),     this, SLOT(onTubeModeSelected(bool)));
    connect(d->diffusionToolBox, SIGNAL(bundlingBoxActivated(bool)), this, SLOT(onBundlingBoxActivated(bool)));
    connect(d->diffusionToolBox, SIGNAL(showBundles(bool)),          this, SLOT(onShowBundles(bool)));
    connect(d->diffusionToolBox, SIGNAL(success()),                  this, SLOT(onTBDiffusionSuccess()));

    this->addToolBox( d->viewToolBox );
    this->addToolBox( d->diffusionToolBox );

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

    /*
    dtkAbstractView *view = 0;
    
    if (singleContainer->current()->view()) {
        view = singleContainer->current()->view();
    }
    else if (view = dtkAbstractViewFactory::instance()->create("v3dView")) {
        singleContainer->current()->setView (view);
    }

    if (!view || view==d->view)
        return;
    
    if (d->view) {
        disconnect(d->view, SIGNAL(closing()), this, SLOT(onViewClosing()));
        if (dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
            disconnect (d->diffusionToolBox, SIGNAL(fiberSelectionValidated(QString)), interactor, SLOT(onSelectionValidated(QString)));
            disconnect (d->diffusionToolBox, SIGNAL(fiberSelectionTagged()),    interactor, SLOT(onSelectionTagged()));
            disconnect (d->diffusionToolBox, SIGNAL(fiberSelectionReset()),     interactor, SLOT(onSelectionReset()));
            disconnect (d->diffusionToolBox, SIGNAL(fiberRadiusSet(int)),       interactor, SLOT(onRadiusSet(int)));            
        }
    }
    
    d->view = view;
    
    // d->diffusionToolBox->setView ( d->view );
    d->view->enableInteractor ("v3dViewFiberInteractor");
    if (dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
        connect (d->diffusionToolBox, SIGNAL(fiberSelectionValidated(QString)), interactor, SLOT(onSelectionValidated(QString)));
        connect (d->diffusionToolBox, SIGNAL(fiberSelectionTagged()),    interactor, SLOT(onSelectionTagged()));
        connect (d->diffusionToolBox, SIGNAL(fiberSelectionReset()),     interactor, SLOT(onSelectionReset()));
        connect (d->diffusionToolBox, SIGNAL(fiberRadiusSet(int)),       interactor, SLOT(onRadiusSet(int)));
    }
    
    connect(d->view, SIGNAL(closing()), this, SLOT(onViewClosing()));
    */
}

void medViewerConfigurationDiffusion::patientChanged(int patientId)
{
    d->diffusionToolBox->clear();
}

/*
void medViewerConfigurationDiffusion::onViewClosing(void)
{
    if (d->view) {
        disconnect(d->view, SIGNAL(closing()), this, SLOT(onViewClosing()));
        if (dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
            disconnect (d->diffusionToolBox, SIGNAL(fiberSelectionValidated(QString)), interactor, SLOT(onSelectionValidated(QString)));
            disconnect (d->diffusionToolBox, SIGNAL(fiberSelectionTagged()),    interactor, SLOT(onSelectionTagged()));
            disconnect (d->diffusionToolBox, SIGNAL(fiberSelectionReset()),     interactor, SLOT(onSelectionReset()));
            disconnect (d->diffusionToolBox, SIGNAL(fiberRadiusSet(int)),       interactor, SLOT(onRadiusSet(int)));            
        }
    }
}
*/

void medViewerConfigurationDiffusion::onViewAdded (dtkAbstractView *view)
{
    if (!view)
        return;

    if (d->views.contains (view))
        return;

    d->views.append (view);

    view->enableInteractor ("v3dViewFiberInteractor");
}

void medViewerConfigurationDiffusion::onViewRemoved (dtkAbstractView *view)
{
   if (!view)
        return;

   if (!d->views.contains (view))
      return;

   // view->disableInteractor ("v3dViewFiberInteractor");

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

void medViewerConfigurationDiffusion::onBundlingBoxActivated (bool value)
{
    foreach (dtkAbstractView *view, d->views) {
    if(dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewFiberInteractor"))
      if (view->property ("Orientation")=="3D") {
        if (value)
	  interactor->setProperty ("BoxVisibility", "true");
        else
	  interactor->setProperty ("BoxVisibility", "false");
        
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
        view->setData( d->diffusionToolBox->output() );
        view->reset();
        view->update();
    }
}

medViewerConfiguration *createMedViewerConfigurationDiffusion(void)
{
    return new medViewerConfigurationDiffusion;
}
