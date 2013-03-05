/* medBrowserPacsSearchToolBox.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct  5 15:49:05 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct  6 11:38:06 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 121
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medBrowserPacsSearchToolBox.h"

#include <QtGui>

class medBrowserPacsSearchToolBoxPrivate
{
public:
    QLineEdit *edit;
    QPushButton *button;
};

medBrowserPacsSearchToolBox::medBrowserPacsSearchToolBox(QWidget *parent) : medToolBox(parent), d(new medBrowserPacsSearchToolBoxPrivate)
{
    QWidget *page = new QWidget(this);

    d->edit = new QLineEdit(page);
    d->button = new QPushButton("Search", page);

    QHBoxLayout *layout = new QHBoxLayout(page);
    layout->addWidget(d->edit);
    layout->addWidget(d->button);

    this->setTitle("Search");
    this->addWidget(page);

    connect(d->edit, SIGNAL(returnPressed()), this, SLOT(onSearch()));
    connect(d->button, SIGNAL(clicked()), this, SLOT(onSearch()));
}

medBrowserPacsSearchToolBox::~medBrowserPacsSearchToolBox()
{
    delete d;

    d = NULL;
}

void medBrowserPacsSearchToolBox::onSearch()
{
    QString name = d->edit->text();
    name.append("*");
    emit search(name);
}
