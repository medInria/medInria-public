#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSourceHandler.h>
#include <medSourceItemModel.h>

#include <medCoreExport.h>
#include <QMap>
#include <QList>
#include <QAbstractItemModelTester>

#include <medDataIndex.h>
#include <medAbstractData.h>
#include <medDefaultWritingPolicy.h>

#include <dtkCoreSupport/dtkSmartPointer.h>


enum asyncRequestType { getRqstType = 1, addRqstType = 2 };
struct asyncRequest
{
    asyncRequest() {stampTimeout = QDateTime::currentSecsSinceEpoch() + 100;}
    asyncRequest(const asyncRequest &rqst) { *this = rqst; }
    asyncRequest & operator=(asyncRequest const & rqst) { type = rqst.type; tmpId = rqst.tmpId; uri = rqst.uri; stampTimeout = rqst.stampTimeout;  return *this; }
    asyncRequestType type;
    QString tmpId;
    QStringList uri;

    QString dataName;

    qint64 stampTimeout;
    //QTimer timeOut;
    QEventLoop waiter;
    bool needMedAbstractConversion;

public:
    friend bool operator< (asyncRequest const & a, asyncRequest const & b);
    friend bool operator> (asyncRequest const & a, asyncRequest const & b);
    friend bool operator==(asyncRequest const & a, asyncRequest const & b);
};

inline bool operator< (asyncRequest const & a, asyncRequest const & b) { return a.uri < b.uri; };
inline bool operator> (asyncRequest const & a, asyncRequest const & b) { return a.uri > b.uri; };
inline bool operator==(asyncRequest const & a, asyncRequest const & b) { return a.uri == b.uri && a.tmpId == b.tmpId && a.type == b.type; };



class MEDCORE_EXPORT medDataHub : public QObject
{
    Q_OBJECT

public:
    using datasetAttributes = QMap<QString, QString>;
    using levelAttributes = QList<datasetAttributes>;

    static medDataHub* instance(QObject *parent = nullptr);
	~medDataHub();

public:
	// ////////////////////////////////////////////////////////////////////////////////////////////
    // Members functions to get Data, metadata and informations                Datasets handling
    QString getDataName(medDataIndex const & index);
    medAbstractData * getData(medDataIndex const & index);
    datasetAttributes getMetaData(medDataIndex const & index);
    bool saveData(medAbstractData *pi_pData, QString const &pi_baseName, QStringList &pio_uri);
    bool getChildrenNames(medDataIndex index, QStringList &names);
    bool createPath(QString pi_sourceId, QStringList pi_folders, QStringList &po_uri, QMap<int, QString> pi_knownKeys = QMap<int, QString>());

    bool writeResults(QString pi_sourceId, medAbstractData * pi_pData, QStringList pi_UriOfRelatedData, QString pi_basePath, medWritingPolicyData & pi_writingPolicyData, medAbstractWritingPolicy * pi_pWritingPolicy);
    QUuid writeResultsHackV3(medAbstractData &data, bool originSrc); //To Adapt

	bool fetchData(medDataIndex const & index);
	bool pushData(medDataIndex const & index);


    // ////////////////////////////////////////////////////////////////////////////////////////////
    // Members functions for GUI    
    // ------------  Members functions to deal with datamodel                   Advanced Accessors
    QList<medSourceItemModel*> models(); // rediscute de son nom
    medSourceItemModel* getModel(QString const & pi_sourceInstanceId); //also used internally
    void expandAll(const QString &sourceInstanceId);
    void sourceIsOnline(QString sourceIntanceId);


public slots:
   void addSource(QString const & pi_sourceId);
   void removeSource(QString const & pi_sourceId);
   void refresh(medDataIndex pi_index); //TODO est-il utile  //uri -> sourceInstanceId/IdLevel1/IdLevel.../IdLevelN
   void unloadData(QObject *obj); /* REDO */
   int  waitGetAsyncData(const QString &sourceId, int rqstId);
   void progress(const QString &sourceId, int rqstId, medAbstractSource::eRequestStatus status);


   void timeOutWatcher();

signals:
    void abortRequest  (int); //abort the requestId
    void getAsyncStatus(medAbstractSource* , int, medAbstractSource::eRequestStatus);
    void sourceAdded   (QString /*sourceInstanceId*/);
    void sourceRemoved (QString /*sourceInstanceId*/);

private:
    medDataHub(QObject *parent = nullptr);
	medAbstractData * variantToMedAbstractData(QVariant &data, const medDataIndex & index);

    QVariant prepareDataToWrite(QString &sourceId, medAbstractData * &pi_pData, QStringList & pio_uri);
    bool warpAddAsync(medAbstractSource::levelMinimalEntries &minimalEntries, QStringList & pio_uri, medSourceItemModel * pModel, unsigned int uiLevel, QString &parentKey, medAbstractData * pi_pData, QVariant &data);

    void getDirectData(const medDataIndex & index, medAbstractData * &pDataRes);
    void getAsyncData (const medDataIndex & index, medAbstractData * &pDataRes);
    static QString getTmpUuid();
    QString convertToPath(QStringList pi_uri);

    void removeRequest(QString sourceId, int rqstId);

private:
    medSourceHandler * m_sourcesHandler;
    QMap< QString, medSourceItemModel*> m_sourceIdToModelMap;


    QMap< QString /*sourceId*/, QMap<int/*requestId*/, asyncRequest> > m_requestsMap;
    QMap< asyncRequest, std::shared_ptr<medNotif>> m_rqstToNotifMap;
	
    QTimer m_clock;

    QMap<medDataIndex, dtkSmartPointer<medAbstractData> > m_IndexToData;
    static medDataHub * s_instance;
};

