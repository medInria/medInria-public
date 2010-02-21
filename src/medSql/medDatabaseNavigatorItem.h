/* medDatabaseNavigatorItem.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:39:28 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Sun Feb 21 13:37:12 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 5
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
// medDatabaseNavigatorItemLabel
// /////////////////////////////////////////////////////////////////

class medDatabaseNavigatorItemLabel : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
     medDatabaseNavigatorItemLabel(QGraphicsItem *parent = 0);
    ~medDatabaseNavigatorItemLabel(void);

    void setText(const QString& text);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QString text;
};

// /////////////////////////////////////////////////////////////////
// medDatabaseNavigatorItemCount
// /////////////////////////////////////////////////////////////////

class medDatabaseNavigatorItemCount : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
     medDatabaseNavigatorItemCount(QGraphicsItem *parent = 0);
    ~medDatabaseNavigatorItemCount(void);

    void setCount(int count);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    int count;
};

// /////////////////////////////////////////////////////////////////
// medDatabaseNavigatorItemChecker
// /////////////////////////////////////////////////////////////////

class medDatabaseNavigatorItemCheckerPrivate;

class medDatabaseNavigatorItemChecker : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
     medDatabaseNavigatorItemChecker(QGraphicsItem *parent = 0);
    ~medDatabaseNavigatorItemChecker(void);

public slots:
    void   check(void);
    void uncheck(void);

    void setChecked(bool checked);

signals:
    void   checked(void);
    void unchecked(void);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    medDatabaseNavigatorItemCheckerPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// medDatabaseNavigatorItem
// /////////////////////////////////////////////////////////////////

class medDatabaseNavigatorItemPrivate;

class medDatabaseNavigatorItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
     medDatabaseNavigatorItem(int patientId = -1, int studyId = -1, int seriesId = -1, int imageId = -1, const QString& path = QString(), QGraphicsItem *parent = 0);
    ~medDatabaseNavigatorItem(void);

    medDatabaseNavigatorItem *clone(void);

    int patientId(void) const;
    int   studyId(void) const;
    int  seriesId(void) const;
    int   imageId(void) const;

    QString path(void) const;

    void setup(void);

signals:
    void patientClicked(int id);
    void   studyClicked(int id);
    void  seriesClicked(int id);
    void   imageClicked(int id);

protected slots:
    void   checked(void);
    void unchecked(void);

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
