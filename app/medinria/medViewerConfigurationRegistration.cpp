#include "medViewerConfigurationRegistration.h"

#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>

#include <medGui/medViewerToolBoxViewProperties.h>
#include <medGui/medToolBoxRegistration.h>
#include <medGui/medViewContainer.h>
#include <medGui/medViewContainerSingle.h>
#include <medGui/medViewContainerCompare.h>
#include <medGui/medStackedViewContainers.h>
#include <medGui/medViewerToolBoxView.h>
#include <medGui/medToolBoxRegistration.h>

class medViewerConfigurationRegistrationPrivate
{
public:
    medViewerToolBoxView   *viewToolBox;
    medToolBoxRegistration * registrationToolBox;
    medViewerToolBoxViewProperties      *viewPropertiesToolBox;
};

medViewerConfigurationRegistration::medViewerConfigurationRegistration(QWidget *parent) : medViewerConfiguration(parent), d(new medViewerConfigurationRegistrationPrivate)
{
    // -- Layout toolbox --
    setLayoutToolBoxVisibility(false);

    // -- View toolbox --

    d->viewToolBox = new medViewerToolBoxView(parent);
    this->addToolBox( d->viewToolBox );
    
    d->viewPropertiesToolBox = new medViewerToolBoxViewProperties(parent);
    this->addToolBox(d->viewPropertiesToolBox);

    // -- Registration toolbox --

    d->registrationToolBox = new medToolBoxRegistration(parent);

    connect(d->registrationToolBox, SIGNAL(setupLayoutCompare()), this, SLOT(onSetupLayoutCompare()));
    connect(d->registrationToolBox, SIGNAL(setupLayoutFuse()),    this, SLOT(onSetupLayoutFuse()));
    
    connect(d->registrationToolBox, SIGNAL(addToolBox(medToolBox *)),
            this, SLOT(addToolBox(medToolBox *)));
    connect(d->registrationToolBox, SIGNAL(removeToolBox(medToolBox *)),
            this, SLOT(removeToolBox(medToolBox *)));

    this->addToolBox( d->registrationToolBox );

    
    //this->setLayoutType(medViewerConfiguration::TopDbBottomTb);
    this->setLayoutType(medViewerConfiguration::LeftDbRightTb);
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

void medViewerConfigurationRegistration::onSetupLayoutCompare (void)
{
    emit layoutModeChanged ("Compare");
}

void medViewerConfigurationRegistration::onSetupLayoutFuse (void)
{
    emit layoutModeChanged ("Fuse");
}

void medViewerConfigurationRegistration::setupViewContainerStack()
{   
    //the stack has been instantiated in constructor
    if (!this->stackedViewContainers()->count())
    {
        //create the fuse container
        medViewContainerSingle *fuseContainer = new medViewContainerSingle(
                this->stackedViewContainers());
        if (dtkAbstractView *view = dtkAbstractViewFactory::instance()->create("v3dView"))
        {
            fuseContainer->setView (view);
            d->registrationToolBox->setFuseView (view);
        }
        
        //create the compare container
        medViewContainerCompare * compareContainer = new medViewContainerCompare(
                this->stackedViewContainers());
        connect(compareContainer,SIGNAL(droppedFixed(medDataIndex)),
                d->registrationToolBox,SLOT(onFixedImageDropped(medDataIndex)));
        connect(compareContainer,SIGNAL(droppedMoving(medDataIndex)),
                d->registrationToolBox,SLOT(onMovingImageDropped(medDataIndex)));
        
        this->stackedViewContainers()->addContainer("Compare",compareContainer);
        this->stackedViewContainers()->addContainer("Fuse",fuseContainer);
        setCurrentViewContainer("Compare");   
    }
}

void medViewerConfigurationRegistration::patientChanged(int patientId)
{
    d->registrationToolBox->clear();
}

medViewerConfiguration *createMedViewerConfigurationRegistration(void)
{
    return new medViewerConfigurationRegistration;
}

