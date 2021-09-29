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
#include <QSequentialIterable>

struct medDataModelElementPrivate
{
    medDataModel *parent;
    QString sourceInstanceId;
    bool bOnline;
    bool bWritable;
    bool bCache;
    bool bLocal;
    QMap<int, QStringList> columnNameByLevel;
    QStringList sectionNames;

    medDataModelItem *root;
};

medDataModelElement::medDataModelElement(medDataModel *parent, QString const & sourceIntanceId) : d(new medDataModelElementPrivate)
{
    d->parent = parent;
    d->sourceInstanceId = sourceIntanceId;
    d->root = new medDataModelItem(this);

    bool bOk = parent->getSourceGlobalInfo(sourceIntanceId, d->bOnline, d->bLocal, d->bWritable, d->bCache);

    if (bOk)
    {
        if (d->bOnline)
        {
            populateLevel(QModelIndex(), "");
        }
        else
        {
            fetchColumnNames(QModelIndex());
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
    Q_UNUSED(parent);
    return d->sectionNames.size();
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
    //TODO Correct implementation
    endInsertRows();
    return true;
}

bool medDataModelElement::removeRows(int row, int count, const QModelIndex & parent)
{
    bool bRes = true;
    
    medDataModelItem *pItem = getItem(parent);
    beginRemoveRows(parent, row, row + count - 1);
    bRes = pItem->removeRows(row, count);
    endRemoveRows();

    return bRes;
}






/* ***********************************************************************/
/* *************** Not overrides functions and slots *********************/
/* ***********************************************************************/

bool medDataModelElement::canFetchMore(const QModelIndex & parent) const
{
    bool bRes = false;

    if (parent.isValid())
    {
        //TODO Correct implementation
        bRes = true;// item->hasChildren && !item->children.count();
    }

    return bRes;
}

void medDataModelElement::fetchMore(const QModelIndex & parent)
{
    if (canFetchMore(parent))
    {
        //TODO Correct implementation

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

QModelIndex medDataModelElement::getIndex(QString iid, QModelIndex const &parent) const
{
    QModelIndex indexRes;

    auto item = getItem(parent);
    int row = item->childIndex(iid);
    if (row >= 0)
    {
        indexRes = parent.child(row, 0);
    }

    return indexRes;
}

QVariant medDataModelElement::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant varRes;

    if (role == Qt::DisplayRole && !d->sectionNames.isEmpty())
    {
        varRes = d->sectionNames[section];
    }

    return varRes;
}

//void medDataModelElement::setColumnAttributes(int p_iLevel, QStringList & attributes)
//{
//    if (d->columnNameByLevel.contains(p_iLevel))
//    {
//        d->columnNameByLevel[p_iLevel].append(attributes);
//        for (auto attribute : attributes)
//        {
//            if (!d->sectionNames.contains(attribute))
//            {
//                d->sectionNames.push_back(attribute);
//            }
//        }
//    }
//}

int medDataModelElement::getColumnInsideLevel(int level, int section)
{
    int iRes = -1;

    if (section > -1 && section < d->sectionNames.size())
    {
        if (d->columnNameByLevel.contains(level))
        {
            iRes = d->columnNameByLevel[level].indexOf(d->sectionNames[section]);
        }
    }

    return iRes;
}

bool medDataModelElement::fetch(QString uri) //See populateLevelV2
{
    QStringList splittedUri;
    splittedUri.append(uri.split('/'));
    QModelIndex childIndex;
    int i = 0;
    for (auto str : splittedUri)
    {
        auto childIndexTmp = getIndex(splittedUri[i]);
        if (!childIndexTmp.isValid())
        {
            //TODO le level intermediaire n'existe pas encore
            //TODO TRY populate
            //TODO essayer à nouveau apres le populate si le child existe
            //     Si oui on continue
            //     Si non on part en erreur
        }
        childIndex = childIndexTmp;
        i++;
    }

    return false;
}

/**
* @brief  This slot refresh the current item pressed by GUI click, if the item don't have sons.
* @param  index of the GUI element clicked.
*/
void medDataModelElement::itemPressed(QModelIndex const &index)
{
    if (index.isValid())
    {
        medDataModelItem * pItemCurrent = getItem(index);
        if (currentLevelFetchable(pItemCurrent) && pItemCurrent->childCount() == 0 )
        {
            auto key = pItemCurrent->iid();
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
bool medDataModelElement::fetchColumnNames(const QModelIndex &index/*int const &m_iLevel*/)
{
    bool bRes = true;
    
    QStringList attributes;
    auto item = getItem(index);
    bRes = d->parent->getLevelAttributes(d->sourceInstanceId, item->level()+1, attributes);
    attributes.pop_front();
    
    if (bRes)
    {
        d->columnNameByLevel[item->level()+1] = attributes;
        for (auto attribute : attributes)
        {
            if (!d->sectionNames.contains(attribute))
            {
                d->sectionNames.push_back(attribute);
            }
        }
    }

    return bRes;
}


void medDataModelElement::populateLevel(QModelIndex const &index, QString const &key)
{
    QVariantList entries;
    medDataModelItem *pItem = getItem(index);

    int iLevel = pItem->level()+1;

    if (d->parent->getLevelMetaData(d->sourceInstanceId, iLevel, key, entries))
    {
        if (entries.size() > 0)
        {
            if (index.isValid())
            {
                int row = rowCount(index);
                int elemCount = entries.size();
                emit layoutAboutToBeChanged(); //this is useful to update arrow on the left if click is not inside
                beginInsertRows(index.siblingAtColumn(0), row, row + elemCount - 1);
            }

            // ////////////////////////////////////////////////////////////////////////
            // Populate column names if not already done
            if (!d->columnNameByLevel.contains(iLevel))
            {
                fetchColumnNames(index/*iLevel*/);
            }

            // ////////////////////////////////////////////////////////////////////////
            // Populate data loop
            for (QVariant &var : entries)
            {
                medDataModelItem *pItemTmp = new medDataModelItem(this);
                auto elem = var.toStringList();
                //pItemTmp->setIID(elem.takeFirst());
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

void medDataModelElement::populateLevelV2(QModelIndex const & index, QString const & uri)
{
    QVariantList entries;
    medDataModelItem *pItem = getItem(index);
    int iLevel = pItem->level() + 1;

    if (d->parent->getLevelMetaData(d->sourceInstanceId, iLevel, uri, entries))
    {
        emit layoutAboutToBeChanged(); //this is useful to update arrow on the left if click is not inside

        QVector<QPair<int, int> > rangeToRemove; // vector of ranges to delete, <beginRange, endRange>
        computeRowRangesToRemove(pItem, entries, rangeToRemove);
        removeRowRanges(rangeToRemove, index);

        //TODO Update data already present inside the model

        QMap<int, QVariantList> entriesToAdd; //position to insert, List of QVariant, itself QVariantList representation of minimal entries
        computeRowRangesToAdd(pItem, entries, entriesToAdd);
        addRowRanges(entriesToAdd, index);

        emit layoutChanged(); // close the emit layoutAboutToBeChanged();
    }
}


bool medDataModelElement::itemStillExist(QVariantList &entries, medDataModelItem * pItem)
{
    bool bFind = false;

    auto it = entries.begin();
    auto end = entries.end();
    while ((it != end) && !bFind)
    {
        bFind = pItem->iid() == (*it).toStringList()[0];
        ++it;
    }

    return bFind;
}

void medDataModelElement::computeRowRangesToRemove(medDataModelItem * pItem, QVariantList &entries, QVector<QPair<int, int>> &rangeToRemove)
{
    int iStartRemoveRange = -1;
    for (int i = 0; i < pItem->childCount(); ++i)
    {
        if (!itemStillExist(entries, pItem->child(i)))
        {
            //Here pItem->child(i) is no longer present inside refreshed entries
            if (iStartRemoveRange == -1)
            {
                //Here a new range starting
                iStartRemoveRange = i;
            }
        }
        else
        {
            //Here pItem->child(i) is still present inside refreshed entries
            if (iStartRemoveRange > -1)
            {
                //Here the current range end
                rangeToRemove.push_back({ iStartRemoveRange, i - 1 });
                iStartRemoveRange = -1; //clean iStartRemoveRange for a future range
            }
        }
    }

    if (iStartRemoveRange > -1)
    {
        //Here we finish with an opened range. Then we close the range with the last child
        rangeToRemove.push_back({ iStartRemoveRange, pItem->childCount() - 1 });
    }
}

void medDataModelElement::removeRowRanges(QVector<QPair<int, int>> &rangeToRemove, const QModelIndex & index)
{
    int iOffsetRange = 0; //Accumulate deletions count to correct ranges still to delete
    for (auto &range : rangeToRemove)
    {
        removeRows(range.first - iOffsetRange, range.second - iOffsetRange, index); //Used Override removeRows of QAbstractItemModel
        iOffsetRange += range.second - range.first; //Update the offset
    }
}



void medDataModelElement::computeRowRangesToAdd(medDataModelItem * pItem, QVariantList &entries, QMap<int, QVariantList> &entriesToAdd)
{
    int  iLastItemAlreadyPresent = -1;
    for (QVariant &var : entries)
    {
        int iTmpLastItemAlreadyPresent = pItem->childIndex(var.toStringList()[0]);

        if (iTmpLastItemAlreadyPresent == -1)
        {
            entriesToAdd[iLastItemAlreadyPresent + 1].append(var);
        }
        else
        {
            iLastItemAlreadyPresent = iTmpLastItemAlreadyPresent;
        }
    }
}

void medDataModelElement::addRowRanges(QMap<int, QVariantList> &entriesToAdd, const QModelIndex & index)
{
    int iOffsetRange = 0;
    auto startRangeList = entriesToAdd.keys();
    medDataModelItem *pItem = getItem(index);
    for (int i = 0; i < startRangeList.size(); ++i)
    {
        int first = startRangeList[i] ;
        int last = startRangeList[i] + entriesToAdd[startRangeList[i]].size();
        beginInsertRows(index.siblingAtColumn(0), first + iOffsetRange, last + iOffsetRange - 1);

        // ////////////////////////////////////////////////////////////////////////
        // Populate data loop
        for (QVariant &var : entriesToAdd[startRangeList[i]])
        {
            medDataModelItem *pItemTmp = new medDataModelItem(this);
            auto elem = var.toStringList();
            pItemTmp->setData(elem);
            pItemTmp->setParent(pItem);
            pItem->append(pItemTmp);
        }
        iOffsetRange += first + last;

        endInsertRows();
    }
}


