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
    //int iCol; //à supprimer
    QMap<unsigned int, QStringList> columnNameByLevel;

    medDataModelItem *root;
};

medDataModelElement::medDataModelElement(medDataModel *parent, QString const & sourceIntanceId) : d(new medDataModelElementPrivate)
{
    d->parent = parent;
    d->sourceInstanceId = sourceIntanceId;
    d->root = new medDataModelItem();

    bool bOk = parent->getSourceGlobalInfo(sourceIntanceId, d->columnNameByLevel[0], d->bOnline, d->bWritable, d->bCache);

    if (bOk)
    {
        //setColumnAttributes(-1, d->columnNameByLevel[0]); //si on supprime ce param dans parent->getSourceGlobalInfo
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
        // Maybe TODO si on supprime ce param dans parent->getSourceGlobalInfo
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
