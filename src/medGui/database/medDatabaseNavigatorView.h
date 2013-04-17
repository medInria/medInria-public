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

class medDatabaseNavigatorItem;
class medDatabaseNavigatorViewPrivate;
class medDatabaseNavigatorTooltip;

class medDatabaseNavigatorView : public QGraphicsView
{
    Q_OBJECT

public:
     medDatabaseNavigatorView(QWidget *parent = 0);
    ~medDatabaseNavigatorView();

    void setScene(QGraphicsScene *scene);

    Qt::Orientation orientation() const;
    
public slots:
    void setOrientation (Qt::Orientation orientation);

signals:
    void hovered(medDatabaseNavigatorItem *item);

protected:
    void mouseMoveEvent(QMouseEvent *event);

private:
    medDatabaseNavigatorViewPrivate *d;
};


