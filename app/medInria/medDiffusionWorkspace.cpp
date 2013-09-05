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
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>

#include <medDataManager.h>

#include <medViewPropertiesToolBox.h>
#include <medTensorViewToolBox.h>
#include <medSHViewToolBox.h>
#include <medDiffusionSelectorToolBox.h>
#include <medFiberViewToolBox.h>
#include <medFiberBundlingToolBox.h>
#include <medViewContainer.h>
#include <medSingleViewContainer.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>

class medDiffusionWorkspacePrivate {
public:

    medViewPropertiesToolBox      *viewPropertiesToolBox;
    medFiberViewToolBox           *fiberViewToolBox;
    medFiberBundlingToolBox       *fiberBundlingToolBox;
    medDiffusionSelectorToolBox   *diffusionToolBox;
    medTensorViewToolBox          *tensorViewToolBox;
    medSHViewToolBox       *shViewToolBox;

    QList<dtkSmartPointer<dtkAbstractView> > views;

    QString uuid;
};

const char* medDiffusionWorkspace::ColorModeValues[]     = { "local", "global", "fa" };
const char* medDiffusionWorkspace::RenderingModeValues[] = { "lines", "ribbons", "tubes" };
const char* medDiffusionWorkspace::boolValues[]          = { "false", "true" };

medDiffusionWorkspace::medDiffusionWorkspace(QWidget *parent) : medWorkspace(parent), d(new medDiffusionWorkspacePrivate)
{
    d->viewPropertiesToolBox = new medViewPropertiesToolBox(parent);

    // -- Bundling  toolbox --
    d->fiberBundlingToolBox = new medFiberBundlingToolBox(parent);

    // -- Diffusion toolbox --
    d->diffusionToolBox = new medDiffusionSelectorToolBox(parent);

    connect(d->diffusionToolBox, SIGNAL(addToolBox(medToolBox *)),
            this, SLOT(addToolBox(medToolBox *)));
    connect(d->diffusionToolBox, SIGNAL(removeToolBox(medToolBox *)),
            this, SLOT(removeToolBox(medToolBox *)));

    // -- Tensor tb --
    d->tensorViewToolBox = new medTensorViewToolBox(parent);

    connect(d->tensorViewToolBox, SIGNAL(glyphShapeChanged(const QString&)), this, SLOT(onGlyphShapeChanged(const QString&)));
    connect(d->tensorViewToolBox, SIGNAL(flipX(bool)),                       this, SLOT(onFlipXChanged(const bool)));
    connect(d->tensorViewToolBox, SIGNAL(flipY(bool)),                       this, SLOT(onFlipYChanged(const bool)));
    connect(d->tensorViewToolBox, SIGNAL(flipZ(bool)),                       this, SLOT(onFlipZChanged(const bool)));

    // -- SH tb --
    d->shViewToolBox = new medSHViewToolBox(parent);
    
    connect(d->shViewToolBox, SIGNAL(tesselationTypeChanged(const QString&)), this, SLOT(onTesselationTypeChanged(const QString&)));
    connect(d->shViewToolBox, SIGNAL(flipX(bool)),                       this, SLOT(onSHFlipXChanged(const bool)));
    connect(d->shViewToolBox, SIGNAL(flipY(bool)),                       this, SLOT(onSHFlipYChanged(const bool)));
    connect(d->shViewToolBox, SIGNAL(flipZ(bool)),                       this, SLOT(onSHFlipZChanged(const bool)));
    connect(d->shViewToolBox, SIGNAL(normalize(bool)),               this, SLOT(onSHNormalize(bool)));
    connect(d->shViewToolBox, SIGNAL(tesselationBasisChanged(const QString&)), this, SLOT(onTesselationBasisChanged(const QString&)));

    // -- Fiber view tb --
    d->fiberViewToolBox = new medFiberViewToolBox(parent);

    connect(d->fiberViewToolBox, SIGNAL(fiberColorModeChanged(int)), this, SLOT(onFiberColorModeChanged(int)));
    connect(d->fiberViewToolBox, SIGNAL(GPUActivated(bool)),         this, SLOT(onGPUActivated(bool)));
    connect(d->fiberViewToolBox, SIGNAL(lineModeSelected(bool)),     this, SLOT(onLineModeSelected(bool)));
    connect(d->fiberViewToolBox, SIGNAL(ribbonModeSelected(bool)),   this, SLOT(onRibbonModeSelected(bool)));
    connect(d->fiberViewToolBox, SIGNAL(tubeModeSelected(bool)),     this, SLOT(onTubeModeSelected(bool)));

    connect(d->fiberBundlingToolBox, SIGNAL(fiberSelectionValidated(const QString&, const QColor&)), this, SLOT(refreshInteractors()));

    connect(d->diffusionToolBox, SIGNAL(success()),                  this, SLOT(onTBDiffusionSuccess()));


    addToolBox( d->viewPropertiesToolBox );
    addToolBox( d->diffusionToolBox );
    addToolBox( d->tensorViewToolBox );
    addToolBox( d->shViewToolBox );
    addToolBox( d->fiberViewToolBox );
    addToolBox( d->fiberBundlingToolBox );
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
    d->views.clear();
    medViewContainer * diffusionContainer = NULL;

    //the stack has been instantiated in constructor
    if (!stackedViewContainers()->count()) {
        medSingleViewContainer *singleViewContainer = new medSingleViewContainer ();
        connect (singleViewContainer, SIGNAL (viewAdded (dtkAbstractView*)),   this, SLOT (onViewAdded (dtkAbstractView*)));
        connect (singleViewContainer, SIGNAL (viewRemoved (dtkAbstractView*)), this, SLOT (onViewRemoved (dtkAbstractView*)));

        //ownership of singleViewContainer is transferred to the stackedWidget.
        stackedViewContainers()->addContainer (identifier(), singleViewContainer);

        diffusionContainer = singleViewContainer;

        stackedViewContainers()->lockTabs();
        stackedViewContainers()->hideTabBar();
    } else {
        diffusionContainer = stackedViewContainers()->container(identifier());
        //TODO: maybe clear views here too?
    }

    if (!diffusionContainer)
        return;

    foreach(dtkAbstractView *view, diffusionContainer->views())
        d->views << view;

    //stackedViewContainers()->setContainer (identifier());
}


