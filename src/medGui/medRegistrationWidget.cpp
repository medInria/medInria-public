/* medRegistrationWidget.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Jan 18 11:14:54 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Feb 19 17:43:18 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 29
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDropSite.h"
#include "medRegistrationWidget.h"

#include <QtCore>
#include <QtGui>

class medRegistrationWidgetPrivate
{
public:
    medDropSite *site1;
    medDropSite *site2;

    QPushButton *registerButton;
};

medRegistrationWidget::medRegistrationWidget(QWidget *parent) : QWidget(parent), d(new medRegistrationWidgetPrivate)
{
    d->site1 = new medDropSite(this);
    d->site1->setText("Fixed");

    d->site2 = new medDropSite(this);
    d->site2->setText("Moving");

    d->registerButton = new QPushButton("Register", this);

    QHBoxLayout *sites_layout = new QHBoxLayout;
    sites_layout->addWidget(d->site1);
    sites_layout->addWidget(d->site2);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(sites_layout);
    layout->addWidget(d->registerButton);
}

medRegistrationWidget::~medRegistrationWidget(void)
{
    delete d;

    d = NULL;
}
