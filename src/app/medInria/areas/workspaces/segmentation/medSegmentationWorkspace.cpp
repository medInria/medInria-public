/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSegmentationWorkspace.h>

#include <medSelectorToolBox.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>

medSegmentationWorkspace::medSegmentationWorkspace(QWidget *parent)
    : medSelectorWorkspace(parent, staticName())
{
    connect(this->tabbedViewContainers(), SIGNAL(containersSelectedChanged()),
            selectorToolBox(), SIGNAL(inputChanged()));
}

bool medSegmentationWorkspace::isUsable()
{
    medToolBoxFactory *tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("Segmentation").size()!=0);
}