void medDiffusionWorkspace::onViewAdded (dtkAbstractView *view)
{
    if (!view)
        return;

    if (d->views.contains (view))
        return;

    d->views.append (view);

    view->enableInteractor("v3dViewFiberInteractor");
    view->enableInteractor("v3dViewTensorInteractor");
    view->enableInteractor("v3dViewSHInteractor");

    if (dtkAbstractViewInteractor *interactor = view->interactor("v3dViewFiberInteractor")) {
        connect(d->fiberViewToolBox, SIGNAL(fiberRadiusSet(int)), interactor, SLOT(onRadiusSet(int)));
        updateFiberInteractorWithToolboxValues(interactor, d->fiberViewToolBox);
    }

    if (dtkAbstractViewInteractor *interactor = view->interactor("v3dViewTensorInteractor")) {
        connect(d->tensorViewToolBox, SIGNAL(sampleRateChanged(int)), interactor, SLOT(onSampleRatePropertySet(int)));
        connect(d->tensorViewToolBox, SIGNAL(eigenVectorChanged(int)), interactor, SLOT(onEigenVectorPropertySet(int)));
        connect(d->tensorViewToolBox, SIGNAL(glyphResolutionChanged(int)), interactor, SLOT(onGlyphResolutionPropertySet(int)));
        connect(d->tensorViewToolBox, SIGNAL(reverseBackgroundColor(bool)), interactor, SLOT(onReverseBackgroundColorPropertySet(bool)));
        connect(d->tensorViewToolBox, SIGNAL(scalingChanged(double)), interactor, SLOT(onScalingPropertySet(double)));
        connect(d->tensorViewToolBox, SIGNAL(hideShowAxial(bool)), interactor, SLOT(onHideShowAxialPropertySet(bool)));
        connect(d->tensorViewToolBox, SIGNAL(hideShowCoronal(bool)), interactor, SLOT(onHideShowCoronalPropertySet(bool)));
        connect(d->tensorViewToolBox, SIGNAL(hideShowSagittal(bool)), interactor, SLOT(onHideShowSagittalPropertySet(bool)));

        // each change triggers and update of the view
        // TODO we need to unify the view->update() as now some updates are done here and some
        // are performed in the 'translate' function of the signal emitted by the toolbox
        // (e.g. onFlipXChanged) ... maybe after dtk is extended to support QVariant properties
        connect(d->tensorViewToolBox, SIGNAL(sampleRateChanged(int)), view, SLOT(update()));
        connect(d->tensorViewToolBox, SIGNAL(eigenVectorChanged(int)), view, SLOT(update()));
        connect(d->tensorViewToolBox, SIGNAL(glyphResolutionChanged(int)), view, SLOT(update()));
        connect(d->tensorViewToolBox, SIGNAL(reverseBackgroundColor(bool)), view, SLOT(update()));
        connect(d->tensorViewToolBox, SIGNAL(scalingChanged(double)), view, SLOT(update()));
        connect(d->tensorViewToolBox, SIGNAL(hideShowAxial(bool)), view, SLOT(update()));
        connect(d->tensorViewToolBox, SIGNAL(hideShowCoronal(bool)), view, SLOT(update()));
        connect(d->tensorViewToolBox, SIGNAL(hideShowSagittal(bool)), view, SLOT(update()));

        updateTensorInteractorWithToolboxValues(interactor, d->tensorViewToolBox);
    }

    if (dtkAbstractViewInteractor *interactor = view->interactor("v3dViewSHInteractor")) {

        connect(d->shViewToolBox,SIGNAL(sampleRateChanged(int)),     interactor,SLOT(onSampleRatePropertySet(int)));
        connect(d->shViewToolBox,SIGNAL(glyphResolutionChanged(int)),interactor,SLOT(onGlyphResolutionPropertySet(int)));
        connect(d->shViewToolBox,SIGNAL(scalingChanged(double)),     interactor,SLOT(onScalingPropertySet(double)));

        connect(d->shViewToolBox,SIGNAL(xSliceChanged(int)),interactor,SLOT(onXSlicePropertySet(int)));
        connect(d->shViewToolBox,SIGNAL(ySliceChanged(int)),interactor,SLOT(onYSlicePropertySet(int)));
        connect(d->shViewToolBox,SIGNAL(zSliceChanged(int)),interactor,SLOT(onZSlicePropertySet(int)));

        connect(d->shViewToolBox,SIGNAL(hideShowAxial(bool)),   interactor,SLOT(onHideShowAxialPropertySet(bool)));
        connect(d->shViewToolBox,SIGNAL(hideShowCoronal(bool)), interactor,SLOT(onHideShowCoronalPropertySet(bool)));
        connect(d->shViewToolBox,SIGNAL(hideShowSagittal(bool)),interactor,SLOT(onHideShowSagittalPropertySet(bool)));

        // each change triggers and update of the view
        // TODO we need to unify the view->update() as now some updates are done here and some
        // are performed in the 'translate' function of the signal emitted by the toolbox
        // (e.g. onFlipXChanged) ... maybe after dtk is extended to support QVariant properties
        connect(d->shViewToolBox,SIGNAL(sampleRateChanged(int)),     view,SLOT(update()));
        connect(d->shViewToolBox,SIGNAL(glyphResolutionChanged(int)),view,SLOT(update()));
        connect(d->shViewToolBox,SIGNAL(scalingChanged(double)),     view,SLOT(update()));
        connect(d->shViewToolBox,SIGNAL(hideShowAxial(bool)),        view,SLOT(update()));
        connect(d->shViewToolBox,SIGNAL(hideShowCoronal(bool)),      view,SLOT(update()));
        connect(d->shViewToolBox,SIGNAL(hideShowSagittal(bool)),     view,SLOT(update()));

        connect(d->shViewToolBox,SIGNAL(xSliceChanged(int)),view,SLOT(update()));
        connect(d->shViewToolBox,SIGNAL(ySliceChanged(int)),view,SLOT(update()));
        connect(d->shViewToolBox,SIGNAL(zSliceChanged(int)),view,SLOT(update()));

        connect(view,SIGNAL(positionChanged(const QVector3D&,bool)),interactor,SLOT(onPositionChanged(const QVector3D&,bool)));

        updateSHInteractorWithToolboxValues(interactor, d->shViewToolBox);
    }
}

