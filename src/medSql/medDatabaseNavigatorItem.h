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

// /////////////////////////////////////////////////////////////////
// medDatabaseNavigatorItem
// /////////////////////////////////////////////////////////////////

class medDatabaseNavigatorItemPrivate;

class medDatabaseNavigatorItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
     medDatabaseNavigatorItem();
     medDatabaseNavigatorItem(int patientId, int studyId, int seriesId, int imageId, const QString& path, const QString &text="", QGraphicsItem *parent = 0);
     medDatabaseNavigatorItem(int patientId, int studyId, int seriesId, int imageId, const QImage& image, const QString &text="", QGraphicsItem *parent = 0);
    ~medDatabaseNavigatorItem(void);

    medDatabaseNavigatorItem *clone(void);

    int patientId(void) const;
    int   studyId(void) const;
    int  seriesId(void) const;
    int   imageId(void) const;

    QString text (void) const;
    
    QString path(void) const;

    void setup(void);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
    void patientClicked(int id);
    void   studyClicked(int id);
    void  seriesClicked(int id);
    void   imageClicked(int id);

protected slots:
    void setImage(const QImage& image);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void  mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    medDatabaseNavigatorItemPrivate *d;
};

#endif // MEDDATABASENAVIGATORITEM_H
