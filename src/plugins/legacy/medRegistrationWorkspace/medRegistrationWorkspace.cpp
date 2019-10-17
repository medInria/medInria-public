/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkSignalBlocker.h>

#include <medAbstractLayeredView.h>
#include <medLayerParameterGroupL.h>
#include <medRegistrationSelectorToolBox.h>
#include <medRegistrationWorkspace.h>
#include <medSelectorWorkspace.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medViewContainer.h>
#include <medViewParameterGroupL.h>
#include <medWorkspaceFactory.h>

class medRegistrationWorkspacePrivate
{
public:
    medViewContainer *containers[3]; // fixed/moving/fuse
    medViewParameterGroupL *viewGroup;
    medLayerParameterGroupL *layerGroups[2]; // fixed/moving
};

medRegistrationWorkspace::medRegistrationWorkspace(QWidget *parent)
    : medSelectorWorkspace(parent, staticName(), new medRegistrationSelectorToolBox(parent, staticName())), d(new medRegistrationWorkspacePrivate)
{
    connect(this->tabbedViewContainers(), SIGNAL(currentChanged(int)), this, SLOT(updateUserLayerClosable(int)));
    connect(dynamic_cast<medRegistrationSelectorToolBox*>(selectorToolBox()), SIGNAL(movingDataRegistered(medAbstractData*)),
        this, SLOT(updateFromRegistrationSuccess(medAbstractData*)));
}

medRegistrationWorkspace::~medRegistrationWorkspace(void)
{
    delete d;
    d = nullptr;
}

void medRegistrationWorkspace::setupTabbedViewContainer()
{
    medAbstractWorkspaceLegacy::setupTabbedViewContainer();

    d->containers[Fixed] = this->tabbedViewContainers()->containersInTab(0).at(0);
    resetDefaultWidgetFixedContainer();

    d->containers[Moving] = d->containers[Fixed]->splitVertically();
    resetDefaultWidgetMovingContainer();

    d->containers[Fuse] = tabbedViewContainers()->addContainerInTabNamed(tr("Fuse"));
    resetDefaultWidgetFuseContainer();

    connect(d->containers[Fixed], SIGNAL(viewContentChanged()),
            this, SLOT(updateFromFixedContainer()));
    connect(d->containers[Moving],SIGNAL(viewContentChanged()),
            this, SLOT(updateFromMovingContainer()));

    connect(d->containers[Fixed],SIGNAL(viewRemoved()),
            this, SLOT(updateFromFixedContainer()));
    connect(d->containers[Moving],SIGNAL(viewRemoved()),
            this, SLOT(updateFromMovingContainer()));

    connect(d->containers[Fixed], SIGNAL(viewRemoved()),this, SLOT(resetDefaultWidgetFixedContainer()));
    connect(d->containers[Moving],SIGNAL(viewRemoved()),this, SLOT(resetDefaultWidgetMovingContainer()));
    connect(d->containers[Fuse],  SIGNAL(viewRemoved()),this, SLOT(resetDefaultWidgetFuseContainer()));

    this->tabbedViewContainers()->setCurrentIndex(Fixed);
    d->containers[Fixed]->setSelected(true);
    d->containers[Moving]->setSelected(false);
}

void medRegistrationWorkspace::resetDefaultWidgetFixedContainer()
{
    if(selectorToolBox()) //null when users close the software
    {
        QLabel *fixedLabel = new QLabel(tr("FIXED"));
        fixedLabel->setAlignment(Qt::AlignCenter);
        d->containers[Fixed]->setDefaultWidget(fixedLabel);
        d->containers[Fixed]->setMultiLayered(false);
        d->containers[Fixed]->setUserSplittable(false);
        d->containers[Fixed]->setClosingMode(medViewContainer::CLOSE_VIEW);
    }
}

void medRegistrationWorkspace::resetDefaultWidgetMovingContainer()
{
    if(selectorToolBox()) //null when users close the software
    {
        QLabel *movingLabel = new QLabel(tr("MOVING"));
        movingLabel->setAlignment(Qt::AlignCenter);
        d->containers[Moving]->setDefaultWidget(movingLabel);
        d->containers[Moving]->setUserSplittable(false);
        d->containers[Moving]->setMultiLayered(false);
        d->containers[Moving]->setClosingMode(medViewContainer::CLOSE_VIEW);
    }
}

void medRegistrationWorkspace::resetDefaultWidgetFuseContainer()
{
    if(selectorToolBox()) //null when users close the software
    {
        QLabel *fuseLabel = new QLabel(tr("FUSE"));
        fuseLabel->setAlignment(Qt::AlignCenter);
        d->containers[Fuse]->setDefaultWidget(fuseLabel);
        d->containers[Fuse]->setClosingMode(medViewContainer::CLOSE_BUTTON_HIDDEN);
        d->containers[Fuse]->setUserSplittable(false);
        d->containers[Fuse]->setAcceptDrops(false);
    }
}

