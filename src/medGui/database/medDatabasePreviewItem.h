/* medDatabasePreviewItem.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:42:56 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:42:57 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASEPREVIEWITEM_H
#define MEDDATABASEPREVIEWITEM_H

#include <QtGui>
#include <QtGui/QGraphicsPixmapItem>

class medDataIndex;
class medDatabasePreviewItemPrivate;

class medDatabasePreviewItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
     medDatabasePreviewItem(const medDataIndex &index, QGraphicsItem *parent = 0);
    ~medDatabasePreviewItem();

    medDatabasePreviewItem *clone();

    medDataIndex dataIndex() const;

    void setAllowDragging(bool isDraggingAllowed);

signals:
    void patientClicked(int id);
    void   studyClicked(int id);
    void  seriesClicked(int id);
    void   imageClicked(int id);

    void hoverEntered(QGraphicsSceneHoverEvent* event, medDatabasePreviewItem* item);
    void hoverLeft(QGraphicsSceneHoverEvent* event, medDatabasePreviewItem* item);

protected slots:
    void setImage(const QImage& image);

    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:
    medDatabasePreviewItemPrivate *d;
};

#endif // MEDDATABASEPREVIEWITEM_H
