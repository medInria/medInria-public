#include "medViewerConfigurationDiffusion.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>

#include <medCore/medDataManager.h>

#include <medGui/medViewContainer.h>
#include <medGui/medViewContainerSingle.h>
#include <medGui/medViewContainerStack.h>
#include <medGui/medViewerToolBoxView.h>
#include <medGui/medToolBoxDiffusion.h>
#include <medGui/medToolBoxDiffusionFiberView.h>
#include <medGui/medToolBoxDiffusionFiberBundling.h>


class medViewerConfigurationDiffusionPrivate
{
public:
    medViewerToolBoxView    *viewToolBox;
    medToolBoxDiffusionFiberView     *fiberViewToolBox;
    medToolBoxDiffusionFiberBundling *fiberBundlingToolBox;
    medToolBoxDiffusion     *diffusionToolBox;    
    
    QList<dtkAbstractView *> views;
};

medViewerConfigurationDiffusion::medViewerConfigurationDiffusion(QWidget *parent) : medViewerConfiguration(parent), d(new medViewerConfigurationDiffusionPrivate)
{
    // -- Layout toolbox --
    setLayoutToolBoxVisibility(false);

    // -- View toolbox --

    d->viewToolBox = new medViewerToolBoxView(parent);

    // -- Bundling  toolbox --
    
    d->fiberBundlingToolBox = new medToolBoxDiffusionFiberBundling(parent);
    
    // -- Diffusion toolbox --

    d->diffusionToolBox = new medToolBoxDiffusion(parent);

    connect(d->diffusionToolBox, SIGNAL(addToolBox(medToolBox *)),
            this, SLOT(addToolBox(medToolBox *)));
    connect(d->diffusionToolBox, SIGNAL(removeToolBox(medToolBox *)),
            this, SLOT(removeToolBox(medToolBox *)));
    
    // -- Fiber view tb --
    d->fiberViewToolBox = new medToolBoxDiffusionFiberView(parent);

    connect(d->fiberViewToolBox, SIGNAL(fiberColorModeChanged(int)), this, SLOT(onFiberColorModeChanged(int)));
    connect(d->fiberViewToolBox, SIGNAL(GPUActivated(bool)),         this, SLOT(onGPUActivated(bool)));
    connect(d->fiberViewToolBox, SIGNAL(lineModeSelected(bool)),     this, SLOT(onLineModeSelected(bool)));
    connect(d->fiberViewToolBox, SIGNAL(ribbonModeSelected(bool)),   this, SLOT(onRibbonModeSelected(bool)));
    connect(d->fiberViewToolBox, SIGNAL(tubeModeSelected(bool)),     this, SLOT(onTubeModeSelected(bool)));
    
    connect(d->fiberBundlingToolBox, SIGNAL(showBundles(bool)),                this, SLOT(onShowBundles(bool)));
    connect(d->fiberBundlingToolBox, SIGNAL(fiberSelectionValidated(QString)), this, SLOT(refreshInteractors()));
    
    connect(d->diffusionToolBox, SIGNAL(success()),                  this, SLOT(onTBDiffusionSuccess()));

    this->addToolBox( d->viewToolBox );
    this->addToolBox( d->fiberViewToolBox );
    this->addToolBox( d->diffusionToolBox );
    this->addToolBox( d->fiberBundlingToolBox );
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

void medViewerConfigurationDiffusion::setupViewContainerStack()
{
    qDebug() << "ConfigurationDiffusionSetupViewContainerStack";
    
    d->views.clear();
    medViewContainer * diffusionContainer;
    //the stack has been instantiated in constructor
    if (!this->stackedViewContainers()->count())
    {
        medViewContainerSingle *single = new medViewContainerSingle ();
        connect (single, SIGNAL (viewAdded (dtkAbstractView*)),   this, SLOT (onViewAdded (dtkAbstractView*)));
        connect (single, SIGNAL (viewRemoved (dtkAbstractView*)), this, SLOT (onViewRemoved (dtkAbstractView*)));
        //ownership of single is transferred to the stackedWidget.
        this->stackedViewContainers()->addContainer (description(), single);
        diffusionContainer = single;
    }
    else
    {
        diffusionContainer = this->stackedViewContainers()->container(description());
        //TODO: maybe clear views here too?
    }
    
    d->views << diffusionContainer->views();
    this->stackedViewContainers()->setContainer (description());
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
    
    // view->setData( d->diffusionToolBox->output(), 0 );
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

    medDataManager::instance()->importNonPersistent ( d->diffusionToolBox->output() );
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
