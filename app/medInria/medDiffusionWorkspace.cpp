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

#include <medViewContainer.h>
#include <medSingleViewContainer.h>
#include <medTabbedViewContainers.h>
#include <medToolBox.h>
#include <medToolBoxFactory.h>
#include <medDiffusionSelectorToolBox.h>

class medDiffusionWorkspacePrivate
{
public:

    medToolBox * diffusionToolBox;
    medToolBox * fiberBundlingToolBox;
    medToolBox * viewPropertiesToolBox;
    medToolBox * fiberViewToolBox;
    medToolBox * tensorViewToolBox;

    medViewContainer * diffusionContainer;
};

medDiffusionWorkspace::medDiffusionWorkspace(QWidget *parent) : medWorkspace(parent), d(new medDiffusionWorkspacePrivate)
{
    d->diffusionContainer = 0;

    d->diffusionToolBox =  new medDiffusionSelectorToolBox(parent);

    d->viewPropertiesToolBox = medToolBoxFactory::instance()->createToolBox("medViewPropertiesToolBox", parent);
    d->fiberBundlingToolBox = medToolBoxFactory::instance()->createToolBox("medFiberBundlingToolBox", parent);

    connect(d->diffusionToolBox, SIGNAL(addToolBox(medToolBox *)),
            this, SLOT(addToolBox(medToolBox *)));
    connect(d->diffusionToolBox, SIGNAL(removeToolBox(medToolBox *)),
            this, SLOT(removeToolBox(medToolBox *)));

    d->tensorViewToolBox = medToolBoxFactory::instance()->createToolBox("medTensorViewToolBox", parent);
    d->fiberViewToolBox = medToolBoxFactory::instance()->createToolBox("medFiberViewToolBox", parent);

    connect(d->diffusionToolBox, SIGNAL(newOutput(dtkAbstractData*)), d->fiberBundlingToolBox, SLOT(setInput(dtkAbstractData*)));
    connect(d->diffusionToolBox, SIGNAL(newOutput(dtkAbstractData*)), this, SLOT(addToView(dtkAbstractData*)));

    this->addToolBox( d->diffusionToolBox );
    this->addToolBox( d->viewPropertiesToolBox );
    this->addToolBox( d->tensorViewToolBox );
    this->addToolBox( d->fiberViewToolBox );
    this->addToolBox( d->fiberBundlingToolBox );
}

medDiffusionWorkspace::~medDiffusionWorkspace(void)
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
    //the stack has been instantiated in constructor
    if ( ! this->stackedViewContainers()->count())
    {
        medSingleViewContainer *singleViewContainer = new medSingleViewContainer ();

        //ownership of singleViewContainer is transferred to the stackedWidget.
        this->stackedViewContainers()->addContainer (identifier(), singleViewContainer);

        d->diffusionContainer = singleViewContainer;

        this->stackedViewContainers()->lockTabs();
        this->stackedViewContainers()->hideTabBar();
    }
    else
    {
        d->diffusionContainer = this->stackedViewContainers()->container(identifier());
        //TODO: maybe clear views here too?
    }

    if ( ! d->diffusionContainer)
        return;
}

void medDiffusionWorkspace::addToView(dtkAbstractData * data)
{
    d->diffusionContainer->view()->setData(data);
}

void medDiffusionWorkspace::onAddTabClicked()
{
    QString name = this->identifier();
    QString realName = this->addSingleContainer(name);
    this->stackedViewContainers()->setContainer(realName);
}

bool medDiffusionWorkspace::isUsable(){
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("diffusion").size()!=0); 
}
