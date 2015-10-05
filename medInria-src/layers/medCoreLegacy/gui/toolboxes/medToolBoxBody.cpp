/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medToolBoxBody.h>
#include <medToolBoxTab.h>

class medToolBoxBodyPrivate
{
public:
    QGridLayout *layout;
    QList<QWidget *> widgets;
    Qt::Orientation layoutOrientation;
    medToolBoxTab * tab;
};

medToolBoxBody::medToolBoxBody(QWidget *parent) : QFrame(parent), d(new medToolBoxBodyPrivate)
{    
    d->layout = new QGridLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);
    d->layoutOrientation = Qt::Vertical;
    d->tab = NULL;
}

medToolBoxBody::~medToolBoxBody(void)
{
    delete d;

    d = NULL;
}

void medToolBoxBody::addWidget(QWidget *widget)
{
    if (!widget)
        return;

    d->widgets.append(widget);

    if (d->layoutOrientation == Qt::Vertical) {
        d->layout->setRowStretch (d->layout->count(), 0);
        d->layout->addWidget(widget, d->layout->count(),
                             0, Qt::AlignTop);
        d->layout->setRowStretch (d->layout->count(), 1);
    }
    else {
        d->layout->setColumnStretch (d->layout->count(), 0);
        d->layout->addWidget(widget, 0,
                             d->layout->count(), Qt::AlignTop);
        d->layout->setColumnStretch (d->layout->count(), 1);
    }

    widget->setParent(this);
}

void medToolBoxBody::setTabWidget(medToolBoxTab * tab)
{
    if (!d->widgets.empty()|| !tab)
        return;

    d->tab = tab;
    addWidget(tab);
}

void medToolBoxBody::clear()
{
    for (int i=0; i<=d->layout->count(); i++)
    {
      if (d->layoutOrientation==Qt::Vertical)
        d->layout->setRowStretch (i, 0);
      else
        d->layout->setColumnStretch (i, 0);
    }
    
    foreach(QWidget *wid, d->widgets)
    {
        wid->hide();
        d->layout->removeWidget(wid);
    }
    
    d->widgets.clear();
}
void medToolBoxBody::setOrientation(Qt::Orientation orientation)
{
    if (d->layoutOrientation==orientation)
        return;

    d->layoutOrientation = orientation;

    this->clear();


    foreach(QWidget * wid, d->widgets ) {
      //addToolBox also sets the orientation of the toolboxes
      addWidget (wid);
      wid->show();
    }
}
