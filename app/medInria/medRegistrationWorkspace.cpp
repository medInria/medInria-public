/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medRegistrationWorkspace.h>

#include <dtkCore/dtkSignalBlocker.h>

#include <medViewFactory.h>
#include <medAbstractView.h>

#include <medRegistrationSelectorToolBox.h>
#include <medViewContainer.h>
#include <medTabbedViewContainers.h>
#include <medRegistrationSelectorToolBox.h>
#include <medAbstractLayeredView.h>
#include <medStringListParameter.h>

#include <medToolBoxFactory.h>

#include <medViewParameterGroup.h>
#include <medLayerParameterGroup.h>

class medRegistrationWorkspacePrivate
{
public:
    medRegistrationSelectorToolBox * selectorToolBox;
    medViewContainer *fixedContainer;
    medViewContainer *movingContainer;
    medViewContainer *fuseContainer;

    medViewParameterGroup *viewGroup;
    medLayerParameterGroup *fixedLayerGroup;
    medLayerParameterGroup *movingLayerGroup;
};

medRegistrationWorkspace::medRegistrationWorkspace(QWidget *parent) : medAbstractWorkspace(parent), d(new medRegistrationWorkspacePrivate)
{
    d->selectorToolBox = new medRegistrationSelectorToolBox(parent);
    d->selectorToolBox->setWorkspace(this);
    this->addToolBox(d->selectorToolBox);
    d->selectorToolBox->setTitle(this->name()); // get workspace name

    connect(this->stackedViewContainers(), SIGNAL(currentChanged(int)), this, SLOT(updateUserLayerClosable(int)));
    connect(d->selectorToolBox, SIGNAL(movingDataRegistered(medAbstractData*)), this, SLOT(updateFromRegistrationSuccess(medAbstractData*)));
    connect(d->selectorToolBox, SIGNAL(destroyed()), this, SLOT(removeSlectorInternToolBox()));
}

medRegistrationWorkspace::~medRegistrationWorkspace(void)
{
    delete d;
    d = NULL;
}

void medRegistrationWorkspace::setupViewContainerStack()
{
    //the stack has been instantiated in constructor
    if (!this->stackedViewContainers()->count())
    {
        d->fixedContainer = this->stackedViewContainers()->addContainerInTab(tr("Compare"));
        QLabel *fixedLabel = new QLabel(tr("FIXED"));
        fixedLabel->setAlignment(Qt::AlignCenter);
        d->fixedContainer->setDefaultWidget(fixedLabel);
        d->fixedContainer->setMultiLayered(false);
        d->fixedContainer->setUserSplittable(false);
        d->fixedContainer->setClosingMode(medViewContainer::CLOSE_VIEW);

        d->movingContainer = d->fixedContainer->splitVertically();
        QLabel *movingLabel = new QLabel(tr("MOVING"));
        movingLabel->setAlignment(Qt::AlignCenter);
        d->movingContainer->setDefaultWidget(movingLabel);
        d->movingContainer->setUserSplittable(false);
        d->movingContainer->setMultiLayered(false);
        d->movingContainer->setClosingMode(medViewContainer::CLOSE_VIEW);


        d->fuseContainer = this->stackedViewContainers()->addContainerInTab(tr("Fuse"));
        QLabel *fuseLabel = new QLabel(tr("FUSE"));
        fuseLabel->setAlignment(Qt::AlignCenter);
        d->fuseContainer->setDefaultWidget(fuseLabel);
        d->fuseContainer->setClosingMode(medViewContainer::CLOSE_BUTTON_HIDDEN);
        d->fuseContainer->setUserSplittable(false);
        d->fuseContainer->setAcceptDrops(false);

        connect(d->fixedContainer, SIGNAL(viewContentChanged()),
                this, SLOT(updateFromFixedContainer()));
        connect(d->movingContainer,SIGNAL(viewContentChanged()),
                this, SLOT(updateFromMovingContainer()));

        connect(d->fixedContainer,SIGNAL(viewRemoved()),
                this, SLOT(updateFromFixedContainer()));
        connect(d->movingContainer,SIGNAL(viewRemoved()),
                this, SLOT(updateFromMovingContainer()));

        this->stackedViewContainers()->lockTabs();
        this->stackedViewContainers()->setCurrentIndex(0);
        d->fixedContainer->setSelected(true);
        d->movingContainer->setSelected(false);
    }
}

void medRegistrationWorkspace::setInitialGroups()
{
    d->viewGroup = new medViewParameterGroup("View Group 1", this, this->identifier());
    d->fixedLayerGroup =  new medLayerParameterGroup("Fixed Group", this, this->identifier());
    d->movingLayerGroup = new medLayerParameterGroup("Moving Group", this, this->identifier());

    d->viewGroup->setLinkAllParameters(true);
    d->viewGroup->removeParameter("DataList");
    d->viewGroup->removeParameter("Position");

    d->fixedLayerGroup->setLinkAllParameters(true);
    d->fixedLayerGroup->removeParameter("Slicing");

    d->movingLayerGroup->setLinkAllParameters(true);
    d->movingLayerGroup->removeParameter("Slicing");
}

bool medRegistrationWorkspace::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("registration").size()!=0);
}

