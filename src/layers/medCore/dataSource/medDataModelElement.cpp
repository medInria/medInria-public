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

medDataModelElement::medDataModelElement(medDataModel *parent, QString const & sourceIntanceId)
{
    m_parent = parent;
    m_SourceInstanceId = sourceIntanceId;
        
    bool bOk = parent->getSourceGlobalInfo(sourceIntanceId, m_columnName, m_bOnline, m_bWritable, m_bCache);

    if (bOk)
    {
        if (m_bCache)
        {
            m_columnName.push_back("Cached");
        }
        if (m_bOnline)
        {
            unsigned int iLevel = 0;
            QVariantList entries;
            parent->getInfoLevel(sourceIntanceId, iLevel, entries);
            m_iRow = entries.size();
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
    return QModelIndex();
}

int medDataModelElement::columnCount(const QModelIndex & parent) const
{
    int iRes = 0;
    
    if (!parent.isValid())
    {
        iRes = m_columnName.size();
    }
    
    return 0;
}

int	medDataModelElement::rowCount(const QModelIndex &parent) const
{
    int iRes = 0;

    medDataModelItem *parentItem;

    if (parent.column() == 0)
    {
        if (!parent.isValid())
        {
            iRes = m_iRow;
        }
        else
        {
            parentItem = static_cast<medDataModelItem *>(parent.internalPointer());
            iRes = parentItem->childCount();
        }
    }    

    return iRes;
}

void medDataModelElement::setColumnAttributes(unsigned int p_uiLevel, QStringList & attributes)
{
}

void medDataModelElement::itemPressed(QModelIndex const &index)
{

}
