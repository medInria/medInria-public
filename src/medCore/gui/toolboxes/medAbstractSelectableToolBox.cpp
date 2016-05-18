/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSelectableToolBox.h>
#include <medSelectorToolBox.h>
#include <medTabbedViewContainers.h>

class medAbstractSelectableToolBoxPrivate
{
public:
    medSelectorToolBox * selectorToolBox;
};

//! Parent should be a medSelectorToolBox
medAbstractSelectableToolBox::medAbstractSelectableToolBox(QWidget *parent) : medToolBox(parent), d(new medAbstractSelectableToolBoxPrivate)
{
    d->selectorToolBox = qobject_cast<medSelectorToolBox*>(parent);
}

medAbstractSelectableToolBox::~medAbstractSelectableToolBox(void)
{
    delete d;
    d = NULL;
}

void medAbstractSelectableToolBox::setSelectorToolBox(medSelectorToolBox *toolbox)
{
    d->selectorToolBox = toolbox;
}

//! Get the selectorToolBox (usually one instance)
medSelectorToolBox *medAbstractSelectableToolBox::selectorToolBox(void)
{
    return d->selectorToolBox;
}

void medAbstractSelectableToolBox::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    updateView();

    if (getWorkspace())
    {
        connect(getWorkspace()->stackedViewContainers(), SIGNAL(containersSelectedChanged()), this,
                SLOT(updateView()), Qt::UniqueConnection);
    }
}

void medAbstractSelectableToolBox::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);

    if (getWorkspace())
    {
        disconnect(getWorkspace()->stackedViewContainers(), SIGNAL(containersSelectedChanged()), this,
                   SLOT(updateView()));
    }
}
