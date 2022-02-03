#include <medDataModelItem.h>
#include <medSourceItemModel.h>

#include <QVariant>
#include <QList>
#include <QVector>
#include <QString>


medDataModelItem::medDataModelItem(medSourceItemModel *model)
{
    this->model = model;
    parentItem = nullptr; //Invalid parent 
    iLevel = -1; //Invalid effective level, used by root item
    bCanHaveSubData = true;
}

medDataModelItem::medDataModelItem(medDataModelItem *parent)
{
    model = parent->model;
    bCanHaveSubData = true;
    setParent(parent);
}

medDataModelItem::~medDataModelItem()
{
    for (auto &childItem : childItems)
    {
        delete childItem;
        childItem = nullptr; //useful ?
    }
    childItems.clear(); //useful ?
}



int medDataModelItem::row() const
{
    int iRowRes = 0;

    if (parentItem)
    {
        iRowRes = parentItem->childItems.indexOf(const_cast<medDataModelItem*>(this)); // ??
    }

    return iRowRes;
}

bool medDataModelItem::removeRows(int row, int count)
{
    bool bRes = true;

    if (row>-1 && count>0 && (row + count - 1 < childItems.size()))
    {
        for (int i = row; i < row + count; ++i)
        {
            auto   childItemToRemove = childItems[i];
            delete childItemToRemove;
        }
        childItems.erase(childItems.begin() + row, childItems.begin() + row + count);
    }

    return bRes;
}

QString medDataModelItem::uri()
{
    QString uriRes = model->getSourceIntanceId() + ":";

    QStringList uriParts;

    medDataModelItem *pItem = this;
    while (pItem->parentItem)
    {
        uriParts.push_front(pItem->iid());
        pItem = pItem->parentItem;
    }
    int size = uriParts.size();
    if (size > 0)
    {
        int i;
        for (i = 0; (i+1)<size; ++i)
        {
            uriRes += uriParts[i] + "\r\n";
        }
        uriRes += uriParts[i];
    }

    return uriRes;
}

QModelIndex medDataModelItem::index() //TODO finish or remove
{
    QModelIndex indexRes;

    if (parentItem != nullptr)
    {
        //indexRes = model->index();//TODO19
    }
    else
    {
        indexRes = parentItem->index().child(parentItem->childItems.indexOf(this), 0);
    }
    return indexRes;
}


/* ***********************************************************************/
/* *************** Data manipulation *************************************/
/* ***********************************************************************/
int medDataModelItem::childIndex(QString iid)
{
    int iRes = -1;
    
    int i = 0;
    bool bFind = false;
    while (!bFind && i < childItems.size())
    {
        bFind = childItems[i]->itemData[0][0] == iid;
        if (bFind)
        {
            iRes = i;
        }
        ++i;
    }

    return iRes;
}

medDataModelItem * medDataModelItem::hasChildItemWithIID(QString iid)
{
    medDataModelItem* pItemRes = nullptr;

    int iPos = childIndex(iid);
    if (iPos > -1)
    {
        pItemRes = child(iPos);
    }

    return pItemRes;
}

void medDataModelItem::setParent(medDataModelItem * parent)
{
    parentItem = parent;
    iLevel = parentItem->iLevel + 1;
}

void medDataModelItem::setMetaData(QMap<QString, QVariant> const & attributes, QMap<QString, QString> const & tags)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    for (auto const & key : attributes.keys())
    {
        itemMeta[key] = attributes[key];
    }
    for (auto const & key : tags.keys())
    {
        itemMetaTag[key] = tags[key];
    }
#else
        itemMeta.insert(attributes);
        itemMetaTag.insert(tags);
#endif
}

QVariant medDataModelItem::data(int column, int role) const
{
    QVariant varRes;

    if (itemData.contains(column) && itemData[column].contains(role))
    {
        varRes = itemData[column][role];
    }

    return varRes;
}
