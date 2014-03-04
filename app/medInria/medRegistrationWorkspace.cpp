/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medRegistrationWorkspace.h>

#include <medViewFactory.h>
#include <medAbstractView.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medRegistrationSelectorToolBox.h>
#include <medViewContainer.h>
#include <medTabbedViewContainers.h>
#include <medRegistrationSelectorToolBox.h>

#include <medToolBoxFactory.h>

class medRegistrationWorkspacePrivate
{
public:
    medRegistrationSelectorToolBox * registrationToolBox;
};

medRegistrationWorkspace::medRegistrationWorkspace(QWidget *parent) : medAbstractWorkspace(parent), d(new medRegistrationWorkspacePrivate)
{
    // -- View toolboxes --

//    QList<QString> toolboxNames = medToolBoxFactory::instance()->toolBoxesFromCategory("view");
//    if(toolboxNames.contains("medViewPropertiesToolBox"))
//    {
//        // we want the medViewPropertiesToolBox to be the first "view" toolbox
//        toolboxNames.move(toolboxNames.indexOf("medViewPropertiesToolBox"),0);
//    }
//    foreach(QString toolbox, toolboxNames)
//    {
//        this->addToolBox( medToolBoxFactory::instance()->createToolBox(toolbox, parent) );
//    }

//    // -- Registration toolbox --

//    d->registrationToolBox = new medRegistrationSelectorToolBox(parent);

//    connect(d->registrationToolBox, SIGNAL(addToolBox(medToolBox *)),
//            this, SLOT(addToolBox(medToolBox *)));
//    connect(d->registrationToolBox, SIGNAL(removeToolBox(medToolBox *)),
//            this, SLOT(removeToolBox(medToolBox *)));

//    this->addToolBox(d->registrationToolBox);
//
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
//    //TODO make it fit with new container - RDE

//    //the stack has been instantiated in constructor
//    if (!this->stackedViewContainers()->count())
//    {
//        //create the fuse container
//        medViewContainer *fuseContainer = new medViewContainer(
//                this->stackedViewContainers());
//        if (medAbstractView* view = medViewFactory::instance()->createView("medVtkView"))
//        {
//            fuseContainer->setView(view);
//            fuseContainer->setAcceptDrops(false);
//            d->registrationToolBox->setFuseView (view);
//        }

//        //create the compare container
//        medViewContainer * compareViewContainer = new medViewContainer(
//                this->stackedViewContainers());
//        connect(compareViewContainer,SIGNAL(droppedFixed(medDataIndex)),
//                d->registrationToolBox,SLOT(onFixedImageDropped(medDataIndex)));
//        connect(compareViewContainer,SIGNAL(droppedMoving(medDataIndex)),
//                d->registrationToolBox,SLOT(onMovingImageDropped(medDataIndex)));
//        connect(compareViewContainer,SIGNAL(viewRemoved(medAbstractView*)),
//                d->registrationToolBox,SLOT(onViewRemoved(medAbstractView*)));
//        connect(fuseContainer,SIGNAL(viewRemoved(medAbstractView*)),
//                d->registrationToolBox,SLOT(onViewRemoved(medAbstractView*)));

//        this->stackedViewContainers()->addContainerInTab("Compare");
//        this->stackedViewContainers()->addContainerInTab("Fuse");
//        this->stackedViewContainers()->lockTabs();
//        this->stackedViewContainers()->setCurrentIndex(0);
//    }
}

void medRegistrationWorkspace::patientChanged(int patientId)
{
    d->registrationToolBox->clear();
}

bool medRegistrationWorkspace::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("registration").size()!=0); 
}


