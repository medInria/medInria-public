/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medBrowserPacsSearchToolBox.h>

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

medBrowserPacsSearchToolBox::~medBrowserPacsSearchToolBox(void)
{
    delete d;

    d = NULL;
}

void medBrowserPacsSearchToolBox::onSearch(void)
{
    QString name = d->edit->text();
    name.append("*");
    emit search(name);
}
