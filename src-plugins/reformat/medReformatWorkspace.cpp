#include "medReformatWorkspace.h"

#include <medLayerParameterGroup.h>
#include <medViewContainer.h>
#include <medTabbedViewContainers.h>
#include <medSettingsManager.h>
#include <medToolBox.h>
#include <medToolBoxFactory.h>
#include <medWorkspaceFactory.h>
#include <medViewContainerManager.h>
#include <medViewEventFilter.h>
#include <medViewParameterGroup.h>
#include <medDataManager.h>
#include <medAbstractSelectableToolBox.h>
#include <medSelectorToolBox.h>

#include <stdexcept>

class medReformatWorkspacePrivate
{
public:
    // Give values to items without a constructor.
    medReformatWorkspacePrivate() :
       selectorToolBox(NULL)
    {}

    medSelectorToolBox* selectorToolBox;
};

medReformatWorkspace::medReformatWorkspace(QWidget *parent) : medAbstractWorkspace(parent), d(new medReformatWorkspacePrivate)
{
    d->selectorToolBox = new medSelectorToolBox(parent, "reformat");
    connect(d->selectorToolBox,SIGNAL(success()),this,SLOT(onSuccess()));

    this->addToolBox(d->selectorToolBox);
    d->selectorToolBox->setTitle(this->name()); // get workspace name

    connect(this->stackedViewContainers(), SIGNAL(containersSelectedChanged()),
            d->selectorToolBox, SIGNAL(inputChanged()));
}

medReformatWorkspace::~medReformatWorkspace(void)
{
    delete d;
    d = NULL;
}

medSelectorToolBox * medReformatWorkspace::selectorToolBox()
{
    return d->selectorToolBox;
}

bool medReformatWorkspace::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("reformat").size()!=0);
}

bool medReformatWorkspace::registered()
{
    return medWorkspaceFactory::instance()->registerWorkspace <medReformatWorkspace>();
}

void medReformatWorkspace::onSuccess()
{
    medAbstractData * output = d->selectorToolBox->currentToolBox()->processOutput();
    medDataManager::instance()->importData(output);
}

