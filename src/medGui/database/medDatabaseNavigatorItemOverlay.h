/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtGui/QGraphicsPixmapItem>

class medDatabaseNavigatorItemOverlayPrivate;

/**
 * Class for overlay buttons on the database navigator items 
 * When clicked, it emits a signal clicked to be catched elsewhere to take action
 * 
 ***/

class medDatabaseNavigatorItemOverlay : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    
public:
    medDatabaseNavigatorItemOverlay(QGraphicsItem *parent = 0);
    ~medDatabaseNavigatorItemOverlay();
            
signals:
    //! Signal emitted when the overlay button is clicked on
    void clicked();
    
protected:
    //! Handle mouse press event (emits clicked)
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    
private:
    medDatabaseNavigatorItemOverlayPrivate *d;
};

