#include "medDatabaseNavigatorTooltip.h"

medDatabaseNavigatorTooltip::medDatabaseNavigatorTooltip(const QPixmap& pixmap, QGraphicsItem *parent) : QObject(), QGraphicsPixmapItem(pixmap, parent)
{
    this->setZValue(20);
}
