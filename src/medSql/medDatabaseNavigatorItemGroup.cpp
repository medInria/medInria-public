/* medDatabaseNavigatorItemGroup.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:39:49 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Oct 26 10:02:28 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 14
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabaseNavigatorController.h"
#include "medDatabaseNavigatorItem.h"
#include "medDatabaseNavigatorItemGroup.h"
#include "medDatabaseNonPersistentController.h"

#include <QtCore>

class medDatabaseNavigatorItemGroupPrivate
{
public:
    bool non_persitent;

    int item_count;

    QString name;

    Qt::Orientation orientation;

    QList<medDatabaseNavigatorItem*> items;
};

medDatabaseNavigatorItemGroup::medDatabaseNavigatorItemGroup(QGraphicsItem *parent) : QGraphicsItem(parent), d(new medDatabaseNavigatorItemGroupPrivate)
{
    d->non_persitent = false;

    d->item_count = 0;

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
        ? item->setPos(d->item_count * (item_height + item_spacing) + 10, 30)
        : item->setPos(10, d->item_count * (item_width + item_spacing) + 30);

    d->item_count++;

    if(item->patientId() >= medDatabaseNonPersistentController::instance()->nonPersistentDataStartingIndex() ||
       item->studyId()   >= medDatabaseNonPersistentController::instance()->nonPersistentDataStartingIndex() ||
       item->seriesId()  >= medDatabaseNonPersistentController::instance()->nonPersistentDataStartingIndex() )
        d->non_persitent = true;
}

void medDatabaseNavigatorItemGroup::clear(void)
{
    foreach(QGraphicsItem *item, this->childItems())
        delete item;

    d->item_count = 0;
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
        return QRectF(0, 0, d->item_count * (item_width + item_spacing) + 20, item_height + item_spacing + 30);
    else
        return QRectF(0, 0, item_width + 20, d->item_count * (item_height + item_spacing) + 30);
}

void medDatabaseNavigatorItemGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    if(!d->non_persitent)
        painter->setBrush(Qt::darkGray);
    else
        painter->setBrush(QColor(129, 143, 164));
    painter->setPen(Qt::NoPen);
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
    d->item_count = 0;

    foreach (medDatabaseNavigatorItem* item, items)
        addItem (item);
}

Qt::Orientation medDatabaseNavigatorItemGroup::orientation (void) const
{
    return d->orientation;
}
