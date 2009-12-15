/* medDatabaseNavigatorItemGroup.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:39:44 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:39:44 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASENAVIGATORITEMGROUP_H
#define MEDDATABASENAVIGATORITEMGROUP_H

#include <QtGui>

class medDatabaseNavigatorItem;
class medDatabaseNavigatorItemGroupPrivate;

class medDatabaseNavigatorItemGroup : public QObject, public QGraphicsItem
{
    Q_OBJECT

    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

    Q_INTERFACES(QGraphicsItem)

public:
     medDatabaseNavigatorItemGroup(QGraphicsItem *parent = 0);
    ~medDatabaseNavigatorItemGroup(void);

    void addItem(medDatabaseNavigatorItem *item);

    void clear(void);

    void setName(const QString& name);

public:
    QRectF boundingRect(void) const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    medDatabaseNavigatorItemGroupPrivate *d;
};

#endif // MEDDATABASENAVIGATORITEMGROUP_H
