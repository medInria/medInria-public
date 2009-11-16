/* medStatusPanel.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sun Oct 11 13:02:28 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Nov  3 09:42:50 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 121
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medStatusPanel.h"

#include <QtGui>

// /////////////////////////////////////////////////////////////////
// medStatusPanel
// /////////////////////////////////////////////////////////////////

class medStatusPanelPrivate
{
public:
    QHBoxLayout *layout;
};

medStatusPanel::medStatusPanel(QWidget *parent) : QFrame(parent), d(new medStatusPanelPrivate)
{
    d->layout = new QHBoxLayout(this);
    d->layout->setContentsMargins(5, 0, 5, 0);
    d->layout->setSpacing(12);
}

medStatusPanel::~medStatusPanel(void)
{
    delete d;

    d = NULL;
}

void medStatusPanel::addSpacer(void)
{
    d->layout->addStretch(2);
}

void medStatusPanel::addWidget(QWidget *widget)
{
    d->layout->addWidget(widget);
}

void medStatusPanel::removeWidget(QWidget *widget)
{
    d->layout->removeWidget(widget);
}
