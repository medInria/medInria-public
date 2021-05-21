#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QSortFilterProxyModel>
#include <QtCore>
#include <QVector>

#include <medCoreLegacyExport.h>

/**
 * Proxy model that sits between a model and a view and filters + sorts items
 */
class MEDCORELEGACY_EXPORT medDatabaseProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    medDatabaseProxyModel(QObject *parent = nullptr);
    ~medDatabaseProxyModel();

    void setFilterRegExpWithColumn(const QRegExp &regExp, int column);

    void clearAllFilters();

protected slots:
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;
    bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const;
    bool customFilterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const;
private:
    mutable unsigned int isCheckingChildren;
    mutable unsigned int isCheckingParents;
    QHash<int,QRegExp> filterVector;
    mutable int currentKey;
    mutable QRegExp currentValue;
};
