/* medDatabasePreviewView.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:44:36 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Sat Mar 20 19:50:42 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 8
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

protected:
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    medDatabasePreviewViewPrivate *d;
};

#endif // MEDDATABASEPREVIEWVIEW_H
