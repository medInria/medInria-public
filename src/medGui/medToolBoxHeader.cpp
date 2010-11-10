/* medToolBoxHeader.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 10:15:58 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 10:16:53 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 4
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medToolBoxHeader.h"

class medToolBoxHeaderPrivate
{
public:
    QLabel *label;
};

medToolBoxHeader::medToolBoxHeader(QWidget *parent) : QFrame(parent), d(new medToolBoxHeaderPrivate)
{
    d->label = new QLabel(this);
    d->label->setText("Untitled");
    d->label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 0, 5, 0);
    layout->setSpacing(0);
    layout->addWidget(d->label);
}

medToolBoxHeader::~medToolBoxHeader(void)
{
    delete d;

    d = NULL;
}

void medToolBoxHeader::setTitle(const QString& title)
{
    d->label->setText(title);
}
