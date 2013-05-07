/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>
#include <QtGui>

class medDatabasePreviewItem;
class medDataIndex;
class medDatabasePreviewViewPrivate;
class medDatabasePreviewTooltip;

class medDatabasePreviewView : public QGraphicsView
{
    Q_OBJECT

public:
     medDatabasePreviewView(QWidget *parent = 0);
    ~medDatabasePreviewView();

    void setScene(QGraphicsScene *scene);

    void setAcceptWheelEvent(bool accept);

signals:
    void moveUp();
    void moveDw();
    void moveRt();
    void moveLt();
    void hovered(medDatabasePreviewItem *item);

    void objectDropped (const medDataIndex& index);

protected:
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    medDatabasePreviewViewPrivate *d;
};


