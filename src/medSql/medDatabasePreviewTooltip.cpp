#include "medDatabasePreviewTooltip.h"

medDatabasePreviewTooltip::medDatabasePreviewTooltip(const QPixmap& pixmap, QGraphicsItem *parent) : QObject(), QGraphicsPixmapItem(pixmap, parent)
{
    this->setZValue(20);
}
