/* medToolBoxSource.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 15:20:03 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu May 13 16:34:02 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 18
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

};

medToolBoxSource::medToolBoxSource(QWidget *parent) : medToolBox(parent), d(new medToolBoxSourcePrivate)
{
    QWidget *database_page = new QWidget(this);

    QPushButton *database_page_expand_all_button = new QPushButton("All", database_page);
    QPushButton *database_page_collapse_all_button = new QPushButton("All", database_page);

    connect(database_page_expand_all_button, SIGNAL(clicked()), this, SIGNAL(expandAll()));
    connect(database_page_collapse_all_button, SIGNAL(clicked()), this, SIGNAL(collapseAll()));

    QFormLayout *database_page_layout = new QFormLayout(database_page);
    database_page_layout->addRow("Expand:", database_page_expand_all_button);
    database_page_layout->addRow("Collapse:", database_page_collapse_all_button);
    database_page_layout->setFormAlignment(Qt::AlignHCenter);

    QWidget *filesystem_page = new QWidget(this);

    QPushButton *filesystem_import_button = new QPushButton("Import", filesystem_page);
    QPushButton *filesystem_export_button = new QPushButton("Export", filesystem_page);
    QPushButton *filesystem_view_button   = new QPushButton("View", filesystem_page);

    QFormLayout *filesystem_page_layout = new QFormLayout(filesystem_page);
    filesystem_page_layout->addRow("Current item:", filesystem_import_button);
    filesystem_page_layout->addRow("Current item:", filesystem_export_button);
    filesystem_page_layout->addRow("Current item:", filesystem_view_button);
    filesystem_page_layout->setFormAlignment(Qt::AlignHCenter);

    connect(filesystem_import_button, SIGNAL(clicked()), this, SIGNAL(importCurrent()));
    connect(filesystem_export_button, SIGNAL(clicked()), this, SIGNAL(exportCurrent()));
    connect(filesystem_view_button, SIGNAL(clicked()), this, SIGNAL(viewCurrent()));

    QWidget *pacs_page = new QWidget(this);

    QFormLayout *pacs_page_layout = new QFormLayout(pacs_page);
    pacs_page_layout->addRow("Url:", new QLineEdit(pacs_page));
    pacs_page_layout->addRow("Login:", new QLineEdit(pacs_page));
    pacs_page_layout->addRow("Password:", new QLineEdit(pacs_page));
    pacs_page_layout->addWidget(new QPushButton("Connect", pacs_page));
    pacs_page_layout->setFormAlignment(Qt::AlignHCenter);

    medToolBoxTab *tab = new medToolBoxTab(this);
    tab->addTab(database_page, "Db");
    tab->addTab(filesystem_page, "Fs");
    tab->addTab(pacs_page, "Pc");
    connect(tab, SIGNAL(currentChanged(int)), this, SIGNAL(indexChanged(int)));

    this->setTitle("Source selector");
    this->setWidget(tab);
}

medToolBoxSource::~medToolBoxSource(void)
{
    delete d;

    d = NULL;
}
