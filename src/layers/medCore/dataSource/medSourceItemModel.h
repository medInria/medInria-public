#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QAbstractItemModel>

#include <medDataModelItem.h>

#include <medCoreExport.h>

class medDataModel;
struct medDataModelElementPrivate;

class MEDCORE_EXPORT medSourceItemModel : public QAbstractItemModel
{

    Q_OBJECT

public:

    using datasetAttributes = QMap<QString, QString>;
    using levelAttributes = QList<datasetAttributes>;
    struct datasetAttributes4
    {
          QMap<QString, QVariant> values; // <keyName, value>
          QMap<QString, QString> tags;   // <keyName, tag value>
    
    };

    medSourceItemModel(medDataModel *parent, QString const & sourceIntanceId);
    virtual ~medSourceItemModel();

    medDataModel * model();

    // ////////////////////////////////////////////////////////////////////////
    // Pure Virtual Override
    QVariant	data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex	index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex	parent(const QModelIndex &index) const override;

    int	columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int	rowCount(const QModelIndex &parent = QModelIndex()) const override;


    // ////////////////////////////////////////////////////////////////////////
    // Simple Virtual Override
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;


    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;


    Qt::DropActions supportedDropActions() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;


    // ////////////////////////////////////////////////////////////////////////
    // Simple methods
    //void setColumnAttributes(int p_iLevel, QStringList &attributes); //maybe developed because not const ?
    int  getColumnInsideLevel(int level, int section) const;
    int  getSectionInsideLevel(int level, int column) const;
    bool fetch(QString uri);
    QString getSourceIntanceId();
    void setOnline(bool pi_bOnline);

    datasetAttributes getMendatoriesMetaData(QModelIndex const & index);
    QList<QMap<int, QString>> getAdditionnalMetaData(QModelIndex const & index);
    bool setAdditionnalMetaData(QModelIndex const & index, QList<QMap<int, QString>> &additionnalMetaData);

    //FLO
    QModelIndex toIndex(QString uri);
    QString     toUri(QModelIndex index);
    bool        setAdditionnalMetaData2(QModelIndex const & index, datasetAttributes4 const &attributes);
    bool        setAdditionnalMetaData2(QModelIndex const & index, QString const & key, QVariant const & value, QString const & tag = QString() );
    bool        additionnalMetaData2(QModelIndex const & index, datasetAttributes4 & attributes);
    bool        additionnalMetaData2(QModelIndex const & index, QString const & key, QVariant & value, QString & tag);


    //JU
    void expandAll(QModelIndex index = QModelIndex(), QString key = "");

public slots:
    void itemPressed(QModelIndex const &index);



private:    
    medDataModelItem* getItem(const QModelIndex &index) const;
    QModelIndex getIndex(QString iid, QModelIndex const &parent = QModelIndex()) const;
    bool fetchColumnNames(const QModelIndex &index);
    void populateLevel(QModelIndex const &index, QString const &key);

    void populateLevelV2(QModelIndex const &index, QString const & key);

    bool itemStillExist(QList<QMap<QString, QString>> &entries, medDataModelItem * pItem);
    void computeRowRangesToRemove(medDataModelItem * pItem, QList<QMap<QString, QString>> &entries, QVector<QPair<int, int>> &rangeToRemove);
    void removeRowRanges(QVector<QPair<int, int>> &rangeToRemove, const QModelIndex & index);

    void computeRowRangesToAdd(medDataModelItem * pItem, QList<QMap<QString, QString>> &entries, QMap<int, QList<QMap<QString, QString>>> &entriesToAdd);
    void addRowRanges(QMap<int, QList<QMap<QString, QString>>> &entriesToAdd, const QModelIndex & index);

    bool currentLevelFetchable(medDataModelItem * pItemCurrent);



signals:
    void online(bool);
    void columnCountChange(int);

private:
    medDataModelElementPrivate* d;


};