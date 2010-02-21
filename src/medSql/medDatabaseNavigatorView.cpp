/* medDatabaseNavigatorView.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:41:46 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Sun Feb 21 13:39:42 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 14
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabaseNavigatorController.h"
#include "medDatabaseNavigatorItem.h"
#include "medDatabaseNavigatorView.h"

class medDatabaseNavigatorViewPrivate
{
public:
};

medDatabaseNavigatorView::medDatabaseNavigatorView(QWidget *parent) : QGraphicsView(parent), d(new medDatabaseNavigatorViewPrivate)
{
    this->viewport()->setMouseTracking(true);

    this->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    if(medDatabaseNavigatorController::instance()->orientation() == Qt::Horizontal) {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    } else {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
