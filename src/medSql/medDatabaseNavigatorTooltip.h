#ifndef MEDDATABASENAVIGATORTOOLTIP_H
#define MEDDATABASENAVIGATORTOOLTIP_H

#include <QtCore>
#include <QtGui>

class medDatabaseNavigatorTooltip : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

    Q_PROPERTY(float opacity READ opacity WRITE setOpacity)

public:
    medDatabaseNavigatorTooltip(const QPixmap& pixmap, QGraphicsItem *parent = 0);
};

#endif // MEDDATABASENAVIGATORTOOLTIP_H