void medDiffusionWorkspace::updateTensorInteractorWithToolboxValues(dtkAbstractViewInteractor* interactor, medTensorViewToolBox* tensorViewToolBox)
{
    // we are temporary using Qt's reflection in this function to call the slots in the interactor
    // without casting to a specific type (which is in a plugin)
    // this code will be modified once a refactor in dtk property system is done
    // (we might switch to QVariant instead of strings)
    // TODO refactor this...

    interactor->setProperty("GlyphShape", tensorViewToolBox->glyphShape());

    const int sampleRate = tensorViewToolBox->sampleRate();
    QMetaObject::invokeMethod( interactor, "onSampleRatePropertySet", Qt::QueuedConnection, Q_ARG( int, sampleRate ) );

    const bool isFlipX = tensorViewToolBox->isFlipX();
    interactor->setProperty("FlipX",boolValues[isFlipX]);

    const bool isFlipY = tensorViewToolBox->isFlipY();
    interactor->setProperty("FlipY",boolValues[isFlipY]);

    const bool isFlipZ = tensorViewToolBox->isFlipZ();
    interactor->setProperty("FlipZ",boolValues[isFlipZ]);

    const int eigenVector = tensorViewToolBox->eigenVector();
    QMetaObject::invokeMethod( interactor, "onEigenVectorPropertySet", Qt::QueuedConnection, Q_ARG( int, eigenVector ) );

    const int glyphResolution = tensorViewToolBox->glyphResolution();
    QMetaObject::invokeMethod( interactor, "onGlyphResolutionPropertySet", Qt::QueuedConnection, Q_ARG( int, glyphResolution ) );

    const double scale = tensorViewToolBox->scale();
    QMetaObject::invokeMethod( interactor, "onScalingPropertySet", Qt::QueuedConnection, Q_ARG( double, scale ) );

    const bool isShowAxial = tensorViewToolBox->isShowAxial();
    QMetaObject::invokeMethod( interactor, "onHideShowAxialPropertySet", Qt::QueuedConnection, Q_ARG( bool, isShowAxial ) );

    const bool isShowCoronal = tensorViewToolBox->isShowCoronal();
    QMetaObject::invokeMethod( interactor, "onHideShowCoronalPropertySet", Qt::QueuedConnection, Q_ARG( bool, isShowCoronal ) );

    const bool isShowSagittal = tensorViewToolBox->isShowSagittal();
    QMetaObject::invokeMethod( interactor, "onHideShowSagittalPropertySet", Qt::QueuedConnection, Q_ARG( bool, isShowSagittal ) );
}

