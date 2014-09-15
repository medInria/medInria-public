/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataIndex.h>
#include <medDatabaseItem.h>

class medDatabaseItemPrivate
{
public:
          medDatabaseItem * parentItem;
    QList<medDatabaseItem *> childItems;

    QList<QVariant> attrData;
    QList<QVariant> itemData;
    medDataIndex index;
};

medDatabaseItem::medDatabaseItem(medDataIndex index, const QList<QVariant>& attributes, const QList<QVariant>& data, medAbstractDatabaseItem *parent) : d(new medDatabaseItemPrivate)
{
    d->index = index;
    d->attrData = attributes;
    d->itemData = data;
    d->parentItem = dynamic_cast<medDatabaseItem*>(parent);
}

medDatabaseItem::~medDatabaseItem(void)
{
    qDeleteAll(d->childItems);
    d->childItems.clear();
    delete d;
    d = NULL;
}

medAbstractDatabaseItem *medDatabaseItem::child(int row)
{
    return d->childItems.value(row);
}

medAbstractDatabaseItem *medDatabaseItem::parent(void)
{
    return d->parentItem;
}

void medDatabaseItem::setParent(medAbstractDatabaseItem *parent)
{
    d->parentItem = static_cast<medDatabaseItem*>(parent);
}

void medDatabaseItem::append(medAbstractDatabaseItem *item)
{
    d->childItems.append(static_cast<medDatabaseItem*>(item));
}

int medDatabaseItem::row(void) const
{
    if (d->parentItem)
        return d->parentItem->d->childItems.indexOf(const_cast<medDatabaseItem*>(this));

    return 0;
}

int medDatabaseItem::childCount(void) const
{
    return d->childItems.count();
}

int medDatabaseItem::childNumber(void) const
{
    if (d->parentItem)
        return d->parentItem->d->childItems.indexOf(const_cast<medDatabaseItem *>(this));

    return 0;
}

int medDatabaseItem::columnCount(void) const
{
    return d->itemData.count();
}

QVariant medDatabaseItem::data(int column) const
{
    return d->itemData.value(column);
}

bool medDatabaseItem::insertChildren(const medDataIndex& index, int position, int count, int columns)
{
    Q_UNUSED(index);
    Q_UNUSED(columns);

    if (position < 0 || position > d->childItems.size())
        return false;

    for (int row = 0 ; row < count ; ++row) {
        QList<QVariant> attr;
        QList<QVariant> data;
        medDatabaseItem * item = new medDatabaseItem(medDataIndex(), attr, data, this);
        d->childItems.insert(position, item);
    }

    return true;
}

bool medDatabaseItem::insertColumns(int position, int columns)
{
    if (position < 0 || position > d->itemData.size())
        return false;

    for (int column = 0 ; column < columns ; ++column)
        d->itemData.insert(position, QVariant());

    foreach (medDatabaseItem *child, d->childItems)
        child->insertColumns(position, columns);

    return true;
}

bool medDatabaseItem::removeChildren(int position, int count,  bool deleteChildren)
{
    if (position < 0 || position + count > d->childItems.size())
        return false;

    for (int row = 0 ; row < count ; ++row)
    {
        if (deleteChildren)
        {
          delete d->childItems.takeAt(position);
        }
        else
        {
            d->childItems.removeAt(position);
        }
    }

    return true;
}

bool medDatabaseItem::removeColumns(int position, int columns)
{
    if (position < 0 || position + columns > d->itemData.size())
        return false;

    for (int column = 0 ; column < columns ; ++column)
        d->itemData.removeAt(position);

    foreach (medDatabaseItem *child, d->childItems)
        child->removeColumns(position, columns);

    return true;
}

bool medDatabaseItem::setData(int column, const QVariant& value)
{
    if (column < 0 || column >= d->itemData.size())
        return false;

    d->itemData[column] = value;

    return true;
}

QVariant medDatabaseItem::attribute(int column)
{
    if(column > d->attrData.size())
        return QVariant();

    return d->attrData.at(column);
}

QVariant medDatabaseItem::value(int column)
{
    if(column > d->itemData.size())
        return QVariant();

    return d->itemData.at(column);
}

QList<QVariant> medDatabaseItem::attributes()
{
    return d->attrData;       
}

QList<QVariant> medDatabaseItem::values()
{
    return d->itemData;  
}

/**
 * Returns the data index for this item
 */
const medDataIndex & medDatabaseItem::dataIndex() const
{
    return d->index;
}

void medDatabaseItem::setDataIndex (const medDataIndex &index)
{
    d->index = index;
}

int medDatabaseItem::rowOf(medAbstractDatabaseItem *child) const
{
    int row = 0;
    bool found = false;
    
    foreach(medDatabaseItem *item, d->childItems)
    {
        if(item==child)
        {
            found = true;
            break;
        }
        else row++;
    }
    
    if(!found)
        row = -1;
    
    return row;
    
}
