/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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

medBrowserSourceSelectorToolBox::~medBrowserSourceSelectorToolBox(void)
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


