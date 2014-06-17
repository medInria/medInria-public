/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medToolBoxContainer.h>
#include <medToolBox.h>

#include <QtGui>

class medToolBoxContainerPrivate
{
public:
    QFrame *container;
    QVBoxLayout *layout;
    QList<medToolBox *> toolboxes;

    Qt::Orientation layoutOrientation;
};

medToolBoxContainer::medToolBoxContainer(QWidget *parent) : QScrollArea(parent), d(new medToolBoxContainerPrivate)
{
    d->container = new QFrame(this);
    d->container->setObjectName("medToolBoxContainer");
    d->layout = new QVBoxLayout(d->container);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);
    d->layout->setSizeConstraint(QLayout::SetMinimumSize);

    this->setFrameStyle(QFrame::NoFrame);
    this->setAttribute(Qt::WA_MacShowFocusRect, false);
    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->setWidget(d->container);
    this->setWidgetResizable(true);

    
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

medToolBoxContainer::~medToolBoxContainer(void)
{
    delete d;
    d = NULL;
}

void medToolBoxContainer::insertToolBox(int index, medToolBox* toolBox)
{
    if (!toolBox)
        return;

    d->toolboxes.insert(index, toolBox);
    toolBox->setParent(d->container);
    d->layout->setStretch(d->layout->count()-1, 0);
    d->layout->insertWidget(index, toolBox, 0, Qt::AlignTop);
    d->layout->addStretch(1);
}

void medToolBoxContainer::addToolBox(medToolBox *toolBox)
{
    this->insertToolBox(d->layout->count(), toolBox);
}

void medToolBoxContainer::removeToolBox(medToolBox *toolBox)
{
    d->toolboxes.removeOne( toolBox );
    d->layout->removeWidget(toolBox);
}


void medToolBoxContainer::clear()
{
    for (int i=0; i<=d->layout->count(); i++)
        d->layout->setStretch (i, 0);

    foreach(medToolBox *tb, d->toolboxes)
    {
        tb->hide();
        d->layout->removeWidget(tb);
    }

    d->toolboxes.clear();
}

QList<medToolBox*> medToolBoxContainer::toolBoxes(void) const
{
    return d->toolboxes;
}
