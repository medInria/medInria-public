/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtGui>

class medDatabasePreviewArrowPrivate;

class medDatabasePreviewArrow : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
     medDatabasePreviewArrow(QPixmap pixmap, QPixmap pressed, QGraphicsItem *parent = 0);
    ~medDatabasePreviewArrow();

signals:
    void clicked();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    medDatabasePreviewArrowPrivate *d;
};


