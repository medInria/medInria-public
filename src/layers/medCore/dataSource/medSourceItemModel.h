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

class medDataHub;
struct medDataModelElementPrivate;

#define MEDDATA_ROLE   102  //Boolean indicate if a medAbstractData is attached directly or indirectly to an item
#define DATASTATE_ROLE 100  //String defined as below
#define DATASTATE_ROLE_DATANOTLOADED  "DataNotLoaded"
#define DATASTATE_ROLE_DATALOADING    "DataLoading"
#define DATASTATE_ROLE_DATALOADED     "DataLoaded"
#define DATASTATE_ROLE_DATANOTSAVED   "DataNotSaved"
#define DATASTATE_ROLE_DATACOMMITTED  "DataCommited"
#define DATASTATE_ROLE_DATAPUSHING    "DataPushing"
#define DATASTATE_ROLE_DATASAVED      "DataSaved"

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
    //enum asyncRequestType {getRqstType = 1, addRqstType = 2};
	//struct asyncRequest
	//{
    //    asyncRequestType type;
	//	QString tmpId;
	//	QStringList uri;
	//};

    medSourceItemModel(medDataHub *parent, QString const & sourceIntanceId);
    virtual ~medSourceItemModel();

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

	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::DropActions supportedDropActions() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;


    // ////////////////////////////////////////////////////////////////////////
    // Simple methods
    QString getColumnNameByLevel(int iLevel, int iCol) const;
    int  getColumnInsideLevel(int level, int section) const;
    int  getSectionInsideLevel(int level, int column) const;
    bool fetch(QStringList uri);
	bool fetchData(QModelIndex index);
    QString getSourceIntanceId();
    void setOnline(bool pi_bOnline);

    datasetAttributes getMendatoriesMetaData(QModelIndex const & index);
    QList<QMap<int, QString>> getAdditionnalMetaData(QModelIndex const & index);
    bool setAdditionnalMetaData(QModelIndex const & index, QList<QMap<int, QString>> &additionnalMetaData);

    //FLO
    QString           getDataName(const QModelIndex &index) const;
    QString           getDataName(QStringList const &uri) const;
    medDataModelItem* getItem(const QModelIndex &index) const;
    medDataModelItem* getItem(QStringList const &uri) const;
    QModelIndex toIndex(QString uri) const;
    QModelIndex toIndex(QStringList uri) const;
    QString     toPath(QModelIndex const & index);
    QStringList fromPath(QStringList humanUri);
    QString     keyForPath(QStringList rootUri, QString folder);
    bool        getChildrenNames(QStringList uri, QStringList &names);

    bool        setAdditionnalMetaData2(QModelIndex const & index, datasetAttributes4 const &attributes);
    bool        setAdditionnalMetaData2(QModelIndex const & index, QString const & key, QVariant const & value, QString const & tag = QString() );
    bool        additionnalMetaData2(QModelIndex const & index, datasetAttributes4 & attributes);
    bool        additionnalMetaData2(QModelIndex const & index, QString const & key, QVariant & value, QString & tag);

    bool addEntry(QString pi_key, QString pi_name, QString pi_description, unsigned int pi_uiLevel, QString pi_parentKey);

    bool refresh(QModelIndex const &pi_index = QModelIndex());
    //JU
    void expandAll(QModelIndex index = QModelIndex());

public slots:
    void itemPressed(QModelIndex const &index);
    bool abortRequest(QModelIndex const &index); //abort the requestId




private:
    //medDataModelItem* getItem(const QModelIndex &index) const;
    medDataModelItem* getItem(int iLevel, QString id) const;
    QModelIndex getIndex(QString iid, QModelIndex const &parent = QModelIndex()) const;
    QModelIndex getIndex(medDataModelItem *pItem) const;
    bool fetchColumnNames(const QModelIndex &index);
    void populateLevel(QModelIndex const &index);


    bool itemStillExist(QList<QMap<QString, QString>> &entries, medDataModelItem * pItem);
    void computeRowRangesToRemove(medDataModelItem * pItem, QList<QMap<QString, QString>> &entries, QVector<QPair<int, int>> &rangeToRemove);
    void removeRowRanges(QVector<QPair<int, int>> &rangeToRemove, const QModelIndex & index);

    void computeRowRangesToAdd(medDataModelItem * pItem, QList<QMap<QString, QString>> &entries, QMap<int, QList<QMap<QString, QString>>> &entriesToAdd);
    void addRowRanges(QMap<int, QList<QMap<QString, QString>>> &entriesToAdd, const QModelIndex & index);

    bool currentLevelFetchable(medDataModelItem * pItemCurrent);

    friend class medDataModelItem;
    bool removeItem(medDataModelItem *pi_item);
    bool registerItem(medDataModelItem *pi_item);

signals:
    void online(bool);
    void columnCountChange(int);

private:
    medDataModelElementPrivate* d;


};