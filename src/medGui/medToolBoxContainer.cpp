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
    QList<medToolBox *> toolboxes;

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
    if (!toolBox)
        return;
    
    if (!d->toolboxes.contains(toolBox)) {
        d->toolboxes.append(toolBox);
        d->layout->insertWidget(d->layout->count()-1, toolBox, 0, Qt::AlignTop);
    }
}

void medToolBoxContainer::removeToolBox(medToolBox *toolBox)
{
    d->toolboxes.removeOne( toolBox );
    d->layout->removeWidget(toolBox);
}


void medToolBoxContainer::clear()
{
    //maybe faster to delete the layout and re-create it, maybe not as safe?
    foreach(medToolBox *tb, d->toolboxes) {
        tb->hide();
        d->layout->removeWidget(tb);
    }
    d->toolboxes.clear();
    /*
    int count = d->layout->count();
    for (int i = 0; i < count ;i++)
        d->layout->itemAt(count-i-1)->widget()->hide();
     */
}

QList<medToolBox*> medToolBoxContainer::toolBoxes(void) const
{
    return d->toolboxes;
}

void medToolBoxContainer::setOrientation(LayoutOrientation orient)
{
    if (d->layoutOrientation != orient)
    {
        d->layoutOrientation = orient;
        QBoxLayout *layout;

        delete d->layout;

        orient == Vertical ? layout = new QVBoxLayout(d->container):
                             layout = new QHBoxLayout(d->container);
        d->layout = layout;
        d->layout->setContentsMargins(2, 4, 0, 0);
        d->layout->addStretch(1);

        //put back toolboxes
       foreach(medToolBox * tb, d->toolboxes ) {
           if (orient==Vertical)
               d->layout->insertWidget(d->layout->count()-1, tb, 0, Qt::AlignTop);
           else 
               d->layout->insertWidget(d->layout->count()-1, tb, 0, Qt::AlignLeft);
       }
            //addToolBox(tb);
       
       this->updateGeometry();
    }
}
