/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include "RequestManager.h"
#include <dcmWeb.h>
#include "httpResponse.h"
#include <medAbstractSource.h>

#include <QObject>

#include <QUuid>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QTemporaryDir>
#include <QMap>
#include <QTimer>
#include <QEventLoop>

#include <QByteArray>

class dcmWeb;

class dcmRetrieveData : public QObject
{
    Q_OBJECT

public:
    explicit dcmRetrieveData(QObject *parent, RequestManager *rqManager, dcmWeb *dicomWeb);
    ~dcmRetrieveData();

    void updateUrl(const QString &url);

    bool retrieveDataService(int pi_requestId, unsigned int pi_uiLevel, const QString &uid);
    void getData(QUuid netReqId);

    bool isCachedDataPath(int requestId);
    void abortDownload(int pi_requestId);

public slots:
    void asyncGetSlot(QUuid netReqId, QByteArray payload, QList<QPair<QByteArray, QByteArray>> headers, int statusOrHttpCode);

signals:
    void asyncGet(QUuid netReqId, QNetworkRequest request); // signal emitted to launch the request
    void asyncAbort(QUuid netReqId); // signal emitted to abort connection
    void inProgress(int requestId, medAbstractSource::eRequestStatus status, QString path = ""); // signal emitted to medPACS for download status


private:
    struct retrieveParams
    {
        int m_requestId; // request Id
        QString m_path; // path in which the response data will be downloaded
        RequestResponse response; // elements of the request reponse
    };

    dcmWeb *m_dicomWeb;

    QMap<QUuid, retrieveParams> m_requestIdMap; // associate an uuid to the request id received from medPACS, download path and request response
	
    QString m_url; // url of the dicomWeb API
    QTemporaryDir m_temporaryDir; // temporary directory in which files are stored
};


/**
 * @fn void dcmRetrieveData::updateUrl(const QString &url)
 * @param url [in] url of the DICOMweb API mentioned in the plugin settings 
 * @brief set private url of the DICOMweb API from the one mentioned in the plugin settings 
*/

/**
 * @fn bool dcmRetrieveData::retrieveDataService(int pi_requestId, unsigned int pi_uiLevel, const QString &uid)
 * @brief send a signal to execute WADO-RS request (RetrieveStudy or RetrieveSeries) : construct the request url to retrieve studies, or series, according to the level
 * @details in the case of studies : url/studies/StudyInstanceUID/
 * in the case of series : url/studies/StudyInstanceUID/series/SeriesInstanceUID/
 * @param pi_requestId [in] request identifier
 * @param pi_uiLevel [in] the level from which the data is to be retrieved
 * @param uid [in] uid of the data to be retrieved
 * @return a boolean to indicate whether there is an error or not
*/

/**
 * @fn void dcmRetrieveData::asyncGetSlot(QUuid netReqId, QByteArray payload, QList<QPair<QByteArray, QByteArray>> headers, int statusOrHttpCode)
 * @brief called method when QNetworkReply sends a signal
 * @details retrieve elements of the WADO-RS request response : response status code, response body ans response main header
 * @param netReqId [in] Uuid associated to the request
 * @param payload [in] response body
 * @param headers [in] main response header
 * @param statusOrHttpCode [in] contains the status code for any error returned by the reponse, 200 if success
*/

/**
 * @fn void dcmRetrieveData::getData(QUuid netReqId)
 * @brief process the request reponse
 * @details call httpResponse to identify and separate each part (instances) of the body and save them as files in a temporary directory
 * send inProgress signals to report download status
 * @param netReqId [in] Uuid associated to the request
*/

/**
 * @fn bool dcmRetrieveData::isCachedDataPath(int pi_requestId)
 * @brief get the local path of the data if it has already been retrieved
 * @param pi_requestId [in] request id linked to the data
 * @return true if the requestId exists, and therefore if the data has already been retrieved, false otherwise
*/

/**
 * @fn void dcmRetrieveData::abortDownload(int pi_requestId)
 * @brief interrupts WADO-RS operation
 * @param pi_requestId [in] request id linked to a WADO-RS operation
*/