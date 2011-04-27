#include "medDatabaseProxyModel.h"

#include <QtCore>

medDatabaseProxyModel::medDatabaseProxyModel( QObject *parent /*= 0*/ )
{
    isCheckingChild = false;
}

medDatabaseProxyModel::~medDatabaseProxyModel()
{

}

bool medDatabaseProxyModel::filterAcceptsRow( int source_row, const QModelIndex & source_parent ) const
{
    // return if no filter was set
    if(filterRegExp().isEmpty())
        return true;

    // get the current modelindex
    QModelIndex current(sourceModel()->index(source_row, 0, source_parent));

    // show all children if parent is valid
    if (source_parent.isValid() && !isCheckingChild)
    {
        if(filterAcceptsRow(source_row, current))
            return true;
    }

    // show the parent if one child is valid 
    if(sourceModel()->hasChildren(current))
    {
        bool atLeastOneValidChild = false;
        int i = 0;
        while(!atLeastOneValidChild)
        {
            const QModelIndex child(current.child(i, 0  ));
            if(!child.isValid())
                break;
            isCheckingChild = true;
            atLeastOneValidChild = filterAcceptsRow(i, current);
            isCheckingChild = false;
            i++;
        }
        if (atLeastOneValidChild)
           return true;
    }

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool medDatabaseProxyModel::filterAcceptsColumn( int source_column, const QModelIndex &source_parent ) const
{
    return QSortFilterProxyModel::filterAcceptsColumn(source_column, source_parent);
}
