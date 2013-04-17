/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseNavigatorItemGroup.h"

#include <QtCore>

#include <medCore/medDataManager.h>
#include <medCore/medAbstractDbController.h>

#include "medDatabaseNavigatorController.h"
#include "medDatabaseNavigatorItem.h"

class medDatabaseNavigatorItemGroupPrivate
{
public:
    int itemCount;

    QString name;

    Qt::Orientation orientation;

    QList<medDatabaseNavigatorItem*> items;
};

medDatabaseNavigatorItemGroup::medDatabaseNavigatorItemGroup(QGraphicsItem *parent) : QGraphicsItem(parent), d(new medDatabaseNavigatorItemGroupPrivate)
{
    d->itemCount = 0;

    d->orientation = medDatabaseNavigatorController::instance()->orientation();
}

medDatabaseNavigatorItemGroup::~medDatabaseNavigatorItemGroup(void)
{
    delete d;

    d = NULL;
}

void medDatabaseNavigatorItemGroup::addItem(medDatabaseNavigatorItem *item)
{
    qreal item_width   = medDatabaseNavigatorController::instance()->itemWidth();
    qreal item_height  = medDatabaseNavigatorController::instance()->itemHeight();
    qreal item_spacing = medDatabaseNavigatorController::instance()->itemSpacing();

    item->setParentItem(this);

    d->items << item;

    // 30px for the decoration
    // 2x10px for the margins

    d->orientation == Qt::Horizontal
        ? item->setPos(d->itemCount * (item_height + item_spacing) + 10, 30)
        : item->setPos(10, d->itemCount * (item_width + item_spacing) + 30);

    d->itemCount++;
}

void medDatabaseNavigatorItemGroup::clear(void)
{
    foreach(QGraphicsItem *item, this->childItems())
        delete item;

    d->itemCount = 0;
}

void medDatabaseNavigatorItemGroup::setName(const QString& name)
{
    d->name = name;
}

QRectF medDatabaseNavigatorItemGroup::boundingRect(void) const
{
    qreal item_width   = medDatabaseNavigatorController::instance()->itemWidth();
    qreal item_height  = medDatabaseNavigatorController::instance()->itemHeight();
    qreal item_spacing = medDatabaseNavigatorController::instance()->itemSpacing();

    // 20px for the margins
    // 30px for the decoration

    if(d->orientation == Qt::Horizontal)
        return QRectF(0, 0, d->itemCount * (item_width + item_spacing) + 20, item_height + item_spacing + 30);
    else
        return QRectF(0, 0, item_width + 20, d->itemCount * (item_height + item_spacing) + 30);
}

void medDatabaseNavigatorItemGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(Qt::darkGray);
    painter->drawRoundedRect(option->rect, 5, 5);
    painter->setPen(Qt::white);
    if (d->orientation==Qt::Horizontal)
        painter->drawText(option->rect.adjusted(5, 5, option->rect.height() -20, -15), Qt::AlignLeft | Qt::TextSingleLine, d->name);
    else
        painter->drawText(option->rect.adjusted(5, 5, -20, option->rect.height() -15), Qt::AlignRight | Qt::TextSingleLine, d->name);
    painter->restore();
}

void medDatabaseNavigatorItemGroup::setOrientation (Qt::Orientation orientation)
{
    if (d->orientation == orientation)
        return;

    d->orientation = orientation;

    QList<medDatabaseNavigatorItem*> items = d->items;
    d->items.clear();
    d->itemCount = 0;

    foreach (medDatabaseNavigatorItem* item, items)
        addItem (item);
}

Qt::Orientation medDatabaseNavigatorItemGroup::orientation() const
{
    return d->orientation;
}
