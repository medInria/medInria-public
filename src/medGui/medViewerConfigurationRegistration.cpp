#include "medViewerConfigurationRegistration.h"


#include "medViewerToolBoxView.h"
#include "medToolBoxRegistration.h"
#include <medGui/medViewContainer.h>
#include <medGui/medViewContainerStack.h>

#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>

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
    this->addToolBox( d->viewToolBox );


    // -- Registration toolbox --

    d->registrationToolBox = new medToolBoxRegistration(parent);

    connect(d->registrationToolBox, SIGNAL(setupLayoutCompare()), this, SLOT(onSetupLayoutCompare()));
    connect(d->registrationToolBox, SIGNAL(setupLayoutFuse()),    this, SLOT(onSetupLayoutFuse()));
    
    connect(d->registrationToolBox, SIGNAL(addToolBox(medToolBox *)),
            this, SLOT(addToolBox(medToolBox *)));
    connect(d->registrationToolBox, SIGNAL(removeToolBox(medToolBox *)),
            this, SLOT(removeToolBox(medToolBox *)));

    this->addToolBox( d->registrationToolBox );

    this->setViewLayoutType (medViewContainer::Compare);
    
    this->setLayoutType(medViewerConfiguration::TopDbBottomTb);
    //this->setLayoutType(medViewerConfiguration::LeftDbRightTb);


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
    emit layoutModeChanged (medViewContainer::Compare);
}

void medViewerConfigurationRegistration::onSetupLayoutFuse (void)
{
    emit layoutModeChanged (medViewContainer::Fuse);
}

void medViewerConfigurationRegistration::setupViewContainerStack(medViewContainerStack *container)
{
    if (!container) {
        return;
    }
    
    medViewContainer *fuseContainer = container->fuse();
    if (fuseContainer->current()->view()) {
        d->registrationToolBox->setFuseView ( fuseContainer->current()->view() );
    } 
    else {
        if (dtkAbstractView *view = dtkAbstractViewFactory::instance()->create("v3dViewPublic")) {
            fuseContainer->current()->setView (view);
            d->registrationToolBox->setFuseView (view);
        }
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
