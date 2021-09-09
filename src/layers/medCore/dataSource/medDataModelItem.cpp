#include <medDataModelItem.h>

#include <QVariant>
#include <QList>
#include <QVector>
#include <QString>

//#include <medDataModelElement.h>

class medDataModelItemPrivate
{
public:
    medDataModelItem * parentItem;
    QList<medDataModelItem *> childItems;

    QVariantList itemData;

    unsigned int uiLevel;
};



medDataModelItem::medDataModelItem() : d(new medDataModelItemPrivate())
{
    d->parentItem = nullptr;
    d->uiLevel = 0;
}

medDataModelItem::~medDataModelItem()
{
    delete d;
}




int medDataModelItem::childCount() const
{
    return d->childItems.size();
}

unsigned int medDataModelItem::level() const
{
    return d->uiLevel;
}

int medDataModelItem::row() const
{
    int iRowRes = 0;

    if (d->parentItem)
    {
        iRowRes = d->parentItem->d->childItems.indexOf(const_cast<medDataModelItem*>(this));
    }

    return iRowRes;
}

medDataModelItem * medDataModelItem::parent() const
{
    return d->parentItem;
}






medDataModelItem * medDataModelItem::child(int row) const
{
    return d->childItems.value(row);
}

QVariant medDataModelItem::data(int column) const
{
    return d->itemData.value(column);
}

void medDataModelItem::setParent(medDataModelItem * parent)
{
    d->parentItem = parent;
    d->uiLevel = d->parentItem->d->uiLevel + 1;
}

void medDataModelItem::setData(QStringList const & value)
{
    if (!d->itemData.isEmpty())
    {
        d->itemData.clear();
    }
    for (auto val : value)
    {
        d->itemData.append(val);
    }    
}

void medDataModelItem::append(medDataModelItem * child)
{
    d->childItems.append(child);
}
