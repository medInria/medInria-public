/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCore/dtkSignalBlocker.h>

#include <medAbstractLayeredView.h>
#include <medLayerParameterGroup.h>
#include <medRegistrationSelectorToolBox.h>
#include <medRegistrationWorkspace.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medViewContainer.h>
#include <medViewParameterGroup.h>

class medRegistrationWorkspacePrivate
{
public:
    QVector<medViewContainer*> containers; // fixed/moving/fuse
    medViewParameterGroup *viewGroup;
    QVector<medLayerParameterGroup*> layerGroups; // fixed/moving
};

medRegistrationWorkspace::medRegistrationWorkspace(QWidget *parent) : medSelectorWorkspace(parent, staticName(), new medRegistrationSelectorToolBox(parent, staticName())), d(new medRegistrationWorkspacePrivate)
{
    connect(this->stackedViewContainers(), SIGNAL(currentChanged(int)), this, SLOT(updateUserLayerClosable(int)));
    connect(selectorToolBox(), SIGNAL(movingDataRegistered(medAbstractData*)), this, SLOT(updateFromRegistrationSuccess(medAbstractData*)));
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
        d->containers.push_back(this->stackedViewContainers()->addContainerInTab(tr("Compare")));
        QLabel *fixedLabel = new QLabel(tr("FIXED"));
        fixedLabel->setAlignment(Qt::AlignCenter);
        d->containers[0]->setDefaultWidget(fixedLabel);
        d->containers[0]->setMultiLayered(false);
        d->containers[0]->setUserSplittable(false);
        d->containers[0]->setClosingMode(medViewContainer::CLOSE_VIEW);

        d->containers.push_back(d->containers[0]->splitVertically());
        QLabel *movingLabel = new QLabel(tr("MOVING"));
        movingLabel->setAlignment(Qt::AlignCenter);
        d->containers[1]->setDefaultWidget(movingLabel);
        d->containers[1]->setUserSplittable(false);
        d->containers[1]->setMultiLayered(false);
        d->containers[1]->setClosingMode(medViewContainer::CLOSE_VIEW);

        d->containers.push_back(stackedViewContainers()->addContainerInTab(tr("Fuse")));
        QLabel *fuseLabel = new QLabel(tr("FUSE"));
        fuseLabel->setAlignment(Qt::AlignCenter);
        d->containers[2]->setDefaultWidget(fuseLabel);
        d->containers[2]->setClosingMode(medViewContainer::CLOSE_BUTTON_HIDDEN);
        d->containers[2]->setUserSplittable(false);
        d->containers[2]->setAcceptDrops(false);

        connect(d->containers[0], SIGNAL(viewContentChanged()),
                this, SLOT(updateFromFixedContainer()));
        connect(d->containers[1],SIGNAL(viewContentChanged()),
                this, SLOT(updateFromMovingContainer()));

        connect(d->containers[0],SIGNAL(viewRemoved()),
                this, SLOT(updateFromFixedContainer()));
        connect(d->containers[1],SIGNAL(viewRemoved()),
                this, SLOT(updateFromMovingContainer()));

        this->stackedViewContainers()->lockTabs();
        this->stackedViewContainers()->setCurrentIndex(0);
        d->containers[0]->setSelected(true);
        d->containers[1]->setSelected(false);
    }
}

void medRegistrationWorkspace::setInitialGroups()
{
    d->viewGroup = new medViewParameterGroup("View Group 1", this, this->identifier());
    d->layerGroups.push_back(new medLayerParameterGroup("Fixed Group", this, this->identifier()));
    d->layerGroups.push_back(new medLayerParameterGroup("Moving Group", this, this->identifier()));

    d->viewGroup->setLinkAllParameters(true);
    d->viewGroup->removeParameter("DataList");
    d->viewGroup->removeParameter("Position");

    d->layerGroups[0]->setLinkAllParameters(true);
    d->layerGroups[0]->removeParameter("Slicing");

    d->layerGroups[1]->setLinkAllParameters(true);
    d->layerGroups[1]->removeParameter("Slicing");
}

