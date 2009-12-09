#include "medDatabaseNavigatorController.h"
#include "medDatabaseNavigatorItem.h"
#include "medDatabaseNavigatorItemGroup.h"

#include <QtCore>

class medDatabaseNavigatorItemGroupHeader : public QGraphicsItem
{
public:
     medDatabaseNavigatorItemGroupHeader(QGraphicsItem *parent = 0);
    ~medDatabaseNavigatorItemGroupHeader(void);

    void setName(const QString& name);

    QRectF boundingRect(void) const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QString name;
};

medDatabaseNavigatorItemGroupHeader::medDatabaseNavigatorItemGroupHeader(QGraphicsItem *parent) : QGraphicsItem(parent)
{

}

medDatabaseNavigatorItemGroupHeader::~medDatabaseNavigatorItemGroupHeader(void)
{

}

void medDatabaseNavigatorItemGroupHeader::setName(const QString& name)
{
    this->name = name;
}

QRectF medDatabaseNavigatorItemGroupHeader::boundingRect(void) const
{
    qreal group_width  = medDatabaseNavigatorController::instance()->groupWidth();
    qreal group_height = medDatabaseNavigatorController::instance()->groupHeight();

    if(medDatabaseNavigatorController::instance()->orientation() == Qt::Horizontal)
        return QRectF(0, 0, 30, 1000);
    else
        return QRectF(0, 0, 1000, 30);
}

void medDatabaseNavigatorItemGroupHeader::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if(medDatabaseNavigatorController::instance()->orientation() == Qt::Horizontal) {

        QLinearGradient gradient;
        gradient.setStart(0, 0);
        gradient.setFinalStop(30, 0);
        gradient.setColorAt(0, QColor("#3b3b3c"));
        gradient.setColorAt(1, QColor("#2d2d2f"));

        painter->save();
        painter->setPen(Qt::black);
        painter->setBrush(gradient);
        painter->drawRect(option->rect.adjusted(-1, 0, 1, 0));
        painter->setPen(Qt::white);
        painter->drawText(10, 10, name);
        painter->restore();

    } else {

        QLinearGradient gradient;
        gradient.setStart(0, 0);
        gradient.setFinalStop(0, 30);
        gradient.setColorAt(0, QColor("#3b3b3c"));
        gradient.setColorAt(1, QColor("#2d2d2f"));

        painter->save();
        painter->setPen(Qt::black);
        painter->setBrush(gradient);
        painter->drawRect(option->rect.adjusted(-1, 0, 1, 0));
        painter->setPen(Qt::white);
        painter->drawText(10, 20, name);
        painter->restore();

    }
}

// -------

class medDatabaseNavigatorItemGroupPrivate
{
public:
    int item_count;

    medDatabaseNavigatorItemGroupHeader *header;
};

medDatabaseNavigatorItemGroup::medDatabaseNavigatorItemGroup(QGraphicsItem *parent) : QGraphicsItem(parent), d(new medDatabaseNavigatorItemGroupPrivate)
{
    d->item_count = 0;

    d->header = new medDatabaseNavigatorItemGroupHeader(this);
}

medDatabaseNavigatorItemGroup::~medDatabaseNavigatorItemGroup(void)
{
    delete d;

    d = NULL;
}

void medDatabaseNavigatorItemGroup::addItem(medDatabaseNavigatorItem *item)
{
    qreal item_width   = medDatabaseNavigatorController::instance()->itemWidth();
    qreal item_spacing = medDatabaseNavigatorController::instance()->itemSpacing();

    item->setParentItem(this);

    medDatabaseNavigatorController::instance()->orientation() == Qt::Horizontal
        ? item->setPos(d->item_count * (item_width + item_spacing) + 40, 10)  // 40px for header
        : item->setPos(10, d->item_count * (item_width + item_spacing) + 40); // 40px for header

    d->item_count++;
}

void medDatabaseNavigatorItemGroup::clear(void)
{
    foreach(QGraphicsItem *item, this->childItems())
        delete item;

    d->item_count = 0;
}

void medDatabaseNavigatorItemGroup::setName(const QString& name)
{
    d->header->setName(name);
}

QRectF medDatabaseNavigatorItemGroup::boundingRect(void) const
{
    qreal item_width   = medDatabaseNavigatorController::instance()->itemWidth();
    qreal item_height  = medDatabaseNavigatorController::instance()->itemHeight();
    qreal item_spacing = medDatabaseNavigatorController::instance()->itemSpacing();

    if(medDatabaseNavigatorController::instance()->orientation() == Qt::Horizontal)
        return QRectF(0, 0, d->item_count * (item_width + item_spacing), item_height);
    else
        return QRectF(0, 0, item_width, d->item_count * (item_height + item_spacing));
}

void medDatabaseNavigatorItemGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}
