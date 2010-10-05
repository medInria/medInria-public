/* medToolBoxSource.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 15:20:03 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Sun Jun 13 21:39:19 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 27
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

    QFormLayout *pacs_page_layout = new QFormLayout(d->pacs_page);
    //pacs_page_layout->addRow("Url:", new QLineEdit(d->pacs_page));
    //pacs_page_layout->addRow("Login:", new QLineEdit(d->pacs_page));
    //pacs_page_layout->addRow("Password:", new QLineEdit(d->pacs_page));
    QPushButton* echoButton = new QPushButton("EchoTest", d->pacs_page);
    pacs_page_layout->addWidget(echoButton);
    QPushButton* findButton = new QPushButton("FindTest", d->pacs_page);
    pacs_page_layout->addWidget(findButton);
    pacs_page_layout->setFormAlignment(Qt::AlignHCenter);
    connect(echoButton,SIGNAL(pressed()), this, SIGNAL(echoPressed()));
    connect(findButton,SIGNAL(pressed()), this, SIGNAL(findPressed()));

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
