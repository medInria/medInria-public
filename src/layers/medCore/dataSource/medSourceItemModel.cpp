/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSourceItemModel.h>

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

medSourceItemModel::medSourceItemModel(medDataModel *parent, QString const & sourceIntanceId) : d(new medDataModelElementPrivate)
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

medSourceItemModel::~medSourceItemModel()
{
    delete d->root;
    delete d;
}

medDataModel * medSourceItemModel::model()
{
    return d->parent;
}

QVariant medSourceItemModel::data(const QModelIndex & index, int role) const
{
    QVariant varDataRes;

    if (index.isValid())
    {
        if (role == Qt::TextAlignmentRole && index.column() > 0)
        {
            varDataRes = Qt::AlignHCenter;
        }
        else //if ((role == Qt::DisplayRole || role == Qt::EditRole))
        {
            medDataModelItem *item = getItem(index);
            int i = index.column();
            varDataRes = item->data(i, role);
        }
    }

    return varDataRes;
}

QModelIndex medSourceItemModel::index(int row, int column, const QModelIndex & parent) const
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
QModelIndex medSourceItemModel::parent(const QModelIndex & index) const
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
int medSourceItemModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return d->sectionNames.size();
}

/**
* @brief  Returns the number of rows under the given parent.
* @param  parent is the index of the parent or invalid index for the top level.
* @return Returns the number of rows under the given parent. When the parent is valid it means that rowCount is returning the number of children of parent.
*/
int	medSourceItemModel::rowCount(const QModelIndex &parent) const
{
    int iRes = 0;    

    if (!parent.isValid() || parent.column() == 0)
    {
        iRes = getItem(parent)->childCount();
    }

    return iRes;
}

bool medSourceItemModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    //TODO Correct implementation
    endInsertRows();
    return true;
}

bool medSourceItemModel::removeRows(int row, int count, const QModelIndex & parent)
{
    bool bRes = true;
    
    medDataModelItem *pItem = getItem(parent.siblingAtColumn(0));

    beginRemoveRows(parent.siblingAtColumn(0), row, row + count - 1);
    QSignalBlocker blocker(this);
    bRes = pItem->removeRows(row, count);

    blocker.unblock();

    endRemoveRows();

    return bRes;
}






/* ***********************************************************************/
/* *************** Not overrides functions and slots *********************/
/* ***********************************************************************/

bool medSourceItemModel::canFetchMore(const QModelIndex & parent) const
{
    bool bRes = false;

    if (parent.isValid())
    {
        //TODO Correct implementation
        bRes = true;// item->hasChildren && !item->children.count();
    }

    return bRes;
}

void medSourceItemModel::fetchMore(const QModelIndex & parent)
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

inline medDataModelItem * medSourceItemModel::getItem(const QModelIndex & index) const
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

QModelIndex medSourceItemModel::getIndex(QString iid, QModelIndex const &parent) const
{
    QModelIndex indexRes;

    auto item = getItem(parent);
    int row = item->childIndex(iid);

    if (row >= 0)
    {
        indexRes = index(row, 0, parent);
    }

    return indexRes;
}

QVariant medSourceItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant varRes;

    if (role == Qt::DisplayRole && !d->sectionNames.isEmpty())
    {
        varRes = d->sectionNames[section];
    }

    return varRes;
}









Qt::DropActions medSourceItemModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

Qt::ItemFlags medSourceItemModel::flags(const QModelIndex & index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}

QStringList medSourceItemModel::mimeTypes() const
{
    QStringList types;
    types << "med/index2";
    return types;
}

QMimeData * medSourceItemModel::mimeData(const QModelIndexList & indexes) const
{
    QMimeData *mimeData = new QMimeData;
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    for (QModelIndex const &index : indexes)
    {
        if (index.isValid() && index.column() == 0)
        {
            encodedData.append(getItem(index)->uri().toUtf8());
            encodedData.append('\0');            
        }
    }

    if (!encodedData.isEmpty())
    {
        encodedData.remove(encodedData.length()-1, 1);
    }


    mimeData->setData("med/index2", encodedData);
    return mimeData;
}









