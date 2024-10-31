/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#ifndef DCMWEB_H
#define DCMWEB_H

#include "dcmRetrieveData.h"
#include "RequestManager.h"

#include <medAbstractSource.h>

#include <dcmtk/dcmdata/dctagkey.h>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUuid>
#include <QUrl>
#include <QTimer>
#include <QEventLoop>
#include <QPair>

#include <QString>
#include <QFile>
#include <QTemporaryDir>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

class dcmWeb : public QObject
{
    Q_OBJECT

public:
    dcmWeb(QObject *parent, RequestManager *rqManager);
    ~dcmWeb();

    void updateUrl(const QString &url);
    bool hasUrl();

    int isServerAvailable();

    QList<QMap<DcmTagKey, QString>> patientSearchService(const QMap<DcmTagKey, QString> &tags);
    QList<QMap<DcmTagKey, QString>> studySearchService(const QMap<DcmTagKey, QString> &tags);
    QList<QMap<DcmTagKey, QString>> seriesSearchService(const QMap<DcmTagKey, QString> &tags);

private:
    struct requestParams
    {
        QEventLoop *waiter; // loop
        int waiterCode; // code returned by exit() loop call
        bool upToDate; // up to date status request
        RequestResponse response; // elements of the request reponse
    };

    void updateRequestState(const QUuid &netReqId);
    QUuid waitGetResult(QNetworkRequest &request);

public slots:
    void syncRequestSlot(QUuid netReqId, QByteArray payload, QList<QPair<QByteArray, QByteArray>> headers, int statusOrHttpCode);

signals:
    void syncGet(QUuid netReqId, QNetworkRequest request); // signal emitted to launch the request
    void syncAbort(QUuid netReqId); // signal emitted to abort connection

private:
    QMap<QUuid, requestParams> m_requestMap; // associate an uuid to the request loop and request response
    
    QString m_url; // url of the DICOMweb API to be set
    QTemporaryDir m_temporaryDir; // temporary directory in which files are stored
};


/**
 * @fn void dcmWeb::updateUrl(const QString &url)
 * @param url [in] url of the DICOMweb API mentioned in the plugin settings 
 * @brief set private url of the DICOMweb API from the one mentioned in the plugin settings 
*/

/**
 * @fn bool dcmWeb::hasUrl()
 * @brief check if the url is empty or not
*/

/**
 * @fn void dcmWeb::updateRequestState(const QUuid &netReqId)
 * @brief keep the request up to date by checking any new intercation with the server
 * @param netReqId [in] Uuid created to identify a request
*/

/**
 * @fn QUuid dcmWeb::waitGetResult(QNetworkRequest &request)
 * @brief allow to send a request to the server and wait for a reponse
 * @param netReqId [in] the request to be sent to the server 
 * @return an Uuid created to identify the request
*/

/**
 * @fn void dcmWeb::syncRequestSlot(QUuid netReqId, QByteArray payload, QList<QPair<QByteArray, QByteArray>> headers, int statusOrHttpCode)
 * @brief method called when the request is in progress or finished. Get the response elements as parameters
 * @param netReqId [in] Uuid associated to the request
 * @param payload [in] includes the body of the response, i.e. the instances of a series
 * @param headers [in] includes the main header of the response, i.e. information on the body
 * @param statusOrHttpCode [in] contains the status code for any error returned by the reponse, 200 if success
*/

/**
 * @fn int dcmWeb::isServerAvailable()
 * @brief try to establish a connection with the remote network and verify that there is no error in the reply
*/

/**
 * @fn QList<QMap<DcmTagKey, QString>> dcmWeb::patientSearchService(const QMap<DcmTagKey, QString> &tags)
 * @brief send a signal to execute QIDO-RS request : construct the request url, possibly with DICOM tag keys and their values in the url query part, to search for patients
 * @details convert the reply into a QJsonDocument to read and retrieve patient level tag values
 * @param tags [in] contains the DICOM tag keys whose values must be filled by the request
 * @return a QList for each patient found that matches the query through the values of their DICOM tags, contained in a QMap
*/

/**
 * @fn QList<QMap<DcmTagKey, QString>> dcmWeb::studySearchService(const QMap<DcmTagKey, QString> &tags)
 * @brief send a signal to execute QIDO-RS request (SearchForStudies) : construct the request url, possibly with DICOM tag keys and their values in the url query part, to search for studies
 * @details convert the reply into a QJsonDocument to read and retrieve study level tag values
 * @param tags [in] contains the DICOM tag keys whose values must be filled by the request
 * @return a QList for each study found that matches the query through the values of their DICOM tags, contained in a QMap
*/

/**
 * @fn QList<QMap<DcmTagKey, QString>> dcmWeb::seriesSearchService(const QMap<DcmTagKey, QString> &tags)
 * @brief send a signal to execute QIDO-RS request (SearchForSeries) : construct the request url, possibly with DICOM tag keys and their values in the url query part, to search for series
 * @details convert the reply into a QJsonDocument to read and retrieve series level tag values
 * @param tags [in] contains the DICOM tag keys whose values must be filled by the request
 * @return a QList for each series found that matches the query through the values of their DICOM tags, contained in a QMap
*/

#endif //DCMWEB_H
