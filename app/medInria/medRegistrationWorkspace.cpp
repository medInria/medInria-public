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
#include <medAbstractLayeredView.h>

#include <medToolBoxFactory.h>

class medRegistrationWorkspacePrivate
{
public:
    medRegistrationSelectorToolBox * registrationToolBox;
    medViewContainer *fixedContainer;
    medViewContainer *movingContainer;
    medViewContainer *fuseContainer;
};

medRegistrationWorkspace::medRegistrationWorkspace(QWidget *parent) : medAbstractWorkspace(parent), d(new medRegistrationWorkspacePrivate)
{
    // -- Registration toolbox --

    d->registrationToolBox = new medRegistrationSelectorToolBox(parent);
    this->addToolBox(d->registrationToolBox);

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
    //TODO make it fit with new container - RDE

    //the stack has been instantiated in constructor
    if (!this->stackedViewContainers()->count())
    {
        d->fixedContainer = this->stackedViewContainers()->addContainerInTab("Compare");
        QLabel *fixedLabel = new QLabel("FIXED");
        fixedLabel->setAlignment(Qt::AlignCenter);
        d->fixedContainer->setDefaultWidget(fixedLabel);
        d->fixedContainer->setMultiLayered(false);
        d->fixedContainer->setUserClosable(false);
        d->fixedContainer->setUserSplittable(false);

        d->movingContainer = d->fixedContainer->splitVertically();
        QLabel *movingLabel = new QLabel("MOVING");
        movingLabel->setAlignment(Qt::AlignCenter);
        d->movingContainer->setDefaultWidget(movingLabel);
        d->movingContainer->setUserClosable(false);
        d->movingContainer->setUserSplittable(false);
        d->movingContainer->setMultiLayered(false);


        d->fuseContainer = this->stackedViewContainers()->addContainerInTab("Fuse");
        QLabel *fuseLabel = new QLabel("FUSE");
        fuseLabel->setAlignment(Qt::AlignCenter);
        d->fuseContainer->setDefaultWidget(fuseLabel);
        d->fuseContainer->setUserClosable(false);
        d->fuseContainer->setUserSplittable(false);
        d->fuseContainer->setAcceptDrops(false);

        connect(d->fixedContainer, SIGNAL(viewContentChanged()),
                this, SLOT(updateFixedData()));
        connect(d->movingContainer,SIGNAL(viewContentChanged()),
                this, SLOT(updateMovingData()));

        connect(d->fixedContainer,SIGNAL(viewRemoved()),
                this, SLOT(updateFixedData()));
        connect(d->movingContainer,SIGNAL(viewRemoved()),
                this, SLOT(updateMovingData()));

        this->stackedViewContainers()->lockTabs();
        this->stackedViewContainers()->setCurrentIndex(0);
    }
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

void medRegistrationWorkspace::updateMovingData()
{
    if(!d->movingContainer->view())
    {
        medAbstractLayeredView* fuseView  = dynamic_cast<medAbstractLayeredView*>(d->fuseContainer->view());
        if(fuseView)
            fuseView->removeData(d->registrationToolBox->movingData());

        d->registrationToolBox->setMovingData(NULL);
        return;
    }

    medAbstractLayeredView* movingView  = dynamic_cast<medAbstractLayeredView*>(d->movingContainer->view());
    if(!movingView)
    {
        qWarning() << "Non layered view are not suported yet in registration workspace.";
        return;
    }

    medAbstractData *movingData = movingView->data(movingView->currentLayer());

    medAbstractLayeredView* fuseView  = dynamic_cast<medAbstractLayeredView*>(d->fuseContainer->view());
    if(fuseView)
        fuseView->removeData(movingData);

    d->fuseContainer->addData(movingData);

    d->registrationToolBox->setMovingData(movingData);
}

void medRegistrationWorkspace::updateFixedData()
{
    if(!d->fixedContainer->view())
    {
        medAbstractLayeredView* fuseView  = dynamic_cast<medAbstractLayeredView*>(d->fuseContainer->view());
        if(fuseView)
            fuseView->removeData(d->registrationToolBox->fixedData());

        d->registrationToolBox->setFixedData(NULL);
        return;
    }

    medAbstractLayeredView* fixedView  = dynamic_cast<medAbstractLayeredView*>(d->fixedContainer->view());
    if(!fixedView)
    {
        qWarning() << "Non layered view are not suported yet in registration workspace.";
        return;
    }

    medAbstractData *fixedData = fixedView->data(fixedView->currentLayer());
    medAbstractLayeredView* fuseView  = dynamic_cast<medAbstractLayeredView*>(d->fuseContainer->view());
    if(fuseView)
        fuseView->removeData(fixedData);

    d->fuseContainer->addData(fixedData);
    d->registrationToolBox->setFixedData(fixedData);
}
