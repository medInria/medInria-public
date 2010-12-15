/* medToolBox.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  9 19:46:22 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 10:18:11 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 209
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medToolBox.h"
#include "medToolBoxHeader.h"
#include "medToolBoxBody.h"

class medToolBoxPrivate
{
public:
    medToolBoxHeader *header;
    medToolBoxBody *body;
    QGridLayout * layout;
    Qt::Orientation layoutOrientation;
};

medToolBox::medToolBox(QWidget *parent) : QWidget(parent), d(new medToolBoxPrivate)
{
    d->header = new medToolBoxHeader(this);
    d->body = new medToolBoxBody(this);
    d->layoutOrientation = Qt::Vertical;

    d->layout = new QGridLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);
    d->layout->addWidget(d->header,0,0,Qt::AlignTop);
    d->layout->addWidget(d->body,1,0);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

medToolBox::~medToolBox(void)
{
    delete d->header;
    delete d->body;
    delete d;

    d = NULL;
}

void medToolBox::setWidget(QWidget *widget)
{
    d->body->setWidget(widget);
}

void medToolBox::setTitle(const QString &title)
{
    d->header->setTitle(title);
}

void medToolBox::update(dtkAbstractView *view)
{
    Q_UNUSED(view);
}

void medToolBox::clear()
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
}


void medToolBox::addWidget(QWidget *widget)
{
    if (!widget)
        return;

    if (d->layoutOrientation==Qt::Vertical)
    {
        d->layout->setRowStretch (d->layout->count(), 0);
        d->layout->addWidget(toolBox, d->layout->count(), 0, Qt::AlignTop);
        d->layout->setRowStretch (d->layout->count(), 1);
    }
    else
    {
        d->layout->setColumnStretch (d->layout->count(), 0);
        d->layout->addWidget(toolBox, 0, d->layout->count(), Qt::AlignTop);
        d->layout->setColumnStretch (d->layout->count(), 1);
    }
}

void medToolBox::setOrientation(Qt::Orientation orient)
{
    if (d->layoutOrientation==orient)
        return;

    d->layoutOrientation = orient;

    this->clear();

    //put back widgets
    d->layout->addWidget(d->header,0,0);
    foreach(medToolBox * tb, d->toolboxes ) {
      addToolBox (tb);
    }
}

Qt::Orientation medToolBoxContainer::orientation (void) const
{
    return d->layoutOrientation;
}
