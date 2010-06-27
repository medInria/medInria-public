/* medDatabaseNavigatorItemGroup.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:39:49 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Sun Jun 27 19:21:16 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 11
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
#include "medDatabaseNonPersitentController.h"

#include <QtCore>

class medDatabaseNavigatorItemGroupPrivate
{
public:
    bool non_persitent;

    int item_count;

    QString name;
};

medDatabaseNavigatorItemGroup::medDatabaseNavigatorItemGroup(QGraphicsItem *parent) : QGraphicsItem(parent), d(new medDatabaseNavigatorItemGroupPrivate)
{
    d->non_persitent = false;

    d->item_count = 0;
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

    // 30px for the decoration
    // 2x10px for the margins

    medDatabaseNavigatorController::instance()->orientation() == Qt::Horizontal
        ? item->setPos(d->item_count * (item_width + item_spacing) + 30, 10)
        : item->setPos(10, d->item_count * (item_width + item_spacing) + 30);

    d->item_count++;

    if(item->patientId() >= medDatabaseNonPersitentController::nonPersitentDataStartingIndex())
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

    if(medDatabaseNavigatorController::instance()->orientation() == Qt::Horizontal)
        return QRectF(0, 0, d->item_count * (item_width + item_spacing) + 30, item_height + 20);
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
    painter->drawText(option->rect.adjusted(5, 5, -15, option->rect.height() - 20), Qt::AlignRight | Qt::TextSingleLine, d->name);
    painter->restore();
}
