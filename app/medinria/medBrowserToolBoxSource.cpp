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

    QVBoxLayout *filseystem_layout;
};

medBrowserToolBoxSource::medBrowserToolBoxSource(QWidget *parent) : medToolBox(parent), d(new medBrowserToolBoxSourcePrivate)
{
    d->database_page = new QWidget(this);
    d->filesystem_page = new QWidget(this);
    d->pacs_page = new QWidget(this);

    d->filseystem_layout = new QVBoxLayout(d->filesystem_page);
    d->filseystem_layout->setContentsMargins(0, 0, 0, 0);
    d->filseystem_layout->setSpacing(0);
    
    medToolBoxTab *tab = new medToolBoxTab(this);
    tab->addTab(d->database_page, "Db");
    tab->addTab(d->filesystem_page, "Fs");
    tab->addTab(d->pacs_page, "Pc");
    connect(tab, SIGNAL(currentChanged(int)), this, SIGNAL(indexChanged(int)));

    this->setTitle("Source selector");
    this->setWidget(tab);
}

medBrowserToolBoxSource::~medBrowserToolBoxSource(void)
{
    delete d;

    d = NULL;
}

void medBrowserToolBoxSource::setFileSystemWidget(QWidget *widget)
{
    d->filseystem_layout->addWidget(widget);
}
