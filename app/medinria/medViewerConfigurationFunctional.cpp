#include "medViewerConfigurationFunctional.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>

#include <medCore/medDataManager.h>

#include "medGui/medViewerToolBoxView.h"
#include <medGui/medViewContainer.h>
#include <medGui/medViewContainerSingle.h>
#include <medGui/medStackedViewContainers.h>

class medViewerConfigurationFunctionalPrivate
{
public:
    medViewerToolBoxView                *viewToolBox;

    QList<dtkAbstractView *> views;
};

medViewerConfigurationFunctional::medViewerConfigurationFunctional(QWidget *parent) : medViewerConfiguration(parent), d(new medViewerConfigurationFunctionalPrivate)
{
    // -- Layout toolbox --
    setLayoutToolBoxVisibility(false);

    // -- View toolbox --
    d->viewToolBox = new medViewerToolBoxView(parent);

    this->addToolBox( d->viewToolBox );
}

medViewerConfigurationFunctional::~medViewerConfigurationFunctional(void)
{
    delete d;
    d = NULL;
}

QString medViewerConfigurationFunctional::description(void) const
{
    return "Functional";
}

void medViewerConfigurationFunctional::setupViewContainerStack()
{
    d->views.clear();
    medViewContainer* functionalContainer;
    //the stack has been instantiated in constructor
    if (!this->stackedViewContainers()->count())
    {
        medViewContainerSingle* single = new medViewContainerSingle ();
        connect (single, SIGNAL (viewAdded (dtkAbstractView*)),   this, SLOT (onViewAdded (dtkAbstractView*)));
        connect (single, SIGNAL (viewRemoved (dtkAbstractView*)), this, SLOT (onViewRemoved (dtkAbstractView*)));
        //ownership of single is transferred to the stackedWidget.
        this->stackedViewContainers()->addContainer(description(), single);
        functionalContainer = single;
    }
    else
    {
        functionalContainer = this->stackedViewContainers()->container(description());
        //TODO: maybe clear views here too?
    }

    d->views << functionalContainer->views();
    this->stackedViewContainers()->setContainer(description());
}

void medViewerConfigurationFunctional::onViewAdded (dtkAbstractView *view)
{
    if (!view)
        return;

    if (d->views.contains (view))
        return;

    d->views.append (view);
}

void medViewerConfigurationFunctional::onViewRemoved (dtkAbstractView *view)
{
    if (!view)
        return;

    if (!d->views.contains (view))
        return;

    d->views.removeOne (view);
}

medViewerConfiguration *createMedViewerConfigurationFunctional(void)
{
    return new medViewerConfigurationFunctional;
}
