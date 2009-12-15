/* medDatabaseNavigatorArrow.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:38:51 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:38:52 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASENAVIGATORARROW_H
#define MEDDATABASENAVIGATORARROW_H

#include <QtGui>

class medDatabaseNavigatorArrowPrivate;

class medDatabaseNavigatorArrow : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
     medDatabaseNavigatorArrow(QPixmap pixmap, QPixmap pressed, QGraphicsItem *parent = 0);
    ~medDatabaseNavigatorArrow(void);

signals:
    void clicked(void);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    medDatabaseNavigatorArrowPrivate *d;
};

#endif // MEDDATABASENAVIGATORARROW_H
