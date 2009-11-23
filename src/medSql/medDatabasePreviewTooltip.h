#ifndef MEDDATABASEPREVIEWTOOLTIP_H
#define MEDDATABASEPREVIEWTOOLTIP_H

#include <QtCore>
#include <QtGui>

class medDatabasePreviewTooltip : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

    Q_PROPERTY(float opacity READ opacity WRITE setOpacity)

public:
    medDatabasePreviewTooltip(const QPixmap& pixmap, QGraphicsItem *parent = 0);
};

#endif // MEDDATABASEPREVIEWTOOLTIP_H
