/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSelectableToolBox.h>
#include <medSelectorToolBox.h>
#include <medTabbedViewContainers.h>
#include <medAbstractData.h>

class medAbstractSelectableToolBoxPrivate
{
public:
    QPointer<medSelectorToolBox> selectorToolBox;
};

//! Parent should be a medSelectorToolBox
medAbstractSelectableToolBox::medAbstractSelectableToolBox(QWidget *parent)
    : medToolBox(parent), d(new medAbstractSelectableToolBoxPrivate)
{
    d->selectorToolBox = qobject_cast<medSelectorToolBox*>(parent);
}

medAbstractSelectableToolBox::~medAbstractSelectableToolBox()
{
    delete d;
    d = nullptr;
}

dtkPlugin* medAbstractSelectableToolBox::plugin()
{
    return nullptr;
}

QList<dtkSmartPointer<medAbstractData> > medAbstractSelectableToolBox::processOutputs()
{
    QList<dtkSmartPointer<medAbstractData>> outputs;
    if (auto toolboxOutput = processOutput())
    {
        outputs.append(toolboxOutput);
    }
    return outputs;
}

void medAbstractSelectableToolBox::setSelectorToolBox(medSelectorToolBox *toolbox)
{
    d->selectorToolBox = toolbox;
}

medSelectorToolBox *medAbstractSelectableToolBox::selectorToolBox()
{
    return d->selectorToolBox;
}

void medAbstractSelectableToolBox::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    updateView();

    if (getWorkspace())
    {
        connect(getWorkspace()->tabbedViewContainers(), SIGNAL(containersSelectedChanged()), this,
                SLOT(updateView()), Qt::UniqueConnection);
    }
}

void medAbstractSelectableToolBox::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);

    if (getWorkspace())
    {
        disconnect(getWorkspace()->tabbedViewContainers(), SIGNAL(containersSelectedChanged()), this,
                   SLOT(updateView()));
    }
}
