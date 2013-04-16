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

class medDatabaseNavigatorItem;
class medDatabaseNavigatorItemGroupPrivate;

class medDatabaseNavigatorItemGroup : public QObject, public QGraphicsItem
{
    Q_OBJECT

    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

#if QT_VERSION >= 0x040600
    Q_INTERFACES(QGraphicsItem)
#endif

public:
     medDatabaseNavigatorItemGroup(QGraphicsItem *parent = 0);
    ~medDatabaseNavigatorItemGroup();

    void addItem(medDatabaseNavigatorItem *item);

    void clear();

    void setName(const QString& name);

    void setOrientation (Qt::Orientation orientation);
    Qt::Orientation orientation() const;

public:
    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    medDatabaseNavigatorItemGroupPrivate *d;
};


