/* medDatabaseNavigatorView.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:41:42 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:41:42 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#pragma once

#include <QtCore>
#include <QtGui>

class medDatabaseNavigatorItem;
class medDatabaseNavigatorViewPrivate;
class medDatabaseNavigatorTooltip;

class medDatabaseNavigatorView : public QGraphicsView
{
    Q_OBJECT

public:
     medDatabaseNavigatorView(QWidget *parent = 0);
    ~medDatabaseNavigatorView(void);

    void setScene(QGraphicsScene *scene);

    Qt::Orientation orientation (void) const;
    
public slots:
    void setOrientation (Qt::Orientation orientation);

signals:
    void hovered(medDatabaseNavigatorItem *item);

protected:
    void mouseMoveEvent(QMouseEvent *event);

private:
    medDatabaseNavigatorViewPrivate *d;
};


