/* medDatabasePreviewItemGroup.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:43:19 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:43:19 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabasePreviewController.h"
#include "medDatabasePreviewItem.h"
#include "medDatabasePreviewItemGroup.h"

#include <medCore/medDataIndex.h>

#include <QtCore>

class medDatabasePreviewItemGroupPrivate
{
public:
    int itemCount;

    QHash<int, medDatabasePreviewItem *> items;
};

medDatabasePreviewItemGroup::medDatabasePreviewItemGroup(QGraphicsItem *parent) : QGraphicsItem(parent), d(new medDatabasePreviewItemGroupPrivate)
{
    d->itemCount = 0;
}

medDatabasePreviewItemGroup::~medDatabasePreviewItemGroup()
{
    delete d;

    d = NULL;
}

void medDatabasePreviewItemGroup::addItem(medDatabasePreviewItem *item)
{
    qreal itemWidth   = medDatabasePreviewController::instance()->itemWidth();
    qreal itemSpacing = medDatabasePreviewController::instance()->itemSpacing();

    item->setParentItem(this);

    medDatabasePreviewController::instance()->orientation() == Qt::Horizontal
        ? item->setPos(d->itemCount * (itemWidth + itemSpacing), 0)
        : item->setPos(0, d->itemCount * (itemWidth + itemSpacing));

    if (item->dataIndex().imageId() >= 0)
        d->items.insert(item->dataIndex().imageId(), item);
    else if (item->dataIndex().seriesId() >= 0)
        d->items.insert(item->dataIndex().seriesId(), item);
    else if (item->dataIndex().studyId() >= 0)
        d->items.insert(item->dataIndex().studyId(), item);
    else
        d->items.insert(item->dataIndex().patientId(), item);

    d->itemCount++;
}

void medDatabasePreviewItemGroup::clear()
{
    foreach(QGraphicsItem *item, this->childItems())
        delete item;

    d->itemCount = 0;
    d->items.clear();
}

medDatabasePreviewItem *medDatabasePreviewItemGroup::item(int index)
{
    if(d->items.keys().contains(index))
            return d->items.value(index);

    return NULL;
}

QRectF medDatabasePreviewItemGroup::boundingRect() const
{
    qreal itemWidth   = medDatabasePreviewController::instance()->itemWidth();
    qreal itemHeight  = medDatabasePreviewController::instance()->itemHeight();
    qreal itemSpacing = medDatabasePreviewController::instance()->itemSpacing();

    return QRectF(0, 0, d->itemCount * (itemWidth + itemSpacing), itemHeight);
}

void medDatabasePreviewItemGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}