void medRegistrationWorkspace::updateFromMovingContainer()
{
    if(!d->selectorToolBox)
        return;

    if(!d->movingContainer->view())
    {
        medAbstractLayeredView* fuseView  = dynamic_cast<medAbstractLayeredView*>(d->fuseContainer->view());
        if(fuseView)
        {
            if(fuseView->layer(d->selectorToolBox->movingData()) == 0)
            {
                d->fuseContainer->removeView();
                d->fuseContainer->addData(d->selectorToolBox->fixedData());
            }
            else
                fuseView->removeLayer(1);

        }

        d->selectorToolBox->setMovingData(NULL);
        return;
    }

    medAbstractLayeredView* movingView  = dynamic_cast<medAbstractLayeredView*>(d->movingContainer->view());
    if(!movingView)
    {
        qWarning() << "Non layered view are not suported yet in registration workspace.";
        return;
    }

    medAbstractData *movingData = movingView->layerData(movingView->currentLayer());

    medAbstractLayeredView* fuseView  = dynamic_cast<medAbstractLayeredView*>(d->fuseContainer->view());
    if(fuseView)
        fuseView->removeData(movingData);

    d->fuseContainer->addData(movingData);
    fuseView  = dynamic_cast<medAbstractLayeredView*>(d->fuseContainer->view());

    if(movingData)
    {
        d->viewGroup->addImpactedView(movingView);
        d->viewGroup->addImpactedView(fuseView);

        d->movingLayerGroup->addImpactedlayer(movingView, movingData);
        d->movingLayerGroup->addImpactedlayer(fuseView, movingData);
    }
    if (!d->selectorToolBox->setMovingData(movingData))
    {
        // delete the view because something failed at some point
        d->viewGroup->removeImpactedView(movingView);
        d->movingLayerGroup->removeImpactedlayer(movingView, movingData);
        d->movingLayerGroup->removeImpactedlayer(fuseView, movingData);
        movingView->deleteLater();
    }
}

void medRegistrationWorkspace::updateFromFixedContainer()
{
    if(!d->selectorToolBox)
        return;

    if(!d->fixedContainer->view())
    {
        medAbstractLayeredView* fuseView  = dynamic_cast<medAbstractLayeredView*>(d->fuseContainer->view());
        if(fuseView)
        {
            if(fuseView->layer(d->selectorToolBox->fixedData()) == 0)
            {
                d->fuseContainer->removeView();
                d->fuseContainer->addData(d->selectorToolBox->movingData());
            }
            else
                fuseView->removeLayer(1);

        }

        d->selectorToolBox->setFixedData(NULL);
        return;
    }

    medAbstractLayeredView* fixedView  = dynamic_cast<medAbstractLayeredView*>(d->fixedContainer->view());
    if(!fixedView)
    {
        qWarning() << "Non layered view are not suported yet in registration workspace.";
        return;
    }

    medAbstractData *fixedData = fixedView->layerData(fixedView->currentLayer());
    medAbstractLayeredView* fuseView  = dynamic_cast<medAbstractLayeredView*>(d->fuseContainer->view());
    if(fuseView)
        fuseView->removeData(fixedData);

    d->fuseContainer->addData(fixedData);
    fuseView  = dynamic_cast<medAbstractLayeredView*>(d->fuseContainer->view());

    if(fixedData)
    {
        d->viewGroup->addImpactedView(fixedView);
        d->viewGroup->addImpactedView(fuseView);

        d->fixedLayerGroup->addImpactedlayer(fixedView, fixedData);
        d->fixedLayerGroup->addImpactedlayer(fuseView, fixedData);
    }

    if (!d->selectorToolBox->setFixedData(fixedData))
    {
        // delete the view because something failed at some point
        d->viewGroup->removeImpactedView(fixedView);
        d->fixedLayerGroup->removeImpactedlayer(fixedView, fixedData);
        d->fixedLayerGroup->removeImpactedlayer(fuseView, fixedData);
        fixedView->deleteLater();
    }
}


void medRegistrationWorkspace::updateUserLayerClosable(int tabIndex)
{
    if(tabIndex == 0)
        this->setUserLayerClosable(true);
    else
        this->setUserLayerClosable(false);
}

void medRegistrationWorkspace::updateFromRegistrationSuccess(medAbstractData *output)
{
    if(!d->selectorToolBox)
        return;

    //TODO disconnect because we dont want to change input of the undo redo process.
    //  find a better way to do it ? - RDE
    d->movingContainer->disconnect(this);

    d->movingContainer->removeView();
    d->movingContainer->addData(output);

    d->fuseContainer->removeView();
    d->fuseContainer->addData(d->selectorToolBox->fixedData());
    d->fuseContainer->addData(output);


    // Relink the views...
    medAbstractLayeredView* movingView  = dynamic_cast<medAbstractLayeredView*>(d->movingContainer->view());
    if(!movingView)
    {
        qWarning() << "Non layered view are not suported yet in registration workspace.";
        return;
    }


    medAbstractLayeredView* fuseView  = dynamic_cast<medAbstractLayeredView*>(d->fuseContainer->view());
    if(!fuseView)
    {
        qWarning() << "Non layered view are not suported yet in registration workspace.";
        return;
    }

    d->viewGroup->addImpactedView(movingView);
    d->viewGroup->addImpactedView(fuseView);

    d->movingLayerGroup->addImpactedlayer(movingView, output);
    d->movingLayerGroup->addImpactedlayer(fuseView, output);

    connect(d->movingContainer,SIGNAL(viewContentChanged()),
            this, SLOT(updateFromMovingContainer()));

    connect(d->movingContainer,SIGNAL(viewRemoved()),
            this, SLOT(updateFromMovingContainer()));
}

void medRegistrationWorkspace::removeSlectorInternToolBox()
{
    d->selectorToolBox = NULL;
}
