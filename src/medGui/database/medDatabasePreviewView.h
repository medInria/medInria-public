/* medDatabasePreviewView.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:44:36 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Jun 15 18:44:30 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASEPREVIEWVIEW_H
#define MEDDATABASEPREVIEWVIEW_H

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
    ~medDatabasePreviewView(void);

    void setScene(QGraphicsScene *scene);

signals:
    void moveUp(void);
    void moveDw(void);
    void moveRt(void);
    void moveLt(void);
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

#endif // MEDDATABASEPREVIEWVIEW_H