void medRegistrationWorkspace::setInitialGroups()
{
    d->viewGroup = new medViewParameterGroupL("View Group 1", this, this->identifier());
    d->layerGroups[Fixed] = new medLayerParameterGroupL("Fixed Group", this, this->identifier());
    d->layerGroups[Moving] = new medLayerParameterGroupL("Moving Group", this, this->identifier());

    d->viewGroup->setLinkAllParameters(true);
    d->viewGroup->removeParameter("DataList");
    d->viewGroup->removeParameter("Position");

    d->layerGroups[Fixed]->setLinkAllParameters(true);
    d->layerGroups[Fixed]->removeParameter("Slicing");

    d->layerGroups[Moving]->setLinkAllParameters(true);
    d->layerGroups[Moving]->removeParameter("Slicing");
}

bool medRegistrationWorkspace::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("Registration").size()!=0);
}

bool medRegistrationWorkspace::registered()
{
    return medWorkspaceFactory::instance()->registerWorkspace <medRegistrationWorkspace>();
}

void medRegistrationWorkspace::updateFromFixedContainer()
{
    updateFromContainer(Fixed);
}

void medRegistrationWorkspace::updateFromMovingContainer()
{
    updateFromContainer(Moving);
}

void medRegistrationWorkspace::updateFromContainer(medRegistrationWorkspace::ContainerIndex containerIndex)
{
    medRegistrationSelectorToolBox *toolbox = dynamic_cast<medRegistrationSelectorToolBox*>(selectorToolBox());

    if(toolbox)
    {
        if(!d->containers[containerIndex]->view())
        {
            medAbstractLayeredView *fuseView  = dynamic_cast<medAbstractLayeredView*>(d->containers[Fuse]->view());
            if(fuseView)
            {
                medAbstractData *dataInView      = containerIndex==Fixed? toolbox->fixedData() : toolbox->movingData();
                medAbstractData *dataInOtherView = containerIndex==Fixed? toolbox->movingData() : toolbox->fixedData();

                if(fuseView->layer(dataInView) == 0)
                {
                    d->containers[Fuse]->removeView();
                    d->containers[Fuse]->addData(dataInOtherView);
                }
                else
                {
                    fuseView->removeLayer(1);
                }
            }

            if(containerIndex == Fixed)
            {
                toolbox->setFixedData(nullptr);
            }
            else
            {
                toolbox->setMovingData(nullptr);
            }
        }
        else
        {
            medAbstractLayeredView *currentView  = dynamic_cast<medAbstractLayeredView*>(d->containers[containerIndex]->view());
            if(!currentView)
            {
                qWarning() << "Non layered views are not supported yet in Registration workspace.";
                return;
            }

            medAbstractData *currentData = currentView->layerData(currentView->currentLayer());

            medAbstractLayeredView *fuseView  = dynamic_cast<medAbstractLayeredView*>(d->containers[Fuse]->view());
            if(fuseView)
            {
                fuseView->removeData(currentData);
            }

            d->containers[Fuse]->addData(currentData);
            fuseView  = dynamic_cast<medAbstractLayeredView*>(d->containers[Fuse]->view());

            if(currentData)
            {
                d->layerGroups[containerIndex]->addImpactedlayer(currentView, currentData);
                d->layerGroups[containerIndex]->addImpactedlayer(fuseView, currentData);
            }

            if(containerIndex == Fixed)
            {
                toolbox->setFixedData(currentData);
            }
            else
            {
                toolbox->setMovingData(currentData);
            }
        }
    }
}

void medRegistrationWorkspace::updateUserLayerClosable(int tabIndex)
{
    if(tabIndex == 0)
    {
        this->setUserLayerClosable(true);
    }
    else
    {
        this->setUserLayerClosable(false);
    }
}

void medRegistrationWorkspace::updateFromRegistrationSuccess(medAbstractData *output)
{
    if(selectorToolBox())
    {
        d->containers[Moving]->disconnect(this);
        d->containers[Moving]->removeView();
        d->containers[Moving]->addData(output);

        d->containers[Fuse]->removeView();
        d->containers[Fuse]->addData(dynamic_cast<medRegistrationSelectorToolBox*>(selectorToolBox())->fixedData());
        d->containers[Fuse]->addData(output);

        // Relink the views...
        medAbstractLayeredView *movingView  = dynamic_cast<medAbstractLayeredView*>(d->containers[Moving]->view());
        if(!movingView)
        {
            qWarning() << "Non layered views are not supported yet in Registration workspace.";
            return;
        }

        medAbstractLayeredView *fuseView  = dynamic_cast<medAbstractLayeredView*>(d->containers[Fuse]->view());
        if(!fuseView)
        {
            qWarning() << "Non layered views are not supported yet in Registration workspace.";
            return;
        }

        d->viewGroup->addImpactedView(movingView);
        d->viewGroup->addImpactedView(fuseView);

        d->layerGroups[Moving]->addImpactedlayer(movingView, output);
        d->layerGroups[Moving]->addImpactedlayer(fuseView, output);

        connect(d->containers[Moving],SIGNAL(viewContentChanged()),
                this, SLOT(updateFromMovingContainer()));

        connect(d->containers[Moving],SIGNAL(viewRemoved()),
                this, SLOT(updateFromMovingContainer()));
    }
}
