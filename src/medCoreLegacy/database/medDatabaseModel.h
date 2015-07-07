/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <QtCore/QAbstractItemModel>
#include <QtGui>

class medAbstractDatabaseItem;
class medDatabaseModelPrivate;
class medDataIndex;

class MEDCORE_EXPORT medDatabaseModel : public QAbstractItemModel
{
    Q_OBJECT

public:
     medDatabaseModel(QObject *parent = 0, bool justBringStudies = false);
    ~medDatabaseModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    int columnIndex(const QString& title) const;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    QModelIndex index(int row, int colum, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index = QModelIndex()) const;

    Qt::ItemFlags flags(const QModelIndex& index) const;

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::EditRole);

    bool insertColumns(int position, int columns, const QModelIndex& parent = QModelIndex());
    bool removeColumns(int position, int columns, const QModelIndex& parent = QModelIndex());

    bool insertRows(int position, int rows, const QModelIndex& parent = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex& parent = QModelIndex());
    bool removeAllRows();

    QStringList mimeTypes() const;

    Qt::DropActions supportedDropActions() const;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex& parent);

    QMimeData *mimeData(const QModelIndexList &indexes) const;

    QStringList columnNames() const;

    bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;

protected slots:
    void repopulate();

protected:
    void populate(medAbstractDatabaseItem *parent);

private:
    medDatabaseModelPrivate *d;
public slots:
    void update(const medDataIndex&);

private:
    void updateSerie(const medDataIndex&);
    void updateStudy(const medDataIndex&, bool updateChildren = true);
    void updatePatient(const medDataIndex&, bool updateChildren = true);
    QVariant convertQStringToQVariant(QString key, QString value);
    void changePersistenIndexAndSubIndex(QModelIndex index);
};


