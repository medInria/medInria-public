/* medToolBoxBody.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 10:17:16 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 10:18:50 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 3
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medToolBoxBody.h"
#include <medGui/medToolBoxTab.h>

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
        qDebug()<<"botttom";
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
    for (int i=0; i<=d->layout->count(); i++) {
      if (d->layoutOrientation==Qt::Vertical)
        d->layout->setRowStretch (i, 0);
      else
        d->layout->setColumnStretch (i, 0);
    }
    foreach(QWidget *wid, d->widgets) {
        wid->hide();
        d->layout->removeWidget(wid);
    }
}
void medToolBoxBody::setOrientation(Qt::Orientation orientation)
{
    qDebug()<<"Body :: setOrientation ";
    if (d->layoutOrientation==orientation)
        return;

    d->layoutOrientation = orientation;

    this->clear();
    if (d->tab)
    {
        d->tab->setOrientation(orientation);
    }
    foreach(QWidget * wid, d->widgets ) {
      //addToolBox also sets the orientation of the toolboxes
      qDebug()<<"adding widget";
      addWidget (wid);
      wid->show();
    }
}
