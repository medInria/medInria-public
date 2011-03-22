#include "medViewerConfigurationDiffusion.h"
#include "medGui/medToolBoxDiffusionTensorView.h"
#include "medGui/medToolBoxDiffusion.h"
#include "medGui/medViewerToolBoxView.h"
#include "medGui/medToolBoxDiffusionFiberView.h"
#include "medGui/medToolBoxDiffusionFiberBundling.h"
#include <medGui/medViewContainer.h>
#include <medGui/medViewContainerStack.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>

class medViewerConfigurationDiffusionPrivate
{
public:
    medViewerToolBoxView                *viewToolBox;
    medToolBoxDiffusionFiberView        *fiberViewToolBox;
    medToolBoxDiffusionFiberBundling    *fiberBundlingToolBox;
    medToolBoxDiffusion                 *diffusionToolBox;
    medToolBoxDiffusionTensorView       *tensorViewToolBox;
    
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
    
    // -- Tensor tb --
    d->tensorViewToolBox = new medToolBoxDiffusionTensorView(parent);

    connect(d->tensorViewToolBox, SIGNAL(glyphShapeChanged(const QString&)), this, SLOT(onGlyphShapeChanged(const QString&)));
    connect(d->tensorViewToolBox, SIGNAL(flipX(bool)),                       this, SLOT(onFlipXChanged(bool)));
    connect(d->tensorViewToolBox, SIGNAL(flipY(bool)),                       this, SLOT(onFlipYChanged(bool)));
    connect(d->tensorViewToolBox, SIGNAL(flipZ(bool)),                       this, SLOT(onFlipZChanged(bool)));

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
    this->addToolBox( d->tensorViewToolBox );
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
    view->enableInteractor ("v3dViewTensorInteractor");

    if (dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewFiberInteractor"))
        connect(d->fiberViewToolBox, SIGNAL(fiberRadiusSet(int)), interactor, SLOT(onRadiusSet(int)));
    
    // TODO this needs to be refactored (once dtk supports QVariant properties) as in this case
    // the interactor does the view->update and not this class (as for all the other methods)
    if (dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewTensorInteractor"))
    {
        connect(d->tensorViewToolBox, SIGNAL(sampleRateChanged(int)), interactor, SLOT(onSampleRatePropertySet(int)));
        connect(d->tensorViewToolBox, SIGNAL(eigenVectorChanged(int)), interactor, SLOT(onEigenVectorPropertySet(int)));
        connect(d->tensorViewToolBox, SIGNAL(glyphResolutionChanged(int)), interactor, SLOT(onGlyphResolutionPropertySet(int)));
        connect(d->tensorViewToolBox, SIGNAL(reverseBackgroundColor(bool)), interactor, SLOT(onReverseBackgroundColorPropertySet(bool)));
        connect(d->tensorViewToolBox, SIGNAL(scalingChanged(double)), interactor, SLOT(onScalingPropertySet(double)));
        connect(d->tensorViewToolBox, SIGNAL(hideShowAxial(bool)), interactor, SLOT(onHideShowAxialPropertySet(bool)));
        connect(d->tensorViewToolBox, SIGNAL(hideShowCoronal(bool)), interactor, SLOT(onHideShowCoronalPropertySet(bool)));
        connect(d->tensorViewToolBox, SIGNAL(hideShowSagittal(bool)), interactor, SLOT(onHideShowSagittalPropertySet(bool)));

        connect(view, SIGNAL(positionChanged(const QVector3D&)), interactor, SLOT(onPositionChanged(const QVector3D&)));

        updateTensorInteractorWithToolboxValues(interactor, d->tensorViewToolBox);
    }

    view->setData( d->diffusionToolBox->output(), 0 );
}

void medViewerConfigurationDiffusion::updateTensorInteractorWithToolboxValues(dtkAbstractViewInteractor* interactor, medToolBoxDiffusionTensorView* tensorViewToolBox)
{
    // we are temporary using Qt's reflection in this function to call the slots in the interactor
    // without casting to a specific type (which is in a plugin)
    // this code will be modified once a refactor in dtk property system is done
    // (we might switch to QVariant instead of strings)
    // TODO refactor this...

    interactor->setProperty("GlyphShape", tensorViewToolBox->glyphShape());

    int sampleRate = tensorViewToolBox->sampleRate();
    QMetaObject::invokeMethod( interactor, "onSampleRatePropertySet", Qt::QueuedConnection, Q_ARG( int, sampleRate ) );

    bool isFlipX = tensorViewToolBox->isFlipX();
    interactor->setProperty("FlipX", isFlipX ? "true" : "false");

    bool isFlipY = tensorViewToolBox->isFlipY();
    interactor->setProperty("FlipY", isFlipY ? "true" : "false");

    bool isFlipZ = tensorViewToolBox->isFlipZ();
    interactor->setProperty("FlipZ", isFlipZ ? "true" : "false");

    int eigenVector = tensorViewToolBox->eigenVector();
    QMetaObject::invokeMethod( interactor, "onEigenVectorPropertySet", Qt::QueuedConnection, Q_ARG( int, eigenVector ) );

    int glyphResolution = tensorViewToolBox->glyphResolution();
    QMetaObject::invokeMethod( interactor, "onGlyphResolutionPropertySet", Qt::QueuedConnection, Q_ARG( int, glyphResolution ) );

    double scale = tensorViewToolBox->scale();
    QMetaObject::invokeMethod( interactor, "onScalingPropertySet", Qt::QueuedConnection, Q_ARG( double, scale ) );

    bool isShowAxial = tensorViewToolBox->isShowAxial();
    QMetaObject::invokeMethod( interactor, "onHideShowAxialPropertySet", Qt::QueuedConnection, Q_ARG( bool, isShowAxial ) );

    bool isShowCoronal = tensorViewToolBox->isShowCoronal();
    QMetaObject::invokeMethod( interactor, "onHideShowCoronalPropertySet", Qt::QueuedConnection, Q_ARG( bool, isShowCoronal ) );

    bool isShowSagittal = tensorViewToolBox->isShowSagittal();
    QMetaObject::invokeMethod( interactor, "onHideShowSagittalPropertySet", Qt::QueuedConnection, Q_ARG( bool, isShowSagittal ) );
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

// tensor interaction related methods

void medViewerConfigurationDiffusion::onGlyphShapeChanged(const QString& glyphShape)
{
    foreach (dtkAbstractView *view, d->views) {
        if (dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewTensorInteractor")) {
            interactor->setProperty("GlyphShape", glyphShape);

            view->update();
        }
    }
}

void medViewerConfigurationDiffusion::onFlipXChanged(bool flipX)
{
    foreach (dtkAbstractView *view, d->views) {
        if (dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewTensorInteractor")) {

            if (flipX)
                interactor->setProperty("FlipX", "true");
            else
                interactor->setProperty("FlipX", "false");

            view->update();
        }
    }
}

void medViewerConfigurationDiffusion::onFlipYChanged(bool flipY)
{
    foreach (dtkAbstractView *view, d->views) {
        if (dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewTensorInteractor")) {

            if (flipY)
                interactor->setProperty("FlipY", "true");
            else
                interactor->setProperty("FlipY", "false");

            view->update();
        }
    }
}

void medViewerConfigurationDiffusion::onFlipZChanged(bool flipZ)
{
    foreach (dtkAbstractView *view, d->views) {
        if (dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewTensorInteractor")) {

            if (flipZ)
                interactor->setProperty("FlipZ", "true");
            else
                interactor->setProperty("FlipZ", "false");

            view->update();
        }
    }
}

// end of tensor interaction related methods

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
