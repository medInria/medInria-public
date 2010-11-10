/* medToolBoxBody.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 10:17:16 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 10:18:50 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 3
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medToolBoxBody.h"

class medToolBoxBodyPrivate
{
public:
    QVBoxLayout *layout;
};

medToolBoxBody::medToolBoxBody(QWidget *parent) : QFrame(parent), d(new medToolBoxBodyPrivate)
{    
    d->layout = new QVBoxLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
}

medToolBoxBody::~medToolBoxBody(void)
{
    delete d->layout;
    delete d;

    d = NULL;
}

void medToolBoxBody::setWidget(QWidget *widget)
{
    d->layout->addWidget(widget);

    widget->setParent(this);
}
