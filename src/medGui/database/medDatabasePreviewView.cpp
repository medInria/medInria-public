/* medDatabasePreviewView.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:44:41 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Jun 15 18:44:22 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabasePreviewArrow.h"
#include "medDatabasePreviewController.h"
#include "medDatabasePreviewItem.h"
#include "medDatabasePreviewView.h"
#include "medDatabasePreviewTooltip.h"

class medDatabasePreviewViewPrivate
{
public:
};

medDatabasePreviewView::medDatabasePreviewView(QWidget *parent) : QGraphicsView(parent), d(new medDatabasePreviewViewPrivate)
{
    this->viewport()->setMouseTracking(true);

    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    if(medDatabasePreviewController::instance()->orientation() == Qt::Horizontal) {
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    } else {
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

medDatabasePreviewView::~medDatabasePreviewView(void)
{
    delete d;

    d = NULL;
}

void medDatabasePreviewView::setScene(QGraphicsScene *scene)
{
    QGraphicsView::setScene(scene);
}

void medDatabasePreviewView::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Up: emit moveUp(); break;
    case Qt::Key_Down: emit moveDw(); break;
    case Qt::Key_Left: emit moveLt(); break;
    case Qt::Key_Right: emit moveRt(); break;
    default: break;
    }
}

void medDatabasePreviewView::mouseMoveEvent(QMouseEvent *event)
{
    medDatabasePreviewItem *target = dynamic_cast<medDatabasePreviewItem *>(this->scene()->itemAt(this->mapToScene(event->pos())));

    if(target)
        emit hovered(target);

    QGraphicsView::mouseMoveEvent(event);
}

void medDatabasePreviewView::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event);
}
