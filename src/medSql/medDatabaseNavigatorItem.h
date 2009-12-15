/* medDatabaseNavigatorItem.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:39:28 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:39:28 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
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

class medDatabaseNavigatorItemPrivate;

class medDatabaseNavigatorItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
     medDatabaseNavigatorItem(int patientId = -1, int studyId = -1, int seriesId = -1, int imageId = -1, const QString& path = QString(), QGraphicsItem *parent = 0);
    ~medDatabaseNavigatorItem(void);

    medDatabaseNavigatorItem *clone(void);

    int patientId(void) const;
    int   studyId(void) const;
    int  seriesId(void) const;
    int   imageId(void) const;

    QString path(void) const;

signals:
    void patientClicked(int id);
    void   studyClicked(int id);
    void  seriesClicked(int id);
    void   imageClicked(int id);

protected slots:
    void setImage(const QImage& image);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    medDatabaseNavigatorItemPrivate *d;
};

#endif // MEDDATABASENAVIGATORITEM_H
