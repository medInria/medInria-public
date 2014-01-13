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
    medViewContainer * diffusionContainer;
    
    QString currentContainerName;
};

medDiffusionWorkspace::medDiffusionWorkspace(QWidget *parent) : medWorkspace(parent), d(new medDiffusionWorkspacePrivate)
{
    d->diffusionContainer = 0;

    d->diffusionToolBox =  new medDiffusionSelectorToolBox(parent);

    d->fiberBundlingToolBox = medToolBoxFactory::instance()->createToolBox("medFiberBundlingToolBox", parent);

    connect(d->diffusionToolBox, SIGNAL(addToolBox(medToolBox *)),
            this, SLOT(addToolBox(medToolBox *)));
    connect(d->diffusionToolBox, SIGNAL(removeToolBox(medToolBox *)),
            this, SLOT(removeToolBox(medToolBox *)));

    connect(d->diffusionToolBox, SIGNAL(newOutput(dtkAbstractData*)), d->fiberBundlingToolBox, SLOT(setInput(dtkAbstractData*)));
    connect(d->diffusionToolBox, SIGNAL(newOutput(dtkAbstractData*)), this, SLOT(addToView(dtkAbstractData*)));

    // -- View toolboxes --

    QList<QString> toolboxNames = medToolBoxFactory::instance()->toolBoxesFromCategory("view");
    if(toolboxNames.contains("medViewPropertiesToolBox"))
    {
        // we want the medViewPropertiesToolBox to be the first "view" toolbox
        toolboxNames.move(toolboxNames.indexOf("medViewPropertiesToolBox"),0);
    }
    foreach(QString toolbox, toolboxNames)
    {
       addToolBox( medToolBoxFactory::instance()->createToolBox(toolbox, parent) );
    }

    this->addToolBox( d->diffusionToolBox );
    this->addToolBox( d->fiberBundlingToolBox );

}

medDiffusionWorkspace::~medDiffusionWorkspace()
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
        d->currentContainerName = identifier();

        d->diffusionContainer = singleViewContainer;

        connect(this->stackedViewContainers(),SIGNAL(currentChanged(QString)),this,SLOT(changeCurrentContainer(QString)));
    }
    else
    {
        d->diffusionContainer = this->stackedViewContainers()->container(d->currentContainerName);
        //TODO: maybe clear views here too?
    }
}

void medDiffusionWorkspace::addToView(dtkAbstractData * data)
{
    if( d->diffusionContainer->view() )
    {
        d->diffusionContainer->view()->setData(data, 0);
        d->diffusionContainer->view()->reset();
        d->diffusionContainer->view()->update();
    }
	else
	{
		d->diffusionContainer->open(data);
	}
}

void medDiffusionWorkspace::onAddTabClicked()
{
    QString name = this->identifier();
    QString realName = this->addSingleContainer(name);
    this->stackedViewContainers()->setContainer(realName);
    d->currentContainerName = realName;
}

void medDiffusionWorkspace::changeCurrentContainer(QString name)
{
    d->currentContainerName = name;
}

bool medDiffusionWorkspace::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("diffusion").size()!=0); 
}
