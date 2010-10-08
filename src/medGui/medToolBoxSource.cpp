/* medToolBoxSource.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 15:20:03 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Oct  5 16:07:54 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 114
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medToolBoxSource.h"

#include <QtGui>

class medToolBoxSourcePrivate
{
public:
    QWidget *database_page;
    QWidget *filesystem_page;
    QWidget *pacs_page;

    QVBoxLayout *filseystem_layout;
};

medToolBoxSource::medToolBoxSource(QWidget *parent) : medToolBox(parent), d(new medToolBoxSourcePrivate)
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

medToolBoxSource::~medToolBoxSource(void)
{
    delete d;

    d = NULL;
}

void medToolBoxSource::setFileSystemWidget(QWidget *widget)
{
    d->filseystem_layout->addWidget(widget);
}
