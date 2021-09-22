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
    QMap<int, QStringList> columnNameByLevel;

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
            populateLevel(QModelIndex(), "");
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
        else if ((role == Qt::DisplayRole || role == Qt::EditRole))
        {
            medDataModelItem *item = getItem(index);
            int i = index.column();
            varDataRes = item->data(i).toString();
        }
    }

    return varDataRes;
}

QModelIndex medDataModelElement::index(int row, int column, const QModelIndex & parent) const
{
    QModelIndex res;

    medDataModelItem *childItem = getItem(parent)->child(row);
    if (childItem)
    {
        res = createIndex(row, column, childItem);
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
        auto parentItem = getItem(index)->parent();
        if (parentItem && parentItem != d->root)
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
    
    int level = 0;

    if (parent.isValid())
    {
        level = getItem(parent)->level() + 1;
    }

    iRes = getLevelColumCount(level);

    if (iRes < 0)
    {
        //qWarning << QString(__FILE__) + __LINE__; //TODO
        iRes = 0;
    }
    
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

    if (!parent.isValid() || parent.column() == 0)
    {
        iRes = getItem(parent)->childCount();
    }

    return iRes;
}

bool medDataModelElement::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);

    endInsertRows();
    return true;
}






/* ***********************************************************************/
/* *************** Not overrides functions and slots *********************/
/* ***********************************************************************/

inline bool medDataModelElement::canFetchMore(const QModelIndex & parent) const
{
    bool bRes = false;

    if (parent.isValid())
    {
        bRes = true;// item->hasChildren && !item->children.count();
    }

    return bRes;
}

inline void medDataModelElement::fetchMore(const QModelIndex & parent)
{
    if (canFetchMore(parent))
    {
        //medDataModelElementPrivate *node = getItem(parent);
        //
        // get the information you need to populate node
        //
        //beginInsertRows(node, 0, mm_IDs.count() - 1);
        //setLists(node, mm_IDs, mm_Names);
        //endInsertRows();
    }
}

inline medDataModelItem * medDataModelElement::getItem(const QModelIndex & index) const
{
    medDataModelItem* itemRes = d->root;

    if (index.isValid())
    {
        medDataModelItem *item = static_cast<medDataModelItem*>(index.internalPointer());
        if (item)
        {
            itemRes = item;
        }
    }

    return itemRes;
}

void medDataModelElement::setColumnAttributes(int p_iLevel, QStringList & attributes)
{
    if (d->columnNameByLevel.contains(p_iLevel))
    {
        d->columnNameByLevel[p_iLevel].append(attributes);
    }
}

/**
* @brief  This slot refresh the current item pressed by GUI click, if the item don't have sons.
* @param  index of the GUI element clicked.
*/
void medDataModelElement::itemPressed(QModelIndex const &index)
{
    if (index.isValid() /*&& index.column() == 0*/) //TODO find way to remove  '&& index.column() == 0'
    {
        medDataModelItem * pItemCurrent = getItem(index);
        if (currentLevelFetchable(pItemCurrent) && pItemCurrent->childCount() == 0 )
        {
            auto key = pItemCurrent->data(0).toString();
            populateLevel(index, key);
        }
    }
}

bool medDataModelElement::currentLevelFetchable(medDataModelItem * pItemCurrent)
{
    bool bRes = pItemCurrent->canHaveSubData();
    
    unsigned int uiLevelMax = 0;

    bRes &= d->parent->getLevelCount(d->sourceInstanceId, uiLevelMax);
    if (uiLevelMax != 0)
    {
        bRes &= static_cast<unsigned int>(pItemCurrent->level()) < uiLevelMax-1;
    }

    return bRes;
}


/* ***********************************************************************/
/* *************** Private functions and slots ***************************/
/* ***********************************************************************/
bool medDataModelElement::getColumnNames(int const &m_iLevel) const
{
    bool bRes = true;
    
    QStringList attributes;
    bRes = d->parent->getLevelAttributes(d->sourceInstanceId, m_iLevel, attributes);
    
    if (bRes)
    {
        d->columnNameByLevel[m_iLevel] = attributes;
    }

    return bRes;
}

int medDataModelElement::getLevelColumCount(int pi_iLevel) const
{
    int iRes = -1;
    bool bOk = true;

    if ( !d->columnNameByLevel.contains(pi_iLevel))
    {
        bOk = getColumnNames(pi_iLevel);
    }

    if (bOk)
    {    
        iRes = d->columnNameByLevel[pi_iLevel].size();
    }

    return iRes;
}

void medDataModelElement::populateLevel(QModelIndex const &index, QString const &key)
{
    QVariantList entries;
    medDataModelItem *pItem = getItem(index);

    int m_iLevel = pItem->level()+1;

    if (d->parent->getLevelMetaData(d->sourceInstanceId, m_iLevel, key, entries))
    {
        if (entries.size() > 0)
        {
            if (index.isValid())
            {
                int row = rowCount(index);
                int elemCount = entries.size();
                emit layoutAboutToBeChanged(); //this is useful to update arrow on the left if click is not inside 
                beginInsertRows(index, row, row + elemCount - 1);
            }
            else
            {
                int i = 0;
                ++i;
            }

            // ////////////////////////////////////////////////////////////////////////
            // Populate column names if not already done
            if (!d->columnNameByLevel.contains(m_iLevel))
            {
                getColumnNames(m_iLevel);
            }

            // ////////////////////////////////////////////////////////////////////////
            // Populate data loop
            for (QVariant &var : entries)
            {
                medDataModelItem *pItemTmp = new medDataModelItem();
                auto elem = var.toStringList();
                pItemTmp->setData(elem);
                pItemTmp->setParent(pItem);
                pItem->append(pItemTmp);
            }

            if (index.isValid())
            {
                endInsertRows();
                emit layoutChanged(); // close the emit layoutAboutToBeChanged();
            }
        }
    }
}

