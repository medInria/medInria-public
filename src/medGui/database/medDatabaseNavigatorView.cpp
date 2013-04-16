/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseNavigatorItem.h"

#include "medDatabaseNavigatorView.h"
#include "medDatabaseNavigatorController.h"

class medDatabaseNavigatorViewPrivate
{
public:
  Qt::Orientation orientation;
};

medDatabaseNavigatorView::medDatabaseNavigatorView(QWidget *parent) : QGraphicsView(parent), d(new medDatabaseNavigatorViewPrivate)
{
    this->viewport()->setMouseTracking(true);

    d->orientation = medDatabaseNavigatorController::instance()->orientation();

    if(d->orientation == Qt::Horizontal) {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    } else {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    }

    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    this->setFrameShape(QFrame::NoFrame);

    this->setStyleSheet("background: transparent;");
}

medDatabaseNavigatorView::~medDatabaseNavigatorView(void)
{
    delete d;

    d = NULL;
}

void medDatabaseNavigatorView::setScene(QGraphicsScene *scene)
{
    QGraphicsView::setScene(scene);
}

void medDatabaseNavigatorView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);

    medDatabaseNavigatorItem *target = dynamic_cast<medDatabaseNavigatorItem *>(this->scene()->itemAt(this->mapToScene(event->pos())));

    if(!target)
        return;

    emit hovered(target);
}

void medDatabaseNavigatorView::setOrientation (Qt::Orientation orientation)
{
    d->orientation = orientation;

    if(d->orientation == Qt::Horizontal) {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    } else {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    }
}

Qt::Orientation medDatabaseNavigatorView::orientation() const
{
    return d->orientation;
}
