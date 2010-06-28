/* medDatabaseModel.h ---
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct 14 22:54:51 2008 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 21 15:18:16 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 123
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASEMODEL_H
#define MEDDATABASEMODEL_H

#include "medSqlExport.h"

#include <QtCore/QAbstractItemModel>
#include <QtGui>

class medDatabaseItem;
class medDatabaseModelPrivate;

class MEDSQL_EXPORT medDatabaseModel : public QAbstractItemModel
{
    Q_OBJECT
  
public:
     medDatabaseModel(QObject *parent = 0);
    ~medDatabaseModel(void);

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    int columnIndex(const QString& title) const;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    QModelIndex indexForPatient(int id) const;
    QModelIndex indexForStudy(int id) const;
    QModelIndex indexForSeries(int id) const;
    QModelIndex indexForImage(int id) const;

    QModelIndex index(int row, int colum, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index = QModelIndex()) const;

    Qt::ItemFlags flags(const QModelIndex& index) const;

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::EditRole);

    bool insertColumns(int position, int columns, const QModelIndex& parent = QModelIndex());
    bool removeColumns(int position, int columns, const QModelIndex& parent = QModelIndex());

    bool insertRows(int position, int rows, const QModelIndex& parent = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex& parent = QModelIndex());

    QStringList mimeTypes(void) const;

    Qt::DropActions supportedDropActions(void) const;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex& parent);

protected slots:
    void clear(void);

protected:
    void populate(medDatabaseItem *parent);

private:
    medDatabaseModelPrivate *d;
};

#endif
