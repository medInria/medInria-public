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
};

medToolBox::medToolBox(QWidget *parent) : QWidget(parent), d(new medToolBoxPrivate)
{
    d->header = new medToolBoxHeader(this);
    d->body = new medToolBoxBody(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->header);
    layout->addWidget(d->body);

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
