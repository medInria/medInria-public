/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

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

    void setSlice(int nb);

signals:
    void patientClicked(int id);
    void   studyClicked(int id);
    void  seriesClicked(int id);
    void   imageClicked(int id);
    void visualizationRequested(const medDataIndex &);
    void visualizationRequested(int);

    void hoverEntered(QGraphicsSceneHoverEvent* event, medDatabasePreviewItem* item);
    void hoverLeft(QGraphicsSceneHoverEvent* event, medDatabasePreviewItem* item);

protected slots:
    void setImage(const QImage& image);

    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:
    medDatabasePreviewItemPrivate *d;
};


