/* medToolBox.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  9 19:46:22 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Dec 20 12:57:48 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 254
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
#include "medToolBoxTab.h"

#include <dtkCore/dtkGlobal.h>

class medToolBoxPrivate
{
public:
    medToolBoxHeader *header;
    medToolBoxBody *body;

public:
    QBoxLayout *layout;
};

medToolBox::medToolBox(QWidget *parent) : QWidget(parent), d(new medToolBoxPrivate)
{
    d->header = new medToolBoxHeader(this);
    d->body = new medToolBoxBody(this);

    d->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);
    d->layout->addWidget(d->header);
    d->layout->addWidget(d->body);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

medToolBox::~medToolBox(void)
{
    delete d;

    d = NULL;
}

void medToolBox::setTabWidget(medToolBoxTab *tab)
{
    d->body->setTabWidget(tab);
}

void medToolBox::addWidget(QWidget *widget)
{
    d->body->addWidget(widget);
}

void medToolBox::setTitle(const QString &title)
{
    d->header->setTitle(title);
}

void medToolBox::update(dtkAbstractView *view)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(view);
}

void medToolBox::clear(void)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medToolBox::setOrientation(Qt::Orientation orientation)
{
    switch(orientation) {
    case Qt::Vertical:
        if (d->layout->direction() == QBoxLayout::TopToBottom) {
            return;
        } else {
            d->body->setOrientation(Qt::Vertical);
        }
        break;
    case Qt::Horizontal:
        if (d->layout->direction() == QBoxLayout::LeftToRight) {
            return;
        } else {
            d->body->setOrientation(Qt::Horizontal);
        }
        break;
   }
}

Qt::Orientation medToolBox::orientation (void) const
{
    if(d->layout->direction() == QBoxLayout::LeftToRight)
        return Qt::Horizontal;
    else
        return Qt::Vertical;
}
