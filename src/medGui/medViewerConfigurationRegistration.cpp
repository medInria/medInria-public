#include "medViewerConfigurationRegistration.h"


#include "medViewerToolBoxView.h"
#include "medToolBoxRegistration.h"
#include <medGui/medViewContainer.h>

class medViewerConfigurationRegistrationPrivate
{
public:
    medViewerToolBoxView   *viewToolBox;
    medToolBoxRegistration * registrationToolBox;
};

medViewerConfigurationRegistration::medViewerConfigurationRegistration(QWidget *parent) : medViewerConfiguration(parent), d(new medViewerConfigurationRegistrationPrivate)
{
    // -- Layout toolbox --
    setLayoutToolBoxVisibility(false);

    // -- View toolbox --

    d->viewToolBox = new medViewerToolBoxView(parent);
    /*
    connect(d->viewToolBox, SIGNAL(foregroundLookupTableChanged(QString)), this, SLOT(setupForegroundLookupTable(QString)));
    // connect(d->viewToolBox, SIGNAL(backgroundLookupTableChanged(QString)), this, SLOT(setupBackgroundLookupTable(QString)));
    connect(d->viewToolBox, SIGNAL(lutPresetChanged(QString)), this, SLOT(setupLUTPreset(QString)));
    connect(d->viewToolBox, SIGNAL(tdModeChanged(QString)), this, SLOT(setup3DMode(QString)));
    connect(d->viewToolBox, SIGNAL(tdVRModeChanged(QString)), this, SLOT(setup3DVRMode(QString)));
    connect(d->viewToolBox, SIGNAL(tdLodChanged(int)), this, SLOT(setup3DLOD(int)));
    connect(d->viewToolBox, SIGNAL(windowingChanged(bool)), this, SLOT(setupWindowing(bool)));
    connect(d->viewToolBox, SIGNAL(zoomingChanged(bool)), this, SLOT(setupZooming(bool)));
    connect(d->viewToolBox, SIGNAL(slicingChanged(bool)), this, SLOT(setupSlicing(bool)));
    connect(d->viewToolBox, SIGNAL(measuringChanged(bool)), this, SLOT(setupMeasuring(bool)));
    connect(d->viewToolBox, SIGNAL(croppingChanged(bool)), this, SLOT(setupCropping(bool)));
    connect(d->viewToolBox, SIGNAL(scalarBarVisibilityChanged(bool)), this, SLOT(setupScalarBarVisibility(bool)));
    connect(d->viewToolBox, SIGNAL(axisVisibilityChanged(bool)), this, SLOT(setupAxisVisibility(bool)));
    connect(d->viewToolBox, SIGNAL(rulerVisibilityChanged(bool)), this, SLOT(setupRulerVisibility(bool)));
    connect(d->viewToolBox, SIGNAL(annotationsVisibilityChanged(bool)), this, SLOT(setupAnnotationsVisibility(bool)));
     */

    this->addToolBox( d->viewToolBox );


    // -- Registration toolbox --

    d->registrationToolBox = new medToolBoxRegistration(parent);

    //connect(d->registrationToolBox, SIGNAL(setupLayoutCompare()), this, SLOT(setupLayoutCompare()));
    //connect(d->registrationToolBox, SIGNAL(setupLayoutFuse()), this, SLOT(setupLayoutFuse()));
    connect(d->registrationToolBox, SIGNAL(addToolBox(medToolBox *)),
            this, SLOT(addToolBox(medToolBox *)));
    connect(d->registrationToolBox, SIGNAL(removeToolBox(medToolBox *)),
            this, SLOT(removeToolBox(medToolBox *)));

    this->addToolBox( d->registrationToolBox );

    this->setViewLayoutType(medViewContainer::Custom);
    this->setCustomLayoutType(medViewContainerCustom::A);

}

medViewerConfigurationRegistration::~medViewerConfigurationRegistration(void)
{
    delete d;
    d = NULL;
}

QString medViewerConfigurationRegistration::description(void) const
{
    return "Registration";
}

medViewerConfiguration *createMedViewerConfigurationRegistration(void)
{
    return new medViewerConfigurationRegistration;
}
