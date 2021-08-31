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

    QList<QVariant> itemData;

    unsigned int uiLevel;
};



medDataModelItem::medDataModelItem() : d(new medDataModelItemPrivate())
{
}

medDataModelItem::~medDataModelItem()
{
    delete d;
}




int medDataModelItem::childCount()
{
    return d->childItems.size();
}

unsigned int medDataModelItem::childLevel()
{
    return d->uiLevel+1;
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

medDataModelItem * medDataModelItem::parent()
{
    return d->parentItem;
}






medDataModelItem * medDataModelItem::child(int row)
{
    return d->childItems.value(row);
}

QVariant medDataModelItem::data(int column) const
{
    return d->itemData.value(column);
}
