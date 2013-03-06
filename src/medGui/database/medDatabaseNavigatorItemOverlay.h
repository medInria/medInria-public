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
    ~medDatabaseNavigatorItemOverlay(void);
            
signals:
    //! Signal emitted when the overlay button is clicked on
    void clicked();
    
protected:
    //! Handle mouse press event (emits clicked)
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    
private:
    medDatabaseNavigatorItemOverlayPrivate *d;
};

