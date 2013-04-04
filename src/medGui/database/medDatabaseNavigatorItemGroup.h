/* medDatabaseNavigatorItemGroup.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:39:44 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Mar  3 21:54:06 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 2
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

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


