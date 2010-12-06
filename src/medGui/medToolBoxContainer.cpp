/* medToolBoxContainer.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct 26 21:51:08 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Feb 19 18:25:16 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 16
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medToolBoxContainer.h"

#include "medFlicker.h"
#include "medToolBox.h"

#include <QtGui>

class medToolBoxContainerPrivate
{
public:
    QFrame *container;
    QBoxLayout *layout;

    medToolBoxContainer::LayoutOrientation layoutOrientation;
};

medToolBoxContainer::medToolBoxContainer(QWidget *parent) : QScrollArea(parent), d(new medToolBoxContainerPrivate)
{
    d->container = new QFrame(this);
    d->container->setObjectName("medToolBoxContainer");
    // by default create a vertical layout
    d->layoutOrientation = Vertical;
    d->layout = new QVBoxLayout(d->container);
    d->layout->setContentsMargins(2, 4, 0, 0);
    d->layout->addStretch(1);

    this->setFrameStyle(QFrame::NoFrame);
    this->setAttribute(Qt::WA_MacShowFocusRect, false);
    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->setWidget(d->container);
    this->setWidgetResizable(true);

    medFlicker *flicker = new medFlicker(this);
    flicker->activateOn(this);
}

medToolBoxContainer::~medToolBoxContainer(void)
{
    delete d;

    d = NULL;
}

void medToolBoxContainer::addToolBox(medToolBox *toolBox)
{
    d->layout->insertWidget(d->layout->count()-1, toolBox, 0, Qt::AlignTop);
}

void medToolBoxContainer::removeToolBox(medToolBox *toolBox)
{
    d->layout->removeWidget(toolBox);
}


void medToolBoxContainer::clear()
{
    //maybe faster to delete the layout and re-create it, maybe not as safe?
    int count = d->layout->count();
    for (int i = 0; i < count ;i++)
        d->layout->itemAt(count-i-1)->widget()->hide();
}


void medToolBoxContainer::setOrientation(LayoutOrientation orient)
{
    if (d->layoutOrientation != orient)
    {
        d->layoutOrientation = orient;
        QBoxLayout *layout;

        //remove toolboxes
        QList<medToolBox*> tbs;
        for (int i; i< d->layout->count();i++)
        {
            tbs.append(dynamic_cast<medToolBox*>(d->layout->itemAt(i)->widget()));
        }

        orient == Vertical ? layout = new QVBoxLayout(d->container):
                             layout = new QHBoxLayout(d->container);
        delete d->layout;
        d->layout = layout;
        d->layout->setContentsMargins(2, 4, 0, 0);
        d->layout->addStretch(1);

        //put back toolboxes
        foreach(medToolBox * tb, tbs )
            addToolBox(tb);
    }
}
