/* medDatabasePreviewArrow.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:42:26 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:42:27 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASEPREVIEWARROW_H
#define MEDDATABASEPREVIEWARROW_H

#include <QtGui>

class medDatabasePreviewArrowPrivate;

class medDatabasePreviewArrow : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
     medDatabasePreviewArrow(QPixmap pixmap, QPixmap pressed, QGraphicsItem *parent = 0);
    ~medDatabasePreviewArrow(void);

signals:
    void clicked(void);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    medDatabasePreviewArrowPrivate *d;
};

#endif // MEDDATABASEPREVIEWARROW_H
