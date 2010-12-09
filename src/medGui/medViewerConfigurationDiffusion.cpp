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
    
    dtkAbstractView *view;
};

medViewerConfigurationDiffusion::medViewerConfigurationDiffusion(QWidget *parent) : medViewerConfiguration(parent), d(new medViewerConfigurationDiffusionPrivate)
{
    d->view = 0;
    
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
    connect(d->diffusionToolBox, SIGNAL(ribbonModeSelected(bool)),   this, SLOT(onLineModeSelected(bool)));
    connect(d->diffusionToolBox, SIGNAL(tubeModeSelected(bool)),     this, SLOT(onTubeModeSelected(bool)));
    connect(d->diffusionToolBox, SIGNAL(bundlingBoxActivated(bool)), this, SLOT(onBundlingBoxActivated(bool)));
    connect(d->diffusionToolBox, SIGNAL(success()),                  this, SLOT(onTBDiffusionSuccess()));

    this->addToolBox( d->viewToolBox );
    this->addToolBox( d->diffusionToolBox );

    this->setViewLayoutType (0); // single
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

void medViewerConfigurationDiffusion::setupViewContainerStack(medViewContainerStack *container)
{
    if (!container) {
        return;
    }
        
    dtkAbstractView *view = 0;
    
    medViewContainer *singleContainer = container->single();
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
            disconnect (interactor, SIGNAL(selectionValidated(QString)), d->diffusionToolBox, SLOT (addBundle(QString)));
            
            disconnect (d->diffusionToolBox, SIGNAL(fiberSelectionValidated()), interactor, SLOT(onSelectionValidated()));
            disconnect (d->diffusionToolBox, SIGNAL(fiberSelectionTagged()),    interactor, SLOT(onSelectionTagged()));
            disconnect (d->diffusionToolBox, SIGNAL(fiberSelectionReset()),     interactor, SLOT(onSelectionReset()));
            disconnect (d->diffusionToolBox, SIGNAL(fiberRadiusSet(int)),       interactor, SLOT(onRadiusSet(int)));            
        }
    }
    
    d->view = view;
    
    // d->diffusionToolBox->setView ( d->view );
    d->view->enableInteractor ("v3dViewFiberInteractor");
    if (dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
        connect (interactor, SIGNAL(selectionValidated(QString)), d->diffusionToolBox, SLOT (addBundle(QString)));
        
        connect (d->diffusionToolBox, SIGNAL(fiberSelectionValidated()), interactor, SLOT(onSelectionValidated()));
        connect (d->diffusionToolBox, SIGNAL(fiberSelectionTagged()),    interactor, SLOT(onSelectionTagged()));
        connect (d->diffusionToolBox, SIGNAL(fiberSelectionReset()),     interactor, SLOT(onSelectionReset()));
        connect (d->diffusionToolBox, SIGNAL(fiberRadiusSet(int)),       interactor, SLOT(onRadiusSet(int)));
    }
    
    connect(d->view, SIGNAL(closing()), this, SLOT(onViewClosing()));
}

void medViewerConfigurationDiffusion::patientChanged(int patientId)
{
    d->diffusionToolBox->clear();
}

void medViewerConfigurationDiffusion::onViewClosing(void)
{
    if (d->view) {
        qDebug() << "Fuck the view";
        
        disconnect(d->view, SIGNAL(closing()), this, SLOT(onViewClosing()));
        if (dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
            disconnect (interactor, SIGNAL(selectionValidated(QString)), d->diffusionToolBox, SLOT (addBundle(QString)));
            
            disconnect (d->diffusionToolBox, SIGNAL(fiberSelectionValidated()), interactor, SLOT(onSelectionValidated()));
            disconnect (d->diffusionToolBox, SIGNAL(fiberSelectionTagged()),    interactor, SLOT(onSelectionTagged()));
            disconnect (d->diffusionToolBox, SIGNAL(fiberSelectionReset()),     interactor, SLOT(onSelectionReset()));
            disconnect (d->diffusionToolBox, SIGNAL(fiberRadiusSet(int)),       interactor, SLOT(onRadiusSet(int)));            
        }
    }
}

void medViewerConfigurationDiffusion::onFiberColorModeChanged(int index)
{
    if (!d->view)
        return;
    
    if (dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
        if (index==0)
            interactor->setProperty("ColorMode","local");
        if (index==1)
            interactor->setProperty("ColorMode","global");
        if (index==2)
            interactor->setProperty("ColorMode","fa");
        
        d->view->update();
    }
}

void medViewerConfigurationDiffusion::onGPUActivated (bool value)
{
    if (!d->view)
        return;
    
    if (dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
        if (value)
            interactor->setProperty ("GPUMode", "true");
        else
            interactor->setProperty ("GPUMode", "false");
        
        d->view->update();
    }
}

void medViewerConfigurationDiffusion::onLineModeSelected (bool value)
{
    if (!d->view)
        return;
    
    if (value)
        if(dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
            interactor->setProperty ("RenderingMode", "lines");
            
            d->view->update();
        }
}

void medViewerConfigurationDiffusion::onRibbonModeSelected (bool value)
{
    if (!d->view)
        return;
    
    if (value)
        if(dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
            interactor->setProperty ("RenderingMode", "ribbons");
            
            d->view->update();
        }
}

void medViewerConfigurationDiffusion::onTubeModeSelected (bool value)
{
    if (!d->view)
        return;
    
    if (value)
        if(dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
            interactor->setProperty ("RenderingMode", "tubes");
            
            d->view->update();
        }
}

void medViewerConfigurationDiffusion::onBundlingBoxActivated (bool value)
{
    if (!d->view)
        return;
    
    if(dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
        if (value)
            interactor->setProperty ("BoxVisibility", "true");
        else
            interactor->setProperty ("BoxVisibility", "false");
        
        d->view->update();
    }
}

void medViewerConfigurationDiffusion::onTBDiffusionSuccess(void)
{
    if (d->view) {
        d->view->setData( d->diffusionToolBox->output() );
        d->view->reset();
        d->view->update();
    }
}

medViewerConfiguration *createMedViewerConfigurationDiffusion(void)
{
    return new medViewerConfigurationDiffusion;
}