int medSourceItemModel::getColumnInsideLevel(int level, int section)
{
    int iRes = -1;

    if (section > 0 && section < d->sectionNames.size())
    {
        if (d->columnNameByLevel.contains(level))
        {
            iRes = d->columnNameByLevel[level].indexOf(d->sectionNames[section]);
        }
    }
    else if (section == 0)
    {
        iRes = 1;
    }

    return iRes;
}

int medSourceItemModel::getSectionInsideLevel(int level, int column)
{
    int iRes = -1;

    if (d->columnNameByLevel.contains(level))
    {
        if (column > -1 && column < d->columnNameByLevel[level].size())
        {
            QString colName = d->columnNameByLevel[level][column];
            iRes = d->sectionNames.indexOf(colName);
        }
        //else if (column == 0)
        //{
        //    iRes = 0;
        //}
    }

    return iRes;
}

bool medSourceItemModel::fetch(QString uri) //See populateLevelV2
{
    QStringList splittedUri;
    splittedUri.append(uri.split('/'));
    QModelIndex childIndex;
    int i = 0;
    for (auto str : splittedUri)
    {
        auto childIndexTmp = getIndex(splittedUri[i], childIndex);
        if (!childIndexTmp.isValid())
        {
            //TODO le level intermediaire n'existe pas encore
            //TODO TRY populate
            //TODO essayer  nouveau apres le populate si le child existe
            //     Si oui on continue
            //     Si non on part en erreur
        }
        childIndex = childIndexTmp;
        i++;
    }
    populateLevelV2(childIndex, uri);

    return false;
}

QString medSourceItemModel::getSourceIntanceId()
{
    return d->sourceInstanceId;
}

void medSourceItemModel::setOnline(bool pi_bOnline)
{
    if (d->bOnline != pi_bOnline)
    {
        d->bOnline = pi_bOnline;
        emit online(d->bOnline);
    }
}

medSourceItemModel::datasetAttributes medSourceItemModel::getMetaData(QModelIndex const & index)
{
    datasetAttributes res;

    if (index.isValid())
    {
        medDataModelItem *pItem = getItem(index);
        int iLevel = pItem->level();
        for (int i = 0; i < d->columnNameByLevel[iLevel].size(); ++i)
        {
            res[d->columnNameByLevel[iLevel][i]] = pItem->metaData(i).toString();
        }
    }

    return res;
}

/**
* @brief  This slot refresh the current item pressed by GUI click, if the item don't have sons.
* @param  index of the GUI element clicked.
*/
void medSourceItemModel::itemPressed(QModelIndex const &index)
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

bool medSourceItemModel::currentLevelFetchable(medDataModelItem * pItemCurrent)
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
bool medSourceItemModel::fetchColumnNames(const QModelIndex &index/*int const &m_iLevel*/)
{
    bool bRes = true;
    
    QStringList attributes;
    auto item = getItem(index);
    bRes = d->parent->getMandatoryAttributesKeys(d->sourceInstanceId, item->level()+1, attributes);
    //attributes.pop_front(); //To remove the key of minimal entries structure
    
    if (bRes)
    {
        d->columnNameByLevel[item->level()+1] = attributes;
        attributes.pop_front(); //To remove the key of minimal entries structure
        
        QString firstSectionName = attributes.takeFirst();
        if(d->sectionNames.isEmpty())
        {
            d->sectionNames.push_back(firstSectionName);
        }
        else if ( !d->sectionNames[0].contains(firstSectionName, Qt::CaseInsensitive))
        {
            d->sectionNames[0] = d->sectionNames[0] + " | " + firstSectionName;
        }

        for (auto attribute : attributes)
        {
            if (!d->sectionNames.contains(attribute))
            {
                d->sectionNames.push_back(attribute);
                emit columnCountChange(d->sectionNames.size());
            }
        }
    }

    return bRes;
}


void medSourceItemModel::populateLevel(QModelIndex const &index, QString const &key)
{
    //QVariantList entries; //QList<QList<QString>> list of entries of the given level, each entry has a list of 3 elements {key, name, description}. Key is never displayed, only used to fetch sub-level and used has unique key
    QList<QMap<QString, QString>> entries;
    medDataModelItem *pItem = getItem(index);

    int iLevel = pItem->level()+1;

    if (!d->columnNameByLevel.contains(iLevel))
    {
        fetchColumnNames(index);
    }

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
            // Populate data loop
            for (QMap<QString, QString> &var : entries)
            {
                medDataModelItem *pItemTmp = new medDataModelItem(this);              
                for (QString k : var.keys())
                {
                    int iCol = d->columnNameByLevel[iLevel].indexOf(k);
                    if (iCol == -1)
                    {
                        d->columnNameByLevel[iLevel].push_back(k);
                        if (!d->sectionNames.contains(k))
                        {
                            qDebug() << "[WARN] Unknown column name " << k << " from getMandatoryAttributesKeys in datasource " << d->sourceInstanceId << " at level " << iLevel-1;                            
                            d->sectionNames.push_back(k);
                            emit columnCountChange(d->sectionNames.size());
                            iCol = d->columnNameByLevel[iLevel].size()-1;
                        }
                    }
                    pItemTmp->setData(var[k], iCol);
                }
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

void medSourceItemModel::populateLevelV2(QModelIndex const & index, QString const & uri)
{
    //QVariantList entries; //QList<QList<QString>> list of entries of the given level, each entry has a list of 3 elements {key, name, description}. Key is never displayed, only used to fetch sub-level and used has unique key
    
    QList<QMap<QString, QString>> entries;
    medDataModelItem *pItem = getItem(index);
    int iLevel = pItem->level() + 1;

    if (d->parent->getLevelMetaData(d->sourceInstanceId, iLevel, uri, entries))
    {
        emit layoutAboutToBeChanged(); //this is useful to update arrow on the left if click is not inside

        QVector<QPair<int, int> > rangeToRemove; // vector of ranges to delete, <beginRange, endRange>
        computeRowRangesToRemove(pItem, entries, rangeToRemove);
        removeRowRanges(rangeToRemove, index);

        //TODO Update data already present inside the model

        QMap<int, QList<QMap<QString, QString>>> entriesToAdd; //position to insert, List of QVariant, itself QVariantList representation of minimal entries
        computeRowRangesToAdd(pItem, entries, entriesToAdd);
        addRowRanges(entriesToAdd, index);

        emit layoutChanged(); // close the emit layoutAboutToBeChanged();
    }
}


bool medSourceItemModel::itemStillExist(QList<QMap<QString, QString>> &entries, medDataModelItem * pItem)
{
    bool bFind = false;

    auto it = entries.begin();
    auto end = entries.end();
    while ((it != end) && !bFind)
    {
        bFind = pItem->iid() == (*it).first();// [(*it).keys[0]]; //(*it).values()[0];
        ++it;
    }

    return bFind;
}

void medSourceItemModel::computeRowRangesToRemove(medDataModelItem * pItem, QList<QMap<QString, QString>> &entries, QVector<QPair<int, int>> &rangeToRemove)
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

void medSourceItemModel::removeRowRanges(QVector<QPair<int, int>> &rangeToRemove, const QModelIndex & index)
{
    int iOffsetRange = 0; //Accumulate deletions count to correct ranges still to delete
    for (auto &range : rangeToRemove)
    {
        removeRows(range.first - iOffsetRange, range.second - range.first + 1, index); //Used Override removeRows of QAbstractItemModel
        iOffsetRange += range.second + 1 - range.first; //Update the offset
    }
}



void medSourceItemModel::computeRowRangesToAdd(medDataModelItem * pItem, QList<QMap<QString, QString>> &entries, QMap<int, QList<QMap<QString, QString>>> &entriesToAdd)
{
    int  iLastItemAlreadyPresent = -1;
    for (QMap<QString, QString> &var : entries)
    {
        int iTmpLastItemAlreadyPresent = pItem->childIndex(var.first());

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

void medSourceItemModel::addRowRanges(QMap<int, QList<QMap<QString, QString>>> &entriesToAdd, const QModelIndex & index)
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
        for (QMap<QString, QString> &var : entriesToAdd[startRangeList[i]])
        {
            medDataModelItem *pItemTmp = new medDataModelItem(this);
            for (QString k : var.keys())
            {
                pItemTmp->setData(var[k], getColumnInsideLevel(pItem->level() + 1, d->sectionNames.indexOf(k))+1);
            }
            pItemTmp->setParent(pItem);
            pItem->insert(first + iOffsetRange, pItemTmp);            
        }
        iOffsetRange += first + last;

        endInsertRows();
    }
}


