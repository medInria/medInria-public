#include <medDatabaseNavigatorItemOverlay.h>

// /////////////////////////////////////////////////////////////////
// medDatabaseNavigatorItemOverlay
// /////////////////////////////////////////////////////////////////

class medDatabaseNavigatorItemOverlayPrivate
{
};

medDatabaseNavigatorItemOverlay::medDatabaseNavigatorItemOverlay(QGraphicsItem *parent)
    : QObject(), QGraphicsPixmapItem(parent), d(new medDatabaseNavigatorItemOverlayPrivate)
{
}

medDatabaseNavigatorItemOverlay::~medDatabaseNavigatorItemOverlay()
{
    delete d;
    
    d = NULL;
}

void medDatabaseNavigatorItemOverlay::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit clicked();
}
