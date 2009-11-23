#ifndef MEDDATABASEPREVIEWITEMGROUP_H
#define MEDDATABASEPREVIEWITEMGROUP_H

#include <QtGui>

class medDatabasePreviewItem;
class medDatabasePreviewItemGroupPrivate;

class medDatabasePreviewItemGroup : public QObject, public QGraphicsItem
{
    Q_OBJECT

    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

    Q_INTERFACES(QGraphicsItem)

public:
     medDatabasePreviewItemGroup(QGraphicsItem *parent = 0);
    ~medDatabasePreviewItemGroup(void);

public:
    void addItem(medDatabasePreviewItem *item);

public:
    QRectF boundingRect(void) const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    medDatabasePreviewItemGroupPrivate *d;
};

#endif // MEDDATABASEPREVIEWITEMGROUP_H
