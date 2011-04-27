#ifndef medDatabaseProxyModel_h__
#define medDatabaseProxyModel_h__

#include <QSortFilterProxyModel>
#include "medSqlExport.h"


/**
 * Proxy model that sits between a model and a view and filters + sorts items
 */
class MEDSQL_EXPORT medDatabaseProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    medDatabaseProxyModel(QObject *parent = 0);
    ~medDatabaseProxyModel();

protected slots:
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;
    bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const;

private:
    mutable bool isCheckingChild;
};
#endif // medDatabaseProxyModel_h__
