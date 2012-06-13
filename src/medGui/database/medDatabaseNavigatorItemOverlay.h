#ifndef medDatabaseNavigatorItemOverlay_h
#define medDatabaseNavigatorItemOverlay_h

#include <QtGui/QGraphicsPixmapItem>

class medDatabaseNavigatorItemOverlayPrivate;

class medDatabaseNavigatorItemOverlay : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    
public:
    medDatabaseNavigatorItemOverlay(QGraphicsItem *parent = 0);
    ~medDatabaseNavigatorItemOverlay(void);
            
signals:
    void clicked();
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    
private:
    medDatabaseNavigatorItemOverlayPrivate *d;
};

#endif