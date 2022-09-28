#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSourcesLoader.h>

#include <medCoreExport.h>
#include <QMap>
#include <QList>

#include <medDataIndex.h>
#include <medAbstractData.h>
#include <medDefaultWritingPolicy.h>

#include <medNotif.h>

#include <dtkCoreSupport/dtkSmartPointer.h>



class MEDCORE_EXPORT medSourceHandler : public QObject
{
    Q_OBJECT

public:
    using datasetAttributes = QMap<QString, QString>;
    using levelAttributes = QList<datasetAttributes>;
    enum asyncRequestType { getRqstType = 1, addRqstType = 2 };
    struct asyncRequest
    {
        asyncRequestType type;
        QString tmpId;
        QStringList uri;
    public:
        //bool operator<(asyncRequest const & other);
        //bool operator>(asyncRequest const & other);
        //bool operator==(asyncRequest const & other);
        friend bool operator< (medSourceHandler::asyncRequest const & a,  medSourceHandler::asyncRequest const & b);
        friend bool operator> (medSourceHandler::asyncRequest const & a,  medSourceHandler::asyncRequest const & b);
        friend bool operator==(medSourceHandler::asyncRequest const & a,  medSourceHandler::asyncRequest const & b);
    };

    static medSourceHandler* instance(QObject *parent = nullptr);
	~medSourceHandler();

    // ////////////////////////////////////////////////////////////////////////////////////////////
    // Members functions to interrogate sources
    bool sourceGlobalInfo       (QString const & pi_sourceInstanceId, bool &po_bOnline, bool & po_bLocal, bool &po_bWritable, bool &po_bCache);
    bool mandatoryAttributesKeys(QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QStringList & po_attributes);
    bool attributesForBuildTree (QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QString const & key, levelAttributes & po_entries);
    bool mandatoriesAttributes  (QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QString const & parentKey, levelAttributes & po_entries);
    bool optionalAttributes     (QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QString const & key, datasetAttributes & po_attributes, datasetAttributes & po_tags);
    bool getDirectData          (QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QString const & key, QVariant & po_data);
    bool addDirectData          (QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QString const & parentKey, QVariant const & pi_data, medAbstractSource::levelMinimalEntries & pio_minimalEntries);
    bool createFolder           (QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QString const & parentKey, medAbstractSource::levelMinimalEntries & pio_minimalEntries, medAbstractSource::datasetAttributes4 const &pi_attributes);
    bool commitData             (QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QString const & parentKey, QVariant const & pi_data, medAbstractSource::levelMinimalEntries & pio_minimalEntries);
    bool getAsyncData           (QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QString const & key);
    bool getLevelDesiredWritable(QString const & pi_sourceInstanceId, int & po_iLevel);
    bool isLevelWritable        (QString const & pi_sourceInstanceId, int pi_iLevel, bool & po_bWritable);
	bool levelCount             (QString const & pi_sourceInstanceId, unsigned int &po_uiLevelMax);
    bool asyncResult            (QString const & pi_sourceInstanceId, int pi_iRequest);
    bool abortRequest           (QString const & pi_sourceInstanceId, int pi_iRequest); //TODO call in medSourceItemModel
    bool filteringParameters    (QString const & pi_sourceInstanceId, QList<medAbstractParameter*> & po_parameters);
    bool getIOInterface         (QString const & pi_sourceInstanceId, int &po_iInterface);
    bool getTypeAndFormat       (QString const & pi_sourceInstanceId, QMap<QString, QStringList> &po_TypeAndFormat);
    bool connect                (QString const & pi_sourceInstanceId, bool pi_bOnline);

    // ////////////////////////////////////////////////////////////////////////////////////////////
    // Members functions to advance exchange with sources
    int push                (medDataIndex const & pio_index);
    int getAsyncData        (medDataIndex const & pio_index);
    int  addAssyncData      (medDataIndex const & pio_index, QVariant const & pi_data, medAbstractSource::levelMinimalEntries & pio_minimalEntries);
    bool getAsyncResults    (QString const & pi_sourceInstanceId, int pi_request, QVariant & po_data);
    QString getInstanceName (QString const & pi_sourceInstanceId); //Probably used in medSourceModelPresenter and medSourceWidget

    // ////////////////////////////////////////////////////////////////////////////////////////////
    // Handle asynchronous requests
    bool addRequest(QString sourceIntanceId, int pi_request, asyncRequest & request);
    bool getRequest(QString sourceIntanceId, int pi_request, asyncRequest & request);
    bool removeRequest(QString sourceIntanceId, int pi_request);

signals:
    void getAsyncStatus(QString const /*sourceId*/, int /*requestId*/, medAbstractSource::eRequestStatus /*status*/);
    void sourceAdded   (QString const /*sourceId*/);
    void sourceRemoved (QString const /*sourceId*/);


public:
    // ////////////////////////////////////////////////////////////////////////////////////////////
    // Writing source helper
    void setDefaultWorkingSource(medAbstractSource * pi_pSource);
    medAbstractSource * getDefaultWorkingSource();
    medAbstractSource * getSourceToWrite(QString pi_sourceIdDst = "");

    // ////////////////////////////////////////////////////////////////////////////////////////////
    // Writing Policy
    medAbstractWritingPolicy* getSourceWPolicy(QString pi_sourceId);
    medAbstractWritingPolicy* getGeneralWPolicy(){ return &m_generalWritingPolicy; }



public slots:
    void addSource(medAbstractSource* pi_source);
    void removeSource(medAbstractSource* pi_source);

    void sourceIsOnline(QString sourceIntanceId);/* REDO */
    void progress(int pi_iRequest, medAbstractSource::eRequestStatus status);


private:
    medAbstractSource * getSource(QString const pi_sourceInstanceId);
    medSourceHandler(QObject *parent = nullptr);

private:
    QStringList m_sourceInstanceIdOrderedList;
    QMap< QString, medAbstractSource*> m_sourceIdToInstanceMap;
    QMap< QString /*sourceId*/, QMap<int/*requestId*/, asyncRequest> > m_requestsMap;

    QMap< asyncRequest, std::shared_ptr<medNotif>> m_rqstToNotifMap;
    
    medAbstractSource* m_defaultSource;
    medDefaultWritingPolicy m_generalWritingPolicy;

    static medSourceHandler * s_instance;
};

inline bool operator< (medSourceHandler::asyncRequest const & a, medSourceHandler::asyncRequest const & b) { return a.uri < b.uri; };
inline bool operator> (medSourceHandler::asyncRequest const & a, medSourceHandler::asyncRequest const & b) { return a.uri > b.uri; };
inline bool operator==(medSourceHandler::asyncRequest const & a, medSourceHandler::asyncRequest const & b) { return a.uri == b.uri && a.tmpId == b.tmpId && a.type == b.type; };