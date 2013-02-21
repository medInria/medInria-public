/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtGui>
#include <medCore/medDataIndex.h>


class medDatabasePreviewItem;
class medDatabasePreviewItemGroupPrivate;

class medDatabasePreviewItemGroup : public QObject, public QGraphicsItem
{
    Q_OBJECT

    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

#if QT_VERSION >= 0x040600
    Q_INTERFACES(QGraphicsItem)
#endif

public:
     medDatabasePreviewItemGroup(QGraphicsItem *parent = 0);
    ~medDatabasePreviewItemGroup();

    void addItem(medDatabasePreviewItem *item);

    void clear();

    medDatabasePreviewItem *item(int index);

public:
    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    medDatabasePreviewItemGroupPrivate *d;
};


