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
    int item_count;

    QHash<int, medDatabasePreviewItem *> items;
};

medDatabasePreviewItemGroup::medDatabasePreviewItemGroup(QGraphicsItem *parent) : QGraphicsItem(parent), d(new medDatabasePreviewItemGroupPrivate)
{
    d->item_count = 0;
}

medDatabasePreviewItemGroup::~medDatabasePreviewItemGroup(void)
{
    delete d;

    d = NULL;
}

void medDatabasePreviewItemGroup::addItem(medDatabasePreviewItem *item)
{
    qreal item_width   = medDatabasePreviewController::instance()->itemWidth();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();

    item->setParentItem(this);

    medDatabasePreviewController::instance()->orientation() == Qt::Horizontal
        ? item->setPos(d->item_count * (item_width + item_spacing), 0)
        : item->setPos(0, d->item_count * (item_width + item_spacing));

    if (item->dataIndex().imageId() >= 0)
        d->items.insert(item->dataIndex().imageId(), item);
    else if (item->dataIndex().seriesId() >= 0)
        d->items.insert(item->dataIndex().seriesId(), item);
    else if (item->dataIndex().studyId() >= 0)
        d->items.insert(item->dataIndex().studyId(), item);
    else
        d->items.insert(item->dataIndex().patientId(), item);

    d->item_count++;
}

void medDatabasePreviewItemGroup::clear(void)
{
    foreach(QGraphicsItem *item, this->childItems())
        delete item;

    d->item_count = 0;
    d->items.clear();
}

medDatabasePreviewItem *medDatabasePreviewItemGroup::item(int index)
{
    if(d->items.keys().contains(index))
            return d->items.value(index);

    return NULL;
}

QRectF medDatabasePreviewItemGroup::boundingRect(void) const
{
    qreal item_width   = medDatabasePreviewController::instance()->itemWidth();
    qreal item_height  = medDatabasePreviewController::instance()->itemHeight();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();

    return QRectF(0, 0, d->item_count * (item_width + item_spacing), item_height);
}

void medDatabasePreviewItemGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}
