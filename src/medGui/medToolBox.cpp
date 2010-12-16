/* medToolBox.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  9 19:46:22 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 10:18:11 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 209
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medToolBox.h"
#include "medToolBoxHeader.h"
#include "medToolBoxBody.h"
#include "medToolBoxTab.h"

class medToolBoxPrivate
{
public:
    medToolBoxHeader *header;
    medToolBoxBody *body;
    QGridLayout * layout;
    Qt::Orientation layoutOrientation;
    void clearLayout();
};

medToolBox::medToolBox(QWidget *parent) : QWidget(parent), d(new medToolBoxPrivate)
{
    d->header = new medToolBoxHeader(this);
    d->body = new medToolBoxBody(this);
    d->layoutOrientation = Qt::Vertical;

    d->layout = new QGridLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);
    d->layout->addWidget(d->header,0,0,Qt::AlignTop);
    d->layout->addWidget(d->body,1,0);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

medToolBox::~medToolBox(void)
{
    delete d->header;
    delete d->body;
    delete d;

    d = NULL;
}

void medToolBox::setTabWidget(medToolBoxTab *tab)
{
    d->body->setTabWidget(tab);
}


void medToolBox::addWidget(QWidget *widget)
{
    d->body->addWidget(widget);
}

void medToolBox::setTitle(const QString &title)
{
    d->header->setTitle(title);
}

void medToolBox::update(dtkAbstractView *view)
{
    Q_UNUSED(view);
}

void medToolBox::clear()
{
    return;
}

void medToolBox::setOrientation(Qt::Orientation orient)
{
    qDebug()<<"tb :: setOrientation ";
    qDebug()<<"hor:"<< Qt::Horizontal;
    qDebug()<<"switch from"<<d->layoutOrientation;

    if (d->layoutOrientation==orient)
        return;
    d->layoutOrientation = orient;
    qDebug()<<"to"<<orient;
    d->header->hide();
    d->body->hide();
    d->header->setOrientation(orient);
    d->body->setOrientation(orient);

    if (orient == Qt::Vertical)
    {
//        d->layout->setRowStretch (0, 0);
        d->layout->addWidget(d->header,0,0,Qt::AlignTop);
//        d->layout->setRowStretch (0, 1);
//        d->layout->setRowStretch (1, 0);
        d->layout->addWidget(d->body,1,0);
//        d->layout->setRowStretch (1, 1);

    }
    else
    {
//        d->layout->setColumnStretch (0, 0);
        d->layout->addWidget(d->header,0,0,Qt::AlignTop);
//        d->layout->setColumnStretch (0, 1);
//        d->layout->setColumnStretch (1, 0);
        d->layout->addWidget(d->body,0,1);
//        d->layout->setColumnStretch (1, 1);
    }
    d->header->show();
    d->body->show();
}

Qt::Orientation medToolBox::orientation (void) const
{
    return d->layoutOrientation;
}
