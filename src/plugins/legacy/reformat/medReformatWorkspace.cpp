/*=========================================================================

 medInria

 Copyright (c) INRIA 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "medReformatWorkspace.h"

#include <medSelectorToolBox.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medWorkspaceFactory.h>

medReformatWorkspace::medReformatWorkspace(QWidget *parent) : medSelectorWorkspace(parent, staticName())
{
    connect(this->tabbedViewContainers(), SIGNAL(containersSelectedChanged()),
            selectorToolBox(), SIGNAL(inputChanged()));
}

bool medReformatWorkspace::isUsable()
{
    medToolBoxFactory *tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("Reformat").size()!=0);
}

bool medReformatWorkspace::registered()
{
    return medWorkspaceFactory::instance()->registerWorkspace <medReformatWorkspace>();
}
