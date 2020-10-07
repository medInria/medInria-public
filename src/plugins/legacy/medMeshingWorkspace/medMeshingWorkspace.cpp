/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medMeshingWorkspace.h>
#include <medSelectorToolBox.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medWorkspaceFactory.h>

medMeshingWorkspace::medMeshingWorkspace(QWidget *parent)
    : medSelectorWorkspace(parent, staticName())
{
    connect(this->tabbedViewContainers(), SIGNAL(containersSelectedChanged()),
            selectorToolBox(), SIGNAL(inputChanged()));
}

bool medMeshingWorkspace::isUsable()
{
    medToolBoxFactory *tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("Meshing").size()!=0);
}

bool medMeshingWorkspace::registered()
{
    return medWorkspaceFactory::instance()->registerWorkspace <medMeshingWorkspace>();
}
