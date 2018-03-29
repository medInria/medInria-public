#include "medReformatWorkspace.h"

#include <medSelectorToolBox.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medWorkspaceFactory.h>

medReformatWorkspace::medReformatWorkspace(QWidget *parent) : medSelectorWorkspace(parent, staticName())
{
    connect(this->stackedViewContainers(), SIGNAL(containersSelectedChanged()),
            selectorToolBox(), SIGNAL(inputChanged()));
}

bool medReformatWorkspace::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("Reformat").size()!=0);
}

bool medReformatWorkspace::registered()
{
    return medWorkspaceFactory::instance()->registerWorkspace <medReformatWorkspace>();
}