bool medRegistrationWorkspace::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("Registration").size()!=0);
}

void medRegistrationWorkspace::updateFromFixedContainer()
{
    updateFromContainers(0);
}

void medRegistrationWorkspace::updateFromMovingContainer()
{
    updateFromContainers(1);
}

void medRegistrationWorkspace::updateFromContainers(int containerIndex)
{
    medRegistrationSelectorToolBox* toolbox = dynamic_cast<medRegistrationSelectorToolBox*>(selectorToolBox());

    if(toolbox)
    {
        if(!d->containers[containerIndex]->view())
        {
            medAbstractLayeredView* fuseView  = dynamic_cast<medAbstractLayeredView*>(d->containers[2]->view());
            if(fuseView)
            {
                medAbstractData* dataInView      = containerIndex==0? toolbox->fixedData() : toolbox->movingData();
                medAbstractData* dataInOtherView = containerIndex==0? toolbox->movingData() : toolbox->fixedData();

                if(fuseView->layer(dataInView) == 0)
                {
                    d->containers[2]->removeView();
                    d->containers[2]->addData(dataInOtherView);
                }
                else
                {
                    fuseView->removeLayer(1);
                }
            }

            containerIndex==0? toolbox->setFixedData(NULL) : toolbox->setMovingData(NULL);
        }
        else
        {
            medAbstractLayeredView* currentView  = dynamic_cast<medAbstractLayeredView*>(d->containers[containerIndex]->view());
            if(!currentView)
            {
                qWarning() << "Non layered view are not suported yet in registration workspace.";
                return;
            }

            medAbstractData *currentData = currentView->layerData(currentView->currentLayer());

            medAbstractLayeredView* fuseView  = dynamic_cast<medAbstractLayeredView*>(d->containers[2]->view());
            if(fuseView)
                fuseView->removeData(currentData);

            d->containers[2]->addData(currentData);
            fuseView  = dynamic_cast<medAbstractLayeredView*>(d->containers[2]->view());

            if(currentData)
            {
                d->viewGroup->addImpactedView(currentView);
                d->viewGroup->addImpactedView(fuseView);

                d->layerGroups[containerIndex]->addImpactedlayer(currentView, currentData);
                d->layerGroups[containerIndex]->addImpactedlayer(fuseView, currentData);
            }

            containerIndex==0? toolbox->setFixedData(currentData) : toolbox->setMovingData(currentData);
        }
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
    if(selectorToolBox())
    {
        d->containers[1]->disconnect(this);
        d->containers[1]->removeView();
        d->containers[1]->addData(output);

        d->containers[2]->removeView();
        d->containers[2]->addData(dynamic_cast<medRegistrationSelectorToolBox*>(selectorToolBox())->fixedData());
        d->containers[2]->addData(output);

        // Relink the views...
        medAbstractLayeredView* movingView  = dynamic_cast<medAbstractLayeredView*>(d->containers[1]->view());
        if(!movingView)
        {
            qWarning() << "Non layered view are not suported yet in registration workspace.";
            return;
        }

        medAbstractLayeredView* fuseView  = dynamic_cast<medAbstractLayeredView*>(d->containers[2]->view());
        if(!fuseView)
        {
            qWarning() << "Non layered view are not suported yet in registration workspace.";
            return;
        }

        d->viewGroup->addImpactedView(movingView);
        d->viewGroup->addImpactedView(fuseView);

        d->layerGroups[1]->addImpactedlayer(movingView, output);
        d->layerGroups[1]->addImpactedlayer(fuseView, output);

        connect(d->containers[1],SIGNAL(viewContentChanged()),
                this, SLOT(updateFromMovingContainer()));

        connect(d->containers[1],SIGNAL(viewRemoved()),
                this, SLOT(updateFromMovingContainer()));
    }
}
