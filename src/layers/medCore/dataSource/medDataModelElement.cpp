/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataModelElement.h>

#include <medDataModel.h>
#include <medDataModelItem.h>

#include <QVariantList>

struct medDataModelElementPrivate
{
    medDataModel *parent;
    QString sourceInstanceId;
    bool bOnline;
    bool bWritable;
    bool bCache;
    bool bLocal;
    QMap<unsigned int, QStringList> columnNameByLevel;

    medDataModelItem *root;
};

medDataModelElement::medDataModelElement(medDataModel *parent, QString const & sourceIntanceId) : d(new medDataModelElementPrivate)
{
    d->parent = parent;
    d->sourceInstanceId = sourceIntanceId;
    d->root = new medDataModelItem();

    bool bOk = parent->getSourceGlobalInfo(sourceIntanceId, d->bOnline, d->bLocal, d->bWritable, d->bCache);

    if (bOk)
    {
        if (d->bOnline)
        {
            populateLevel(d->root, "");
        }
        else
        {
            getColumnNames(0);
        }
        if (d->bCache)
        {
            d->columnNameByLevel[0].push_back("Cached");
        }
    }
}

medDataModelElement::~medDataModelElement()
{
    delete d->root;
    delete d;
}

QVariant medDataModelElement::data(const QModelIndex & index, int role) const
{
    QVariant varDataRes;

    if (index.isValid())
    {
        if (role == Qt::TextAlignmentRole && index.column() > 0)
        {
            varDataRes = Qt::AlignHCenter;
        }
        else if (role == Qt::DisplayRole || role == Qt::EditRole)
        {
            medDataModelItem *item = static_cast<medDataModelItem *>(index.internalPointer());
            varDataRes = item->data(index.column());
        }
    }

    return varDataRes;
}

QModelIndex medDataModelElement::index(int row, int column, const QModelIndex & parent) const
{
    QModelIndex res;
    
    medDataModelItem *parentItem = nullptr;

    if (hasIndex(row, column, parent))
    {
        if (!parent.isValid())
        {
            parentItem = d->root;
        }
        else
        {
            parentItem = static_cast<medDataModelItem *>(parent.internalPointer());
        }

        medDataModelItem *childItem = parentItem->child(row);
        if (childItem)
        {
            res = createIndex(row, column, childItem);
        }
    }

    return res;
}

/**
* @brief  Get index of the parent.
* @param  index of which we want the parent.
* @return Returns the parent of the model item with the given index. If the item has no parent, an invalid QModelIndex is returned.
*/
QModelIndex medDataModelElement::parent(const QModelIndex & index) const
{
    QModelIndex indexRes;

    if (index.isValid())
    {
        auto indexItem = static_cast<medDataModelItem *>(index.internalPointer());
        auto parentItem = indexItem->parent();
        if (parentItem)
        {
            indexRes = createIndex(parentItem->row(), 0, parentItem);
        }
    }

    return indexRes;
}

/**
* @brief  Returns the number of columns for the children of the given parent.
* @param  parent is the index of the parent or invalid index for the top level.
* @return Returns the number of columns.
*/
int medDataModelElement::columnCount(const QModelIndex & parent) const
{
    int iRes = 0;
    
    unsigned int level = 0;

    if (parent.isValid())
    {
        medDataModelItem *parentItem = nullptr;
        parentItem = static_cast<medDataModelItem *>(parent.internalPointer());
        level = parentItem->level() + 1;
    }

    iRes = getLevelColumCount(level + 1);
    
    return iRes;
}

/**
* @brief  Returns the number of rows under the given parent.
* @param  parent is the index of the parent or invalid index for the top level.
* @return Returns the number of rows under the given parent. When the parent is valid it means that rowCount is returning the number of children of parent.
*/
int	medDataModelElement::rowCount(const QModelIndex &parent) const
{
    int iRes = 0;    

    if (parent.column() == 0)
    {
        if (!parent.isValid())
        {
            iRes = d->root->childCount();
        }
        else
        {
            medDataModelItem *parentItem = nullptr;
            parentItem = static_cast<medDataModelItem *>(parent.internalPointer());
            iRes = parentItem->childCount();
        }
    }    

    return iRes;
}






/* ***********************************************************************/
/* *************** Not overrides functions and slots *********************/
/* ***********************************************************************/

void medDataModelElement::setColumnAttributes(unsigned int p_uiLevel, QStringList & attributes)
{
    if (d->columnNameByLevel.contains(p_uiLevel))
    {
        d->columnNameByLevel[p_uiLevel].append(attributes);
    }
}

/**
* @brief  This slot refresh the current item pressed by GUI click, if the item don't have sons.
* @param  index of the GUI element clicked.
*/
void medDataModelElement::itemPressed(QModelIndex const &index)
{
    medDataModelItem * pItemCurrent = static_cast<medDataModelItem*>(index.internalPointer());

    if (index.isValid())
    {
        if (pItemCurrent)
        {
            if (pItemCurrent->childCount() == 0)
            {
                auto key = pItemCurrent->data(0).toString();
                populateLevel(pItemCurrent, key);
            }
        }
    }
}


/* ***********************************************************************/
/* *************** Private functions and slots ***************************/
/* ***********************************************************************/
bool medDataModelElement::getColumnNames(unsigned int const &iLevel) const
{
    bool bRes = true;
    
    QStringList attributes;
    bRes = d->parent->getLevelAttributes(d->sourceInstanceId, iLevel, attributes);
    
    if (bRes)
    {
        d->columnNameByLevel[iLevel] = attributes;
    }

    return bRes;
}

int medDataModelElement::getLevelColumCount(unsigned int pi_uiLevel) const
{
    int iRes = -1;
    bool bOk = true;

    if ( !d->columnNameByLevel.contains(pi_uiLevel))
    {
        bOk = getColumnNames(pi_uiLevel);
    }

    if (bOk)
    {    
        iRes = d->columnNameByLevel[pi_uiLevel].size();
    }

    return iRes;
}

void medDataModelElement::populateLevel(medDataModelItem * pi_pItem, QString const &key)
{
    QVariantList entries;
    unsigned int iLevel = pi_pItem->level();
    d->parent->getLevelMetaData(d->sourceInstanceId, iLevel+1, key, entries);

    // ////////////////////////////////////////////////////////////////////////
    // Populate column names if not already done
    if (!d->columnNameByLevel.contains(iLevel))
    {
        getColumnNames(iLevel);
    }
    
    // ////////////////////////////////////////////////////////////////////////
    // Populate data loop
    for (QVariant &var : entries)
    {
        medDataModelItem *pItemTmp = new medDataModelItem();
        auto elem = var.toStringList();
        pItemTmp->setData(elem);
        pItemTmp->setParent(pi_pItem);
        pi_pItem->append(pItemTmp);
    }
}

