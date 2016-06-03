/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSegmentationWorkspace.h>

#include <medAbstractSelectableToolBox.h>
#include <medDataManager.h>
#include <medLayerParameterGroup.h>
#include <medSelectorToolBox.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medViewParameterGroup.h>

#include <stdexcept>

class medSegmentationWorkspacePrivate
{
public:
    // Give values to items without a constructor.
    medSegmentationWorkspacePrivate() :
       selectorToolBox(NULL)
    {}

    medSelectorToolBox *selectorToolBox;
    medToolBox * roiManagementToolBox;
};


medSegmentationWorkspace::medSegmentationWorkspace(QWidget * parent) :
medAbstractWorkspace(parent), d(new medSegmentationWorkspacePrivate)
{
    d->selectorToolBox = new medSelectorToolBox(parent, "segmentation");

    connect(d->selectorToolBox,SIGNAL(success()),this,SLOT(importToolBoxOutput()));

    this->addToolBox(d->selectorToolBox);
    d->selectorToolBox->setTitle(this->name()); // get workspace name

    if(medToolBoxFactory::instance()->createToolBox("medRoiManagementToolBox"))
	{
		d->roiManagementToolBox= medToolBoxFactory::instance()->createToolBox("medRoiManagementToolBox");
		d->roiManagementToolBox->setWorkspace(this);
		this->addToolBox(d->roiManagementToolBox);
	}

    connect(this->stackedViewContainers(), SIGNAL(containersSelectedChanged()),
            d->selectorToolBox, SIGNAL(inputChanged()));
}

medSegmentationWorkspace::~medSegmentationWorkspace(void)
{
    delete d;
    d = NULL;
}

medSelectorToolBox * medSegmentationWorkspace::segmentationToobox()
{
    return d->selectorToolBox;
}

bool medSegmentationWorkspace::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("segmentation").size()!=0); 
}

void medSegmentationWorkspace::importToolBoxOutput()
{
    medAbstractData * output = d->selectorToolBox->currentToolBox()->processOutput();
    medDataManager::instance()->importData(output);
}
