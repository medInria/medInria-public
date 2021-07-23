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
    int iRow;
    int iCol;
    QMap<unsigned int, QStringList> columnNameByLevel;
};

medDataModelElement::medDataModelElement(medDataModel *parent, QString const & sourceIntanceId)
{
    d->parent = parent;
    d->sourceInstanceId = sourceIntanceId;
        
    bool bOk = parent->getSourceGlobalInfo(sourceIntanceId, d->columnNameByLevel[0], d->bOnline, d->bWritable, d->bCache);

    if (bOk)
    {
        if (d->bCache)
        {
            d->columnNameByLevel[0].push_back("Cached");
        }
        if (d->bOnline)
        {
            unsigned int iLevel = 0;
            QVariantList entries;
            parent->getLevelMetaData(sourceIntanceId, iLevel, entries);
            d->iRow = entries.size();
        }        
    }
}

medDataModelElement::~medDataModelElement()
{
}

QVariant medDataModelElement::data(const QModelIndex & index, int role) const
{
    return QVariant();
}

QModelIndex medDataModelElement::index(int row, int column, const QModelIndex & parent) const
{
    return QModelIndex();
}

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

int medDataModelElement::columnCount(const QModelIndex & parent) const
{
    int iRes = 0;
    
    if (!parent.isValid())
    {
        iRes = getLevelColumCount(0);
    }
    else
    {
        medDataModelItem *parentItem = nullptr;
        parentItem = static_cast<medDataModelItem *>(parent.internalPointer());
        unsigned int level = parentItem->childLevel();
        iRes = getLevelColumCount(level);
    }
    
    return iRes;
}
int	medDataModelElement::rowCount(const QModelIndex &parent) const
{
    int iRes = 0;    

    if (parent.column() == 0)
    {
        if (!parent.isValid())
        {
            iRes = d->iRow;
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
    else
    {
        // Maybe TODO
    }
}

void medDataModelElement::itemPressed(QModelIndex const &index)
{

}




/* ***********************************************************************/
/* *************** Private functions and slots ***************************/
/* ***********************************************************************/
int medDataModelElement::getLevelColumCount(unsigned int pi_uiLevel) const
{
    int iRes = -1;

    if (d->columnNameByLevel.contains(pi_uiLevel))
    {
        iRes = d->columnNameByLevel[pi_uiLevel].size();
    }
    else
    {
        QStringList attributes;
        if (d->parent->getLevelAttributes(d->sourceInstanceId, pi_uiLevel, attributes))
        {
            d->columnNameByLevel[pi_uiLevel] = attributes;
            iRes = attributes.size();
        }
    }

    return iRes;
}
