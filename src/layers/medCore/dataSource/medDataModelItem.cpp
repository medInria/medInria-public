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
    model->removeItem(this);
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

QList<medDataModelItem*> medDataModelItem::offspringList()
{
    QList<medDataModelItem*> offspringRes = childItems;
    
    for (int i = 0; i < offspringRes.size(); ++i)
    {
        offspringRes.append(offspringRes[i]->childItems);
    }

    return offspringRes;
}

QStringList medDataModelItem::relativeUri()
{
    QStringList uriListRes;

    medDataModelItem *pItem = this;
    while (pItem->parentItem)
    {
        uriListRes.push_front(pItem->iid());
        pItem = pItem->parentItem;
    }

    return uriListRes;
}

QStringList medDataModelItem::uri()
{
    QStringList uriRes = relativeUri();
    uriRes.push_front(model->getSourceIntanceId());
    return uriRes;
}

QString medDataModelItem::uriAsString()
{
    QString uriRes = model->getSourceIntanceId() + ":";

    QStringList uriParts = relativeUri();
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
    model->registerItem(this);
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

QString medDataModelItem::iid(QString displayValue)
{
    QString iidRes;

    int i = 0;
    while (iidRes.isEmpty() && i < childItems.size())
    {
        if (displayValue == childItems[i]->data(1).toString())
        {
            iidRes = childItems[i]->iid();
        }
        i++;
    }

    return iidRes;
}

//bool medDataModelItem::isAssociatedAbstractData()
//{
//    bool bRes = false;
//
//    bRes = itemData[0][100] == "DataCommited" || itemData[0][100] == "DataLoaded";
//
//    int i = 0;
//    int iChildCount = childItems.size();
//    while (!bRes && i < iChildCount)
//    {
//        bRes = childItems[i]->isAssociatedAbstractData();
//        i++;
//    }
//
//    return bRes;
//}

bool medDataModelItem::containRoleValues(QMap<int , QVariantList> mapList)
{
    bool bRes = false;

    int i = 0;
    while (!bRes && i<mapList.size())
    {
        int role = mapList.keys()[i];
        if (itemData[0].contains(role))
        {
            if (itemData[0][role].canConvert<bool>() && mapList[role].isEmpty())
            {
                bRes = itemData[0][role].toBool();
            }
            else
            {
                bRes = itemData[0][role] == mapList[role][i];
            }
        }
        ++i;
    }

    return bRes;
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