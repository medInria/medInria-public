/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSourceModel.h>

#include <medDataHub.h>
#include <medSourceModelItem.h>

#include <QVariantList>
#include <QSequentialIterable>

struct medDataModelElementPrivate
{
    medDataHub *parent;
    QString sourceInstanceId;
    bool bOnline;
    bool bWritable;
    bool bCache;
    bool bLocal;
    QMap<int, QStringList> columnNameByLevel;
    QStringList sectionNames;

    medSourceModelItem *root;
    QMap<int /*level*/, QList<medSourceModelItem*> > itemsMapByLevel;
};

medSourceModel::medSourceModel(medDataHub *parent, QString const & sourceIntanceId) : d(new medDataModelElementPrivate)
{
    d->parent = parent;
    d->sourceInstanceId = sourceIntanceId;
    d->root = new medSourceModelItem(this);

    bool bOk = medSourceHandler::instance()->sourceGlobalInfo(sourceIntanceId, d->bOnline, d->bLocal, d->bWritable, d->bCache);

    if (bOk)
    {
        if (d->bOnline)
        {
            populateLevel(QModelIndex());
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

medSourceModel::~medSourceModel()
{
    delete d->root;
    delete d;
}

//medDataHub * medSourceModel::model()
//{
//    return d->parent;
//}



QVariant medSourceModel::data(const QModelIndex & index, int role) const
{
    QVariant varDataRes;
    if (index.isValid())
    {
        if (role == Qt::TextAlignmentRole && index.column() > 0)
        {
            varDataRes = Qt::AlignHCenter;
        }
//        else if (role == Qt::ForegroundRole)
//        {
//            auto value = data(index, 100).toString();
//            if (value == "DataLoading")
//            {
//                varDataRes = QColor(Qt::red);
//            }
//            else if (value == "DataLoaded")
//            {
//                varDataRes = QColor(Qt::blue);
//            }
//            else if (value == "DataPushing")
//            {
//                varDataRes = QColor(Qt::yellow);
//            }
//            else if (value == "DataCommited")
//            {
//                varDataRes = QColor(Qt::cyan);
//            }
//        }
//        else if (role == Qt::DecorationRole)
//        {
//            auto value = data(index, 100).toString();
////            if (value == "DataLoading")
////            {
////                varDataRes = QIcon(":icons/yellow_spot.svg");
////            }
//        }
        else
        {
            medSourceModelItem *item = getItem(index);
            int i = index.column();
            //varDataRes = item->data(i+1, role);
            varDataRes = item->data(getColumnInsideLevel(item->iLevel, i), role);
        }
    }

    return varDataRes;
}

QModelIndex medSourceModel::index(int row, int column, const QModelIndex & parent) const
{
    QModelIndex res;

    medSourceModelItem *childItem = getItem(parent)->child(row);
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
QModelIndex medSourceModel::parent(const QModelIndex & index) const
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
int medSourceModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return d->sectionNames.size();
}

/**
* @brief  Returns the number of rows under the given parent.
* @param  parent is the index of the parent or invalid index for the top level.
* @return Returns the number of rows under the given parent. When the parent is valid it means that rowCount is returning the number of children of parent.
*/
int	medSourceModel::rowCount(const QModelIndex &parent) const
{
    int iRes = 0;    

    if (!parent.isValid() || parent.column() == 0)
    {
        iRes = getItem(parent)->childCount();
    }

    return iRes;
}

bool medSourceModel::insertRows(int row, int count, const QModelIndex &parent)
{
    bool bRes;

    medSourceModelItem * pParent = getItem(parent);
    bRes = row <= pParent->childCount() && row > -1;
    if (bRes)
    {
        beginInsertRows(parent, row, row + count - 1);
        for (int i = 0; i < count; ++i)
        {
            pParent->insert(row + i, new medSourceModelItem(pParent));
        }
        endInsertRows();
    }

    return bRes;
}

bool medSourceModel::removeRows(int row, int count, const QModelIndex & parent)
{
    bool bRes = true;
    
    medSourceModelItem *pItem = getItem(parent.siblingAtColumn(0));

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

bool medSourceModel::canFetchMore(const QModelIndex & parent) const
{
    bool bRes = false;

    if (parent.isValid())
    {
        //TODO Correct implementation
        bRes = true;// item->hasChildren && !item->children.count();
    }

    return bRes;
}

void medSourceModel::fetchMore(const QModelIndex & parent)
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

QString medSourceModel::getDataName(const QModelIndex & index) const
{
    QString nameRes;

    if (index.isValid())
    {
        nameRes = getItem(index)->data(1).toString();
    }

    return nameRes;
}

QString medSourceModel::getDataName(QStringList const & uri) const
{
    return getDataName(toIndex(uri));
}

inline medSourceModelItem * medSourceModel::getItem(const QModelIndex & index) const
{
    medSourceModelItem* itemRes = d->root;

    if (index.isValid())
    {
        medSourceModelItem *item = static_cast<medSourceModelItem*>(index.internalPointer());
        if (item)
        {
            itemRes = item;
        }
    }

    return itemRes;
}

medSourceModelItem* medSourceModel::getItem(QStringList const &uri) const
{
    medSourceModelItem * itemRes = nullptr;

    if ((uri.size() > 1) && (uri[0] == d->sourceInstanceId))
    {
        itemRes = d->root;
        for (int i = 1; i < uri.size() && itemRes; ++i)
        {
            itemRes = itemRes->child(itemRes->childIndex(uri[i]));
        }
    }

    return itemRes;
}

medSourceModelItem * medSourceModel::getItem(int iLevel, QString id) const
{
    medSourceModelItem * pItemRes = nullptr;

    if (d->itemsMapByLevel.contains(iLevel))
    {
        auto & itemLst = d->itemsMapByLevel[iLevel];
        int i = 0;
        while (i < itemLst.size() && pItemRes==nullptr)
        {
            if (itemLst[i]->iid() == id)
            {
                pItemRes = itemLst[i];
            }
            i++;
        }
    }

    return pItemRes;
}

QModelIndex medSourceModel::getIndex(QString iid, QModelIndex const &parent) const
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

QModelIndex medSourceModel::getIndex(medSourceModelItem * pItem) const
{
    QModelIndex indexRes;

    if (pItem)
    {
        indexRes = createIndex(pItem->row(), 0, pItem);
    }

    return indexRes;
}

QVariant medSourceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant varRes;

    if (role == Qt::DisplayRole && !d->sectionNames.isEmpty())
    {
        varRes = d->sectionNames[section];
    }

    return varRes;
}

bool medSourceModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	bool bRes = false;

	if ((role >= 100 || role == 0 ) && index.isValid())
	{
		getItem(index)->setData(value, 1, role);
		emit dataChanged(index, index);
		bRes = true;
	}

	return bRes;
}









Qt::DropActions medSourceModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

Qt::ItemFlags medSourceModel::flags(const QModelIndex & index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}

QStringList medSourceModel::mimeTypes() const
{
    QStringList types;
    types << "med/index2" << "text/uri-list";
    return types;
}

QMimeData * medSourceModel::mimeData(const QModelIndexList & indexes) const
{
    QMimeData *mimeData = new QMimeData;
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    for (QModelIndex const &index : indexes)
    {
        if (index.isValid() && index.column() == 0)
        {
            if (!encodedData.isEmpty())
            {
                encodedData.append('\0');
            }
            encodedData.append(getItem(index)->uriAsString().toUtf8());                       
        }
    }


    mimeData->setData("med/index2", encodedData);
    return mimeData;
}

bool medSourceModel::dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent)
{

    bool bRes = false;

    if (data->hasFormat("med/index2"))
    {
        auto indexList = medDataIndex::readMimeDataMulti(data);

        for (auto & index : indexList)
        {
            switch (action)
            {
                case Qt::CopyAction:
                case Qt::LinkAction:
                    addDataFromSource(index, nullptr, parent); break;
                case Qt::MoveAction:
                    //TODO move
                default:
                    break;
            }
        }
    }
    else if (data->hasFormat("text/uri-list"))
    {
        for (const QUrl & url : data->urls())
        {
            addDataFromFile(url.toLocalFile(), nullptr, parent);
        }
    }

    return bRes;
}



void medSourceModel::addDataFromSource(medDataIndex dataIndex, medAbstractData * data, const QModelIndex & parent)
{
    auto *pHub = d->parent;
    auto destinationIndex = dataIndexFromModelIndex(parent);

    if (!parent.isValid())
    {        
        destinationIndex.setUri(getSourceIntanceId() + ':');
    }
    
    pHub->copyData(dataIndex, destinationIndex);

}

void medSourceModel::addDataFromFile(QString path, medAbstractData * data, const QModelIndex & parent)
{
    auto *pHub = d->parent;
    auto destinationIndex = dataIndexFromModelIndex(parent);

    if (!parent.isValid())
    {
        destinationIndex.setUri(getSourceIntanceId() + ':');
    }

    pHub->copyData(path, destinationIndex);

}











QString medSourceModel::getColumnNameByLevel(int iLevel, int iCol) const
{
    QString strRes;
    if (d->columnNameByLevel.contains(iLevel))
    {
        strRes = d->columnNameByLevel[iLevel][iCol];
    }
    return strRes;
}

int medSourceModel::getColumnInsideLevel(int level, int section) const
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

int medSourceModel::getSectionInsideLevel(int level, int column) const
{
    int iRes = -1;

    if (d->columnNameByLevel.contains(level))
    {
        if (column > -1 && column < d->columnNameByLevel[level].size())
        {
            QString colName = d->columnNameByLevel[level][column];
            iRes = d->sectionNames.indexOf(colName);
        }
    }

    return iRes;
}

bool medSourceModel::fetch(QStringList uri) //See populateLevelV2
{
    bool bRes = false;

    auto index = toIndex(uri);
    if (index.isValid() || uri.size()==1)
    {
        beginResetModel();
        populateLevel(toIndex(uri));
        endResetModel();
        bRes = true;
    }

    return bRes;   
}

bool medSourceModel::fetchData(QModelIndex index)
{
	bool bRes = true;

    medDataIndex idx= getItem(index)->uri();	
    bRes = d->parent->fetchData(idx);
	setData(index, DATASTATE_ROLE_DATALOADING, DATASTATE_ROLE);

	return bRes;
}

QString medSourceModel::getSourceIntanceId()
{
    return d->sourceInstanceId;
}

void medSourceModel::setOnline(bool pi_bOnline)
{
    if (d->bOnline != pi_bOnline)
    {
        d->bOnline = pi_bOnline;
        emit online(d->bOnline);
    }
}

medSourceHandler::datasetAttributes medSourceModel::getMendatoriesMetaData(QModelIndex const & index)
{
    medSourceHandler::datasetAttributes res;

    if (index.isValid())
    {
        medSourceModelItem *pItem = getItem(index);
        int iLevel = pItem->level();
        for (int i = 0; i < d->columnNameByLevel[iLevel].size(); ++i)
        {
            res.values[d->columnNameByLevel[iLevel][i]] = pItem->data(i).toString();
        }
    }

    return res;
}

bool medSourceModel::currentOnlineStatus()
{
    return d->bOnline;
}

bool medSourceModel::currentOnlineStatus()
{
    return d->bOnline;
}

medDataIndex medSourceModel::dataIndexFromModelIndex(const QModelIndex &index) const
{
    medDataIndex dataIndexRes;

    if (index.model() == this && index.isValid())
    {
        dataIndexRes = reinterpret_cast<medSourceModelItem*>(index.internalPointer())->uri();
    }

    return dataIndexRes;
}

QModelIndex medSourceModel::toIndex(QString uri) const
{
    int sourceDelimterIndex = uri.indexOf(QString(":")); //TODO 06
    QStringList uriAsList = uri.right(uri.size() - sourceDelimterIndex - 1).split(QString("\r\n"));
    uriAsList.push_front(uri.left(sourceDelimterIndex));
    
    return toIndex(uriAsList);
}

QModelIndex medSourceModel::toIndex(QStringList uri) const
{
    QModelIndex indexRes;

    medSourceModelItem * itemTmp =  getItem(uri);
    if (itemTmp)
    {
        indexRes = createIndex(itemTmp->parent()->childItems.indexOf(itemTmp), 0, itemTmp);
    }

    return indexRes;
}

//QString medSourceModel::toUri(QModelIndex index)
//{
//    QString uriRes;
//
//    if (index.isValid())
//    {
//        auto *item = getItem(index);
//        if (item->model == this)
//        {
//            uriRes = item->uriAsString();
//        }
//    }
//
//    return uriRes;
//}

QString medSourceModel::toPath(QModelIndex const & index) const
{
    QStringList tmpNames;

    QModelIndex indexTmp = index;
    while (indexTmp.isValid())
    {
        auto *item = getItem(indexTmp);
        if (item->model == this)
        {
            tmpNames.push_front(item->data(1).toString());
        }
        indexTmp = indexTmp.parent();
    }

    return tmpNames.join("\r\n");
}

QStringList medSourceModel::fromPath(QStringList humanUri)
{
    QStringList uriRes;    

    medSourceModelItem *itemTmp = d->root;
    uriRes.push_back(d->sourceInstanceId);

    for (int i = 0; i < humanUri.size() && itemTmp; ++i)
    {
        QString keyTmp = itemTmp->iid(humanUri[i]);
        if (!keyTmp.isEmpty())
        {
            uriRes.push_back(keyTmp);
            itemTmp = itemTmp->child(itemTmp->childIndex(keyTmp));
        }
        else
        {
            itemTmp = nullptr;
        }
    }

    return uriRes;
}

QString medSourceModel::keyForPath(QStringList rootUri, QString folder)
{
    QString keyRes;

    QModelIndex index = toIndex(rootUri);
    if (index.isValid() || rootUri.size()==1) //if valid index was found or if rootUri contains only sourceId, because sourceId only return invalid root index
    {
        keyRes = getItem(index)->iid(folder);
    }

    return keyRes;
}

bool medSourceModel::getChildrenNames(QStringList uri, QStringList &names)
{
    auto* pItem = getItem(uri);
    if (pItem)
    {
        for (auto *pChild : pItem->childItems)
        {
            names.push_back(pChild->data(1).toString());
        }
    }

    return pItem != nullptr;
}

bool medSourceModel::setAdditionnalMetaData(QModelIndex const & index, medSourceHandler::datasetAttributes const & attributes)
{
    bool bRes = false;

    if (index.isValid())
    {
        medSourceModelItem *pItem = getItem(index);
        if(pItem->model == this)
        {
            pItem->setMetaData(attributes.values, attributes.tags);
            bRes = true;
        }
    }

    return bRes;
}

bool medSourceModel::getAdditionnalMetaData(QModelIndex const & index, medSourceHandler::datasetAttributes & attributes)
{
    bool bRes = false;

    if (index.isValid())
    {
        medSourceModelItem *pItem = getItem(index);
        if (pItem->model == this)
        {
            QMap<QString, QString> attr;
            for (auto key: pItem->itemMeta.keys())
            {
                attr[key] = pItem->itemMeta[key].toString();
            }
            attributes.values = attr;
            attributes.tags = pItem->itemMetaTag;
            bRes = true;
        }
    }

    return bRes;
}

bool medSourceModel::addEntry(QString pi_key, QString pi_name, QString pi_description, unsigned int pi_uiLevel, QString pi_parentKey)
{
    bool bRes = false;

    medSourceModelItem *pParentItem = getItem(pi_uiLevel-1, pi_parentKey);
    if (pParentItem != nullptr)
    {
        QModelIndex parentIndex = getIndex(pParentItem);
        emit layoutAboutToBeChanged(); //this is useful to update arrow on the left if click is not inside
        beginInsertRows(parentIndex, pParentItem->childCount(), pParentItem->childCount());

        medSourceModelItem *pNewItem = new medSourceModelItem(this);
        pNewItem->setData(pi_key, 0);
        pNewItem->setData(pi_name, 1);
        pNewItem->setData(pi_description, 3);
        pNewItem->setParent(pParentItem);
        pParentItem->append(pNewItem);
        
        endInsertRows();
        emit layoutChanged(); // close the emit layoutAboutToBeChanged();

        bRes = true;
    }

    return bRes;
}

bool medSourceModel::substituteTmpKey(QStringList uri, QString pi_key)
{
    bool bRes = pi_key.length()>0;

    if (bRes)
    {
        auto *pItem = getItem(uri);
        if (pItem)
        {
            pItem->setData(pi_key, 0);
            uri.pop_back();
            uri.push_back(pi_key);
            auto parentIndex = toIndex(uri).parent();
            emit dataChanged(parentIndex, parentIndex);
        }
        else
        {
            bRes = false;
        }
    }

    return bRes;
}

bool medSourceModel::refresh(QModelIndex const &pi_index)
{
    bool bRes = true;

    QList<QStringList> stack;
    stack.push_back(QStringList());

    auto * pStartItem = getItem(pi_index);
    int iStartLevel = pStartItem->level();

    beginResetModel();
    populateLevel(pi_index);
    endResetModel();

    // ////////////////////////////////////////////////////////////////////////
    // Init stack from root item with first sub-items with associated medAbstractData
    for (auto *pChildItem : pStartItem->childItems)
    {
        if (pChildItem->data(0, MEDDATA_ROLE).toBool())
        {
            stack[0].push_back(pChildItem->iid());
        }
    }
    iStartLevel++;

    // ////////////////////////////////////////////////////////////////////////
    // Fill stack with sub-levels item with associated medAbstractData
    for (int i = 0; i < stack.size(); ++i)
    {
        for (int iEntry = 0; iEntry < stack[i].size(); iEntry++)
        {
            auto *pItem = getItem(i + iStartLevel, stack[i][iEntry]);
            for (auto pChildItem : pItem->childItems)
            {
                if (pChildItem->data(0, MEDDATA_ROLE).toBool())
                {
                    if (stack.size() == i + 1) { stack.push_back(QStringList()); }
                    stack[i + 1].push_back(pChildItem->iid());
                }
            }
        }
    }

    // ////////////////////////////////////////////////////////////////////////
    // Perform populateLevel on each element of stack
    for (int i = 0; i < stack.size() ; ++i)
    {
        for (auto key : stack[i])
        {
            auto *pItem = getItem(i + iStartLevel, key);
            auto index = getIndex(pItem);
            beginResetModel();
            populateLevel(index);
            endResetModel();
        }
    }

    return bRes;
}

/**
* @brief  This slot refresh the current item pressed by GUI click, if the item don't have sons.
* @param  index of the GUI element clicked.
*/
void medSourceModel::itemPressed(QModelIndex const &index)
{
    if (index.isValid())
    {
        medSourceModelItem * pItemCurrent = getItem(index);
        if (currentLevelFetchable(pItemCurrent) && pItemCurrent->childCount() == 0 )
        {
            populateLevel(index);
        }
    }
}

bool medSourceModel::abortRequest(QModelIndex const & index)
{
    bool bRes = false;
    //TODO
//    auto uri = d->sourceInstanceId + ":" + getItem(index)->relativeUri().join("\r\n");
//    auto requestList = d->requestsMap.values();
//
//    int i = 0;
//    while (!bRes && i<requestList.size())
//    {
//        bRes = requestList[i].uri == uri;
//        if (bRes)
//        {
//            medSourceHandler::instance()->abortRequest(d->sourceInstanceId , d->requestsMap.keys()[i]);
//        }
//        i++;
//    }

    return bRes;
}

bool medSourceModel::currentLevelFetchable(medSourceModelItem * pItemCurrent)
{
    bool bRes = pItemCurrent->canHaveSubData();
    if (pItemCurrent->level()==-1)
    {
        bRes = true;
    }
    else
    {
        unsigned int uiLevelMax = 0;

        bRes &= medSourceHandler::instance()->levelCount(d->sourceInstanceId, uiLevelMax);
        if (uiLevelMax != 0)
        {
            bRes &= static_cast<unsigned int>(pItemCurrent->level()) < uiLevelMax - 1;
        }
    }
    return bRes;
}

bool medSourceModel::removeItem(medSourceModelItem * pi_item)
{
    bool bRes = false;

    int iLevel = pi_item->level();
    if (iLevel > -1 && d->itemsMapByLevel.contains(iLevel))
    {
        bRes = d->itemsMapByLevel[iLevel].removeOne(pi_item);
    }

    return bRes;
}

bool medSourceModel::registerItem(medSourceModelItem * pi_item)
{
    bool bRes = false;

    int iLevel = pi_item->level();
    if (iLevel > -1)
    {
        d->itemsMapByLevel[iLevel].push_back(pi_item);
        bRes = true;
    }

    return bRes;
}



/* ***********************************************************************/
/* *************** Private functions and slots ***************************/
/* ***********************************************************************/
bool medSourceModel::fetchColumnNames(const QModelIndex &index/*int const &iLevel*/)
{
    bool bRes = true;
    
    QStringList attributes;
    auto item = getItem(index);
    bRes = medSourceHandler::instance()->mandatoryAttributesKeys(d->sourceInstanceId, item->level()+1, attributes);
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
                beginInsertColumns(index, d->sectionNames.size(), d->sectionNames.size());
                d->sectionNames.push_back(attribute);
                endInsertColumns();
                emit columnCountChange(d->sectionNames.size());
            }
        }
    }

    return bRes;
}

void medSourceModel::populateLevel(QModelIndex const & index)
{
    //QVariantList entries; //QList<QList<QString>> list of entries of the given level, each entry has a list of 3 elements {key, name, description}. Key is never displayed, only used to fetch sub-level and used has unique key
    QString key = getItem(index)->iid();
    // QList<QMap<QString, QString>> entries;
    medSourceHandler::listAttributes entries;
    medSourceModelItem *pItem = getItem(index);
    int iLevel = pItem->level() + 1;


    if (!d->columnNameByLevel.contains(iLevel))
    {
        fetchColumnNames(index);
    }

    if (pItem->data(0, 101).toBool())
    {
        QVector<QPair<int, int> > rangeToRemove; // vector of ranges to delete, <beginRange, endRange>
        computeRowRangesToRemove(pItem, entries, rangeToRemove);
        removeRowRanges(rangeToRemove, index);
    }
    else
    { 
        if (medSourceHandler::instance()->attributesForBuildTree(d->sourceInstanceId, iLevel, key, entries))
        {
             emit layoutAboutToBeChanged(); //this is useful to update arrow on the left if click is not inside
            QVector<QPair<int, int> > rangeToRemove; // vector of ranges to delete, <beginRange, endRange>
            computeRowRangesToRemove(pItem, entries, rangeToRemove);
            removeRowRanges(rangeToRemove, index);
            //emit layoutChanged(); // close the emit layoutAboutToBeChanged();

            //TODO Update data already present inside the model

            //emit layoutAboutToBeChanged(); //this is useful to update arrow on the left if click is not inside
            QMap<int, QList<QMap<QString, QString>>> entriesToAdd; //position to insert, List of QVariant, itself QVariantList representation of minimal entries
            computeRowRangesToAdd(pItem, entries, entriesToAdd);
            addRowRanges(entriesToAdd, index);        
            emit layoutChanged(); // close the emit layoutAboutToBeChanged();
        }    
    }
}

bool medSourceModel::itemStillExist(medSourceHandler::listAttributes &entries, medSourceModelItem * pItem)
{
    bool bFind = false;

    QString iidColName = getColumnNameByLevel(pItem->level(), 0);

    auto it = entries.begin();
    auto end = entries.end();
    while ((it != end) && !bFind)
    {
        bFind = pItem->iid() == (*it).values[iidColName];
        ++it;
    }

    return bFind;
}

void medSourceModel::computeRowRangesToRemove(medSourceModelItem * pItem, medSourceHandler::listAttributes &entries, QVector<QPair<int, int>> &rangeToRemove)
{
    int iStartRemoveRange = -1;
    for (int i = 0; i < pItem->childCount(); ++i)
    {
        auto *pChild = pItem->child(i);
        bool bNotOnSource = !itemStillExist(entries, pChild);
        bool bNoMedDataAssociated = !pChild->containRoleValues( QMap<int, QVariantList>({ { MEDDATA_ROLE, QVariantList() } }) );

        if (bNotOnSource && bNoMedDataAssociated)
        {
            //Here pChild is no longer present inside refreshed entries
            if (iStartRemoveRange == -1)
            {
                //Here a new range starting
                iStartRemoveRange = i;
            }
        }
        else
        {
            //Here pChild is still present inside refreshed entries
            if (iStartRemoveRange > -1)
            {
                //Here the current range end
                rangeToRemove.push_back({ iStartRemoveRange, i - 1 });
                iStartRemoveRange = -1; //clean iStartRemoveRange for a future range
            }

            //Here if pChild is not present into the source, all of offspring will be mark as not present
            pChild->setData(bNotOnSource, 0, 101);
            if (bNotOnSource)
            {
                for (auto *pItemTmp : pChild->offspringList())
                {
                    pItemTmp->setData(true, 0, 101);
                }
            }
        }
    }

    if (iStartRemoveRange > -1)
    {
        //Here we finish with an opened range. Then we close the range with the last child
        rangeToRemove.push_back({ iStartRemoveRange, pItem->childCount() - 1 });
    }
}

void medSourceModel::removeRowRanges(QVector<QPair<int, int>> &rangeToRemove, const QModelIndex & index)
{
    int iOffsetRange = 0; //Accumulate deletions count to correct ranges still to delete
    if (rangeToRemove.size() > 0)
    {
        for (auto &range : rangeToRemove)
        {
            removeRows(range.first - iOffsetRange, range.second - range.first + 1, index); //Used Override removeRows of QAbstractItemModel
            iOffsetRange += range.second + 1 - range.first; //Update the offset
        }
    }
}



void medSourceModel::computeRowRangesToAdd(medSourceModelItem * pItem, medSourceHandler::listAttributes &entries, QMap<int, QList<QMap<QString, QString>>> &entriesToAdd)
{
    int  iLastItemAlreadyPresent = -1;
    
    QString iidColName = getColumnNameByLevel(pItem->level()+1, 0);
    if (!iidColName.isEmpty())
    {    
        for (medSourceHandler::datasetAttributes &var : entries)
        {
            int iTmpLastItemAlreadyPresent = pItem->childIndex(var.values[iidColName]);

            if (iTmpLastItemAlreadyPresent == -1)
            {
                entriesToAdd[iLastItemAlreadyPresent + 1].append(var.values);
            }
            else
            {
                iLastItemAlreadyPresent = iTmpLastItemAlreadyPresent;
                pItem->child(iLastItemAlreadyPresent)->setData(false, 0, 101);
            }
        }
    }
}

void medSourceModel::addRowRanges(QMap<int, QList<QMap<QString, QString>>> &entriesToAdd, const QModelIndex & index)
{
    int iOffsetRange = 0;
    auto startRangeList = entriesToAdd.keys();
    medSourceModelItem *pItem = getItem(index);
    int iLevel = pItem->level() + 1;
    for (int i = 0; i < startRangeList.size(); ++i)
    {
        int first = startRangeList[i] ;
        int last = startRangeList[i] + entriesToAdd[startRangeList[i]].size();
        beginInsertRows(index.siblingAtColumn(0), first + iOffsetRange, last + iOffsetRange - 1);

        // ////////////////////////////////////////////////////////////////////////
        // Populate data loop
        for (QMap<QString, QString> &var : entriesToAdd[startRangeList[i]])
        {
            medSourceModelItem *pItemTmp = new medSourceModelItem(this);
            for (QString k : var.keys())
            {
                int iCol = d->columnNameByLevel[iLevel].indexOf(k);
                if (iCol == -1)
                {
                    d->columnNameByLevel[iLevel].push_back(k);
                    if (!d->sectionNames.contains(k))
                    {
                        qDebug() << "[WARN] Unknown column name " << k << " from getMandatoryAttributesKeys in datasource " << d->sourceInstanceId << " at level " << iLevel-1;
                        beginInsertColumns(index, d->sectionNames.size(), d->sectionNames.size());
                        d->sectionNames.push_back(k);
                        endInsertColumns();
                        emit columnCountChange(d->sectionNames.size());
                        iCol = d->columnNameByLevel[iLevel].size()-1;
                    }
                }
                pItemTmp->setData(var[k], iCol);
                if (k == "type")
                {
                    pItemTmp->setData(var[k], 1, DATATYPE_ROLE);
                }
            }
            pItemTmp->setParent(pItem);
            pItem->insert(first + iOffsetRange, pItemTmp);            
        }
        iOffsetRange += first + last;

        endInsertRows();
    }
}

void medSourceModel::expandAll(QModelIndex index)
{
    auto item = getItem(index);
    if (currentLevelFetchable(item))
    {
        populateLevel(index);

        for (auto childItem : item->childItems)
        {
            QStringList uri = childItem->uri();
            QModelIndex childIndex = toIndex(uri);
            expandAll(childIndex);
        }
    }
}

int medSourceModel::getDataType(const QModelIndex & index)
{
    int iRes = -1;

    auto value = data(index, DATATYPE_ROLE).toString();
    if (value == "dataset")
    {
        iRes = DATATYPE_ROLE_DATASET;
    }
    else if (value == "folder")
    {
        iRes = DATATYPE_ROLE_FOLDER;
    }
    else if (value == "both")
    {
        iRes = DATATYPE_ROLE_BOTH;
    }

    return iRes;
}