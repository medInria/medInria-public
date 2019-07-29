/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medLayerParameterGroupL.h>
#include <medSegmentationWorkspace.h>
#include <medSegmentationSelectorToolBox.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medViewParameterGroupL.h>

class medSegmentationWorkspacePrivate
{
public:
    // Give values to items without a constructor.
    medSegmentationWorkspacePrivate() :
       selectorToolBox(nullptr)
    {}

    medSegmentationSelectorToolBox *selectorToolBox;
};


medSegmentationWorkspace::medSegmentationWorkspace(QWidget * parent /* = NULL */ ) :
medAbstractWorkspaceLegacy(parent), d(new medSegmentationWorkspacePrivate)
{
    d->selectorToolBox = new medSegmentationSelectorToolBox(parent);

    // Always have a parent.
    if (!parent)
        throw (std::runtime_error ("Must have a parent widget"));

    this->addToolBox(d->selectorToolBox);

    medViewParameterGroupL *viewGroup1 = new medViewParameterGroupL("View Group 1", this, this->identifier());
    viewGroup1->setLinkAllParameters(true);
    viewGroup1->removeParameter("DataList");

    medLayerParameterGroupL *layerGroup1 = new medLayerParameterGroupL("Layer Group 1", this, this->identifier());
    layerGroup1->setLinkAllParameters(true);

    connect(this->tabbedViewContainers(), SIGNAL(containersSelectedChanged()),
            d->selectorToolBox, SIGNAL(inputChanged()));
}

void medSegmentationWorkspace::setupTabbedViewContainer()
{
    if (!tabbedViewContainers()->count()) {
        this->tabbedViewContainers()->addContainerInTabNamed(this->name());
    }
    this->tabbedViewContainers()->unlockTabs();
    this->tabbedViewContainers()->setKeepLeastOne(true);
}

medSegmentationWorkspace::~medSegmentationWorkspace(void)
{
    delete d;
    d = nullptr;
}

medSegmentationSelectorToolBox * medSegmentationWorkspace::selectorToolBox()
{
    return d->selectorToolBox;
}

bool medSegmentationWorkspace::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("segmentation").size()!=0);
}
