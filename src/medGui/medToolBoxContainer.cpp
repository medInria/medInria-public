/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medToolBoxContainer.h"

#include "medFlicker.h"
#include "medToolBox.h"

#include <QtGui>

class medToolBoxContainerPrivate
{
public:
    QFrame *container;
    QGridLayout *layout;
    QList<medToolBox *> toolboxes;

    Qt::Orientation layoutOrientation;
};

medToolBoxContainer::medToolBoxContainer(QWidget *parent) : QScrollArea(parent), d(new medToolBoxContainerPrivate)
{
    d->container = new QFrame(this);
    d->container->setObjectName("medToolBoxContainer");
    // by default create a vertical layout
    d->layoutOrientation = Qt::Vertical;
    d->layout = new QGridLayout(d->container);
    // d->layout->setContentsMargins(2, 4, 0, 0);
    // d->layout->setSizeConstraint(QLayout::SetNoConstraint);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);
    d->layout->setSizeConstraint(QLayout::SetMinimumSize);

    this->setFrameStyle(QFrame::NoFrame);
    this->setAttribute(Qt::WA_MacShowFocusRect, false);
    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->setWidget(d->container);
    this->setWidgetResizable(true);

    medFlicker *flicker = new medFlicker(this);
    flicker->activateOn(this);
    
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

medToolBoxContainer::~medToolBoxContainer(void)
{
    delete d;

    d = NULL;
}

void medToolBoxContainer::addToolBox(medToolBox *toolBox)
{
    if (!toolBox)
        return;
    toolBox->setOrientation(d->layoutOrientation);

    if (!d->toolboxes.contains(toolBox)) {
        d->toolboxes.append(toolBox);
        toolBox->setParent (d->container);

        if (d->layoutOrientation==Qt::Vertical) {
            d->layout->setRowStretch (d->layout->count(), 0);
            d->layout->addWidget(toolBox, d->layout->count(), 0, Qt::AlignTop);
            d->layout->setRowStretch (d->layout->count(), 1);
        }
        else {
            d->layout->setColumnStretch (d->layout->count(), 0);
            d->layout->addWidget(toolBox, 0, d->layout->count(), Qt::AlignTop);
            d->layout->setColumnStretch (d->layout->count(), 1);
        }
    }
}

void medToolBoxContainer::removeToolBox(medToolBox *toolBox)
{
    d->toolboxes.removeOne( toolBox );
    d->layout->removeWidget(toolBox);
}


void medToolBoxContainer::clear()
{
    for (int i=0; i<=d->layout->count(); i++) {
      if (d->layoutOrientation==Qt::Vertical)
	d->layout->setRowStretch (i, 0);
      else
	d->layout->setColumnStretch (i, 0);
    }
    foreach(medToolBox *tb, d->toolboxes) {
        tb->hide();
        d->layout->removeWidget(tb);
    }
    d->toolboxes.clear();
}

QList<medToolBox*> medToolBoxContainer::toolBoxes(void) const
{
    return d->toolboxes;
}

void medToolBoxContainer::setOrientation(Qt::Orientation orient)
{
    if (d->layoutOrientation==orient)
        return;

    d->layoutOrientation = orient;

    this->clear();

    //put back toolboxes
    //never used:   the switch of workspace is done while no tb is present...
    //              + clear removes the tbs...
//    foreach(medToolBox * tb, d->toolboxes ) {
//      //addToolBox also sets the orientation of the toolboxes
//      addToolBox (tb);
//    }
}

Qt::Orientation medToolBoxContainer::orientation() const
{
    return d->layoutOrientation;
}

void medToolBoxContainer::setLayoutSpacing( int spacing )
{
    d->layout->setContentsMargins( spacing, spacing, spacing, spacing );
    d->layout->setSpacing( spacing );
}