void medDiffusionWorkspace::updateFiberInteractorWithToolboxValues(dtkAbstractViewInteractor* interactor, medFiberViewToolBox* fiberViewToolBox)
{
    // we are temporary using Qt's reflection in this function to call the slots in the interactor
    // without casting to a specific type (which is in a plugin)
    // this code will be modified once a refactor in dtk property system is done
    // (we might switch to QVariant instead of strings)
    // TODO refactor this...

    const int fiberRadius = d->fiberViewToolBox->fiberRadius();
    QMetaObject::invokeMethod( interactor, "onRadiusSet", Qt::QueuedConnection, Q_ARG( int, fiberRadius ) );

    const int fiberColorMode = d->fiberViewToolBox->fiberColorMode();
    interactor->setProperty("ColorMode",ColorModeValues[fiberColorMode]);

    const bool isGPUActivated = d->fiberViewToolBox->isGPUActivated();
    interactor->setProperty ("GPUMode",boolValues[isGPUActivated]);

    const bool isLineModeSelected = d->fiberViewToolBox->isLineModeSelected();
    if (isLineModeSelected)
        interactor->setProperty ("RenderingMode", "lines");

    const bool isRibbonModeSelected = d->fiberViewToolBox->isRibbonModeSelected();
    if (isRibbonModeSelected)
        interactor->setProperty ("RenderingMode", "ribbons");

    const bool isTubesModeSelected = d->fiberViewToolBox->isTubesModeSelected();
    if (isTubesModeSelected)
        interactor->setProperty ("RenderingMode", "tubes");
}

