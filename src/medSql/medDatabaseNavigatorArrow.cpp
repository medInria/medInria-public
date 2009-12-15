/* medDatabaseNavigatorArrow.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:38:56 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:38:57 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabaseNavigatorArrow.h"

class medDatabaseNavigatorArrowPrivate
{
public:
    QPixmap pixmap;
    QPixmap pressed;
};

medDatabaseNavigatorArrow::medDatabaseNavigatorArrow(QPixmap pixmap, QPixmap pressed, QGraphicsItem *parent) : QGraphicsPixmapItem(pixmap, parent), d(new medDatabaseNavigatorArrowPrivate)
{
    d->pixmap = pixmap;
    d->pressed = pressed;

    this->setZValue(10);
}

medDatabaseNavigatorArrow::~medDatabaseNavigatorArrow(void)
{
    delete d;

    d = NULL;
}

void medDatabaseNavigatorArrow::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    this->setPixmap(d->pressed);
}

void medDatabaseNavigatorArrow::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    this->setPixmap(d->pixmap);

    emit clicked();
}
