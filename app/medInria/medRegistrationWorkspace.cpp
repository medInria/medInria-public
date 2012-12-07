#include "medRegistrationWorkspace.h"

#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medWorkspaceViewPropertiesToolBox.h>
#include <medToolBoxRegistration.h>
#include <medViewContainer.h>
#include <medSingleViewContainer.h>
#include <medCompareViewContainer.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxRegistration.h>

#include <medToolBoxFactory.h>

class medRegistrationWorkspacePrivate
{
public:
    medToolBoxRegistration * registrationToolBox;
    medWorkspaceViewPropertiesToolBox      *viewPropertiesToolBox;
};

medRegistrationWorkspace::medRegistrationWorkspace(QWidget *parent) : medViewerWorkspace(parent), d(new medRegistrationWorkspacePrivate)
{


    d->viewPropertiesToolBox = new medWorkspaceViewPropertiesToolBox(parent);
    this->addToolBox(d->viewPropertiesToolBox);

    // -- Registration toolbox --

    d->registrationToolBox = new medToolBoxRegistration(parent);

    connect(d->registrationToolBox, SIGNAL(addToolBox(medToolBox *)),
            this, SLOT(addToolBox(medToolBox *)));
    connect(d->registrationToolBox, SIGNAL(removeToolBox(medToolBox *)),
            this, SLOT(removeToolBox(medToolBox *)));

    this->addToolBox( d->registrationToolBox );


    //this->setLayoutType(medViewerWorkspace::TopDbBottomTb);
    this->setLayoutType(medViewerWorkspace::LeftDbRightTb);
}

medRegistrationWorkspace::~medRegistrationWorkspace(void)
{
    delete d;
    d = NULL;
}

QString medRegistrationWorkspace::identifier() const {
    return "Registration";
}

QString medRegistrationWorkspace::description() const {
    return tr("Registration");
}


void medRegistrationWorkspace::setupViewContainerStack()
{
    //the stack has been instantiated in constructor
    if (!this->stackedViewContainers()->count())
    {
        //create the fuse container
        medSingleViewContainer *fuseContainer = new medSingleViewContainer(
                this->stackedViewContainers());
        if (dtkSmartPointer<dtkAbstractView> view = dtkAbstractViewFactory::instance()->createSmartPointer("v3dView"))
        {
            fuseContainer->setView (view);
            d->registrationToolBox->setFuseView (view);
        }

        //create the compare container
        medCompareViewContainer * compareViewContainer = new medCompareViewContainer(
                this->stackedViewContainers());
        connect(compareViewContainer,SIGNAL(droppedFixed(medDataIndex)),
                d->registrationToolBox,SLOT(onFixedImageDropped(medDataIndex)));
        connect(compareViewContainer,SIGNAL(droppedMoving(medDataIndex)),
                d->registrationToolBox,SLOT(onMovingImageDropped(medDataIndex)));

        this->stackedViewContainers()->addContainer("Compare",compareViewContainer);
        this->stackedViewContainers()->addContainer("Fuse",fuseContainer);
        this->stackedViewContainers()->lockTabs();
        setCurrentViewContainer("Compare");
    }
}

void medRegistrationWorkspace::patientChanged(int patientId)
{
    d->registrationToolBox->clear();
}

bool medRegistrationWorkspace::isUsable(){
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("registration").size()!=0); 
}


