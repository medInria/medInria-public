/* medDatabasePreviewArrow.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:42:30 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:42:31 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabasePreviewArrow.h"

class medDatabasePreviewArrowPrivate
{
public:
    QPixmap pixmap;
    QPixmap pressed;
};

medDatabasePreviewArrow::medDatabasePreviewArrow(QPixmap pixmap, QPixmap pressed, QGraphicsItem *parent) : QGraphicsPixmapItem(pixmap, parent), d(new medDatabasePreviewArrowPrivate)
{
    d->pixmap = pixmap;
    d->pressed = pressed;

    this->setZValue(10);
}

medDatabasePreviewArrow::~medDatabasePreviewArrow(void)
{
    delete d;

    d = NULL;
}

void medDatabasePreviewArrow::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    this->setPixmap(d->pressed);
}

void medDatabasePreviewArrow::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    this->setPixmap(d->pixmap);

    emit clicked();
}
