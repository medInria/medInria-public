/* medBrowserSourceSelectorToolBox.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 15:20:03 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 10:28:53 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 115
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medBrowserSourceSelectorToolBox.h"
#include <medToolBoxTab.h>

#include <QtGui>

class medBrowserSourceSelectorToolBoxPrivate
{
public:
    QList <QWidget *> additionalWidgets;
    medToolBoxTab *tab;
};

medBrowserSourceSelectorToolBox::medBrowserSourceSelectorToolBox(QWidget *parent) : medToolBox(parent), d(new medBrowserSourceSelectorToolBoxPrivate)
{
    d->tab = new medToolBoxTab(this);
    connect(d->tab, SIGNAL(currentChanged(int)), this, SIGNAL(indexChanged(int)));

    this->setTitle(tr("Source selector"));
    this->setTabWidget(d->tab);
}

medBrowserSourceSelectorToolBox::~medBrowserSourceSelectorToolBox()
{
    delete d;
    d = NULL;
}

void medBrowserSourceSelectorToolBox::addTab(QString tabName, QWidget *widget, QString description)
{
    d->additionalWidgets.push_back(widget);
    int i = d->tab->addTab(d->additionalWidgets.back(), tabName);
	d->tab->setTabToolTip (i, description);
}

void medBrowserSourceSelectorToolBox::setCurrentTab(int current)
{
	d->tab->setCurrentIndex(current);
}