void medDiffusionWorkspace::updateSHInteractorWithToolboxValues(dtkAbstractViewInteractor* interactor, medSHViewToolBox* shViewToolBox)
{
    // we are temporary using Qt's reflection in this function to call the slots in the interactor
    // without casting to a specific type (which is in a plugin)
    // this code will be modified once a refactor in dtk property system is done
    // (we might switch to QVariant instead of strings)
    // TODO refactor this...
    
    interactor->setProperty("TesselationType", shViewToolBox->tesselationType());
    interactor->setProperty("TesselationBasis", shViewToolBox->tesselationBasis());
    
    int sampleRate = shViewToolBox->sampleRate();
    QMetaObject::invokeMethod( interactor, "onSampleRatePropertySet", Qt::QueuedConnection, Q_ARG( int, sampleRate ) );
    
    const bool isFlipX = shViewToolBox->isFlipX();
    interactor->setProperty("FlipX",boolValues[isFlipX]);
    
    const bool isFlipY = shViewToolBox->isFlipY();
    interactor->setProperty("FlipY",boolValues[isFlipY]);
    
    const bool isFlipZ = shViewToolBox->isFlipZ();
    interactor->setProperty("FlipZ",boolValues[isFlipZ]);
    
    const bool isEnhanced = shViewToolBox->isEnhanced();
    interactor->setProperty("Normalization",boolValues[isEnhanced]);

    const int glyphResolution = shViewToolBox->glyphResolution();
    QMetaObject::invokeMethod( interactor, "onGlyphResolutionPropertySet", Qt::QueuedConnection, Q_ARG( int, glyphResolution ) );
    
    const double scale = shViewToolBox->scale();
    QMetaObject::invokeMethod( interactor, "onScalingPropertySet", Qt::QueuedConnection, Q_ARG( double, scale ) );
    
    const bool isShowAxial = shViewToolBox->isShowAxial();
    QMetaObject::invokeMethod( interactor, "onHideShowAxialPropertySet", Qt::QueuedConnection, Q_ARG( bool, isShowAxial ) );
    
    const bool isShowCoronal = shViewToolBox->isShowCoronal();
    QMetaObject::invokeMethod( interactor, "onHideShowCoronalPropertySet", Qt::QueuedConnection, Q_ARG( bool, isShowCoronal ) );
    
    const bool isShowSagittal = shViewToolBox->isShowSagittal();
    QMetaObject::invokeMethod( interactor, "onHideShowSagittalPropertySet", Qt::QueuedConnection, Q_ARG( bool, isShowSagittal ) );
}

void medDiffusionWorkspace::onViewRemoved (dtkAbstractView *view)
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

void medDiffusionWorkspace::onBoolValueChanged(const bool b,const char* b_name,const char* interactor) {
    medDiffusionWorkspace::onStringValueChanged(QString(boolValues[b]),b_name,interactor);
}

void medDiffusionWorkspace::onBoolValueChanged(const bool b,const char* b_name,const char* v_name,const char* interactor_name) {
    foreach (dtkAbstractView* view,d->views) {
        if (b)
            if (dtkAbstractViewInteractor* interactor = view->interactor(interactor_name)) {
                interactor->setProperty(b_name,v_name);
                view->update();
            }
    }
}

void medDiffusionWorkspace::onStringValueChanged(const QString& s,const char* s_name,const char* interactor_name) {
    foreach (dtkAbstractView *view, d->views) {
        if (dtkAbstractViewInteractor* interactor = view->interactor(interactor_name)) {
            interactor->setProperty(s_name,s);
            view->update();
        }
    }
}

