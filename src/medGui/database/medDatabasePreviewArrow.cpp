/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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
