/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSegmentationSelectorToolBox.h>
#include <medSegmentationAbstractToolBox.h>
#include <medTabbedViewContainers.h>

class medSegmentationAbstractToolBoxPrivate
{
public:
    medSegmentationSelectorToolBox *selectorToolBox;
};

//! Parent should be a medSegmentationSelectorToolBox
medSegmentationAbstractToolBox::medSegmentationAbstractToolBox(QWidget *parent) : medToolBox(parent), d(new medSegmentationAbstractToolBoxPrivate)
{
    d->selectorToolBox = qobject_cast<medSegmentationSelectorToolBox*>(parent);
}

medSegmentationAbstractToolBox::~medSegmentationAbstractToolBox(void)
{
    delete d;
    d = nullptr;
}

medSegmentationSelectorToolBox *medSegmentationAbstractToolBox::selectorToolBox(void)
{
    return d->selectorToolBox;
}

void medSegmentationAbstractToolBox::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    updateView();

    if (getWorkspace())
    {
        connect(getWorkspace()->tabbedViewContainers(), SIGNAL(containersSelectedChanged()),
                this, SLOT(updateView()), Qt::UniqueConnection);
    }
}

void medSegmentationAbstractToolBox::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);

    if (getWorkspace())
    {
        disconnect(getWorkspace()->tabbedViewContainers(), SIGNAL(containersSelectedChanged()), this,
                   SLOT(updateView()));
    }
}