void medDiffusionWorkspace::onFiberColorModeChanged(const int index) {
    onStringValueChanged(QString(ColorModeValues[index]),"ColorMode","v3dViewFiberInteractor");
}

void medDiffusionWorkspace::onGPUActivated(const bool value) {
    onBoolValueChanged(value,"GPUMode","v3dViewFiberInteractor");
}

void medDiffusionWorkspace::onLineModeSelected(const bool value) {
    onBoolValueChanged(value,"RenderingMode","lines","v3dViewFiberInteractor");
}

void medDiffusionWorkspace::onRibbonModeSelected (const bool value) {
    onBoolValueChanged(value,"RenderingMode","ribbons","v3dViewFiberInteractor");
}

void medDiffusionWorkspace::onTubeModeSelected (const bool value) {
    onBoolValueChanged(value,"RenderingMode","tubes","v3dViewFiberInteractor");
}

void medDiffusionWorkspace::onTBDiffusionSuccess()
{
    foreach (dtkAbstractView *view, d->views) {
        view->setData( d->diffusionToolBox->output(), 0 );
        view->reset();
        view->update();
    }

    if (d->diffusionToolBox->output()->identifier()=="v3dDataFibers")
        d->fiberBundlingToolBox->setData( d->diffusionToolBox->output() );

    d->uuid = QUuid::createUuid().toString();
    medDataManager::instance()->importNonPersistent ( d->diffusionToolBox->output(), d->uuid);
}

// tensor interaction related methods

void medDiffusionWorkspace::onGlyphShapeChanged(const QString& glyphShape) {
    onStringValueChanged(glyphShape,"GlyphShape","v3dViewTensorInteractor");
}

void medDiffusionWorkspace::onFlipXChanged(const bool flipX) {
    onBoolValueChanged(flipX,"FlipX","v3dViewTensorInteractor");
}

void medDiffusionWorkspace::onFlipYChanged(const bool flipY) {
    onBoolValueChanged(flipY,"FlipY","v3dViewTensorInteractor");
}

void medDiffusionWorkspace::onFlipZChanged(const bool flipZ) {
    onBoolValueChanged(flipZ,"FlipZ","v3dViewTensorInteractor");
}

// SH interaction related methods

void medDiffusionWorkspace::onTesselationTypeChanged(const QString& tesselationType) {
    onStringValueChanged(tesselationType,"TesselationType","v3dViewSHInteractor");
}

void medDiffusionWorkspace::onTesselationBasisChanged(const QString& tesselationBasis) {
    onStringValueChanged(tesselationBasis,"TesselationBasis","v3dViewSHInteractor");
}

void medDiffusionWorkspace::onSHFlipXChanged(const bool flipX) {
    onBoolValueChanged(flipX,"FlipX","v3dViewSHInteractor");
}

void medDiffusionWorkspace::onSHFlipYChanged(const bool flipY) {
    onBoolValueChanged(flipY,"FlipY","v3dViewSHInteractor");
}

void medDiffusionWorkspace::onSHFlipZChanged(const bool flipZ) {
    onBoolValueChanged(flipZ,"FlipZ","v3dViewSHInteractor");
}

void medDiffusionWorkspace::onSHNormalize(const bool normalize) {
    onBoolValueChanged(normalize,"Normalization","v3dViewSHInteractor");
}

// end of SH interaction related methods

void medDiffusionWorkspace::refreshInteractors ()
{
    foreach (dtkAbstractView *view, d->views) {
        if(view->interactor ("v3dViewFiberInteractor")) {
            // interactor->update(); // TO BE IMPLEMENTED
            view->update();
        }
    }
}

void medDiffusionWorkspace::onAddTabClicked()
{
    QString name = identifier();
    QString realName = addSingleContainer(name);
    stackedViewContainers()->setContainer(realName);
}

bool medDiffusionWorkspace::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("diffusion").size()!=0); 
}
