/* medDatabaseNavigatorItem.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:39:28 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Thu May 13 20:12:20 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASENAVIGATORITEM_H
#define MEDDATABASENAVIGATORITEM_H

#include <QtGui/QGraphicsPixmapItem>

#include <medCore/medDataIndex.h>

// /////////////////////////////////////////////////////////////////
// medDatabaseNavigatorItem
// /////////////////////////////////////////////////////////////////

class medDatabaseNavigatorItemPrivate;

class medDatabaseNavigatorItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
     medDatabaseNavigatorItem(const medDataIndex &index = medDataIndex(), QGraphicsItem *parent = 0);
    ~medDatabaseNavigatorItem(void);

    medDatabaseNavigatorItem *clone(void);

    medDataIndex dataIndex() const;

    QString text (void) const;

    void setup(void);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public slots:
    void saveData();    
    void exportData();    
    void deleteData();    
    
signals:
    void itemClicked(const medDataIndex&);

protected slots:
    void setImage(const QImage& image);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void  mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    medDatabaseNavigatorItemPrivate *d;
};

#endif // MEDDATABASENAVIGATORITEM_H
