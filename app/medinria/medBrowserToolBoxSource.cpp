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
#include <medGui/medToolBoxTab.h>

#include <QtGui>

class medBrowserToolBoxSourcePrivate
{
public:
    QWidget *database_page;
    QWidget *filesystem_page;
    QWidget *pacs_page;
		QList <QWidget *> additional_widgets;
	
		medToolBoxTab *tab;

    QVBoxLayout *filesystem_layout;
    QVBoxLayout *pacs_layout;
};

medBrowserToolBoxSource::medBrowserToolBoxSource(QWidget *parent) : medToolBox(parent), d(new medBrowserToolBoxSourcePrivate)
{
    d->database_page = new QWidget(this);
    d->filesystem_page = new QWidget(this);
    d->pacs_page = new QWidget(this);

    d->filesystem_layout = new QVBoxLayout(d->filesystem_page);
    d->filesystem_layout->setContentsMargins(0, 0, 0, 0);
    d->filesystem_layout->setSpacing(0);
   
    d->pacs_layout = new QVBoxLayout(d->pacs_page);
    d->pacs_layout->setContentsMargins(0, 0, 0, 0);
    d->pacs_layout->setSpacing(0);

    d->tab = new medToolBoxTab(this);
    d->tab->addTab(d->database_page, "Db");
    d->tab->addTab(d->filesystem_page, "Fs");
    d->tab->addTab(d->pacs_page, "Pc");
    connect(d->tab, SIGNAL(currentChanged(int)), this, SIGNAL(indexChanged(int)));

    this->setTitle("Source selector");
    this->setTabWidget(d->tab);
}

medBrowserToolBoxSource::~medBrowserToolBoxSource(void)
{
    delete d;

    d = NULL;
}

void medBrowserToolBoxSource::addAdditionalTab(QString tabName, QWidget *widget)
{
	d->additional_widgets.push_back(widget);
	d->tab->addTab(d->additional_widgets.back(), tabName);
}

void medBrowserToolBoxSource::setFileSystemWidget(QWidget *widget)
{
    d->filesystem_layout->addWidget(widget);
}

void medBrowserToolBoxSource::setPacsWidget( QWidget* widget )
{
    d->pacs_layout->addWidget(widget);
}


