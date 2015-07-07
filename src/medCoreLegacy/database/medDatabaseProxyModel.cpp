/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseProxyModel.h>

#include <QtCore>

medDatabaseProxyModel::medDatabaseProxyModel( QObject *parent /*= 0*/ ):
    QSortFilterProxyModel(parent)
{
    isCheckingChild = false;
    isCheckingParent = false;
}

medDatabaseProxyModel::~medDatabaseProxyModel()
{

}

bool medDatabaseProxyModel::filterAcceptsRow( int source_row, const QModelIndex & source_parent ) const
{
    // checking all stored keys
    QHash<int, QRegExp>::const_iterator i = filterVector.constBegin();
    while (i != filterVector.constEnd())
    {
        currentKey = i.key();
        currentValue = i.value();
        if (!customFilterAcceptsRow(source_row, source_parent))
            return false;
        i++;
    }

    // all good
    return true;
}

bool medDatabaseProxyModel::filterAcceptsColumn( int source_column, const QModelIndex &source_parent ) const
{
    return QSortFilterProxyModel::filterAcceptsColumn(source_column, source_parent);
}

void medDatabaseProxyModel::setFilterRegExpWithColumn( const QRegExp &regExp, int column )
{
    filterVector[column] = regExp;
    filterChanged();
}

void medDatabaseProxyModel::clearAllFilters()
{
    filterVector.clear();
}

bool medDatabaseProxyModel::customFilterAcceptsRow( int source_row, const QModelIndex & source_parent ) const
{

    // get the current model index
    QModelIndex current(sourceModel()->index(source_row, 0, source_parent));
    // get the data we want to check
    QModelIndex index = sourceModel()->index(source_row, currentKey, source_parent);

    // show all children if parent is valid
    if (current.parent().isValid() && !isCheckingChild && !isCheckingParent)
    {
        isCheckingParent = true;
        if(customFilterAcceptsRow(current.parent().row(), current.parent().parent()))
            return true;
    }

    // show the parent if one child is valid
    if(sourceModel()->hasChildren(current) && !isCheckingChild && !isCheckingParent )
    {
        bool atLeastOneValidChild = false;
        int i = 0;
        while(!atLeastOneValidChild)
        {
            const QModelIndex child(current.child(i, 0  ));
            if(!child.isValid())
                break;
            isCheckingChild = true;
            atLeastOneValidChild = customFilterAcceptsRow(i, current);
            i++;
        }
        if (atLeastOneValidChild)
            return true;
    }

    // set back
    isCheckingParent = false;
    isCheckingChild = false;

    // ignoring invalid items
    if (!current.isValid() || !index.isValid())
        return true;

    // pattern to data check
    if (!sourceModel()->data(index).toString().contains(currentValue))
        return false;

    // if we make it up to here we are good
    return true;
}
