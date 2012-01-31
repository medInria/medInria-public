/* medBrowserToolBoxSource.cpp ---
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

#include "medBrowserToolBoxSource.h"
#include <medToolBoxTab.h>

#include <QtGui>

class medBrowserToolBoxSourcePrivate
{
public:
    QList <QWidget *> additional_widgets;
    medToolBoxTab *tab;
};

medBrowserToolBoxSource::medBrowserToolBoxSource(QWidget *parent) : medToolBox(parent), d(new medBrowserToolBoxSourcePrivate)
{
    d->tab = new medToolBoxTab(this);
    connect(d->tab, SIGNAL(currentChanged(int)), this, SIGNAL(indexChanged(int)));

    this->setTitle(tr("Source selector"));
    this->setTabWidget(d->tab);
}

medBrowserToolBoxSource::~medBrowserToolBoxSource(void)
{
    delete d;
    d = NULL;
}

void medBrowserToolBoxSource::addTab(QString tabName, QWidget *widget, QString description)
{
	d->additional_widgets.push_back(widget);
	int i = d->tab->addTab(d->additional_widgets.back(), tabName);
	d->tab->setTabToolTip (i, description);
}



