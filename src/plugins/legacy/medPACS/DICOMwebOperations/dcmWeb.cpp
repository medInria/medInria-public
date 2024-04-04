/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "dcmWeb.h"

#include <medNotif.h>

#include <dcmtk/dcmdata/dctag.h>
#include <dcmtk/dcmdata/dcdeftag.h>

dcmWeb::dcmWeb(QObject *parent, RequestManager *rqManager) : QObject(parent)
{
    // signal to send the request
    QObject::connect(this, &dcmWeb::syncGet, rqManager, &RequestManager::httpGet, Qt::ConnectionType::QueuedConnection);
    // signal to abort connection
    QObject::connect(this, &dcmWeb::syncAbort, rqManager, &RequestManager::abort, Qt::ConnectionType::QueuedConnection);
}

dcmWeb::~dcmWeb()
{
}

void dcmWeb::updateUrl(const QString &url)
{
    if(url.endsWith("/"))
    {
        m_url = url;
    }
    else
    {
        m_url = url + "/";
    }
}

bool dcmWeb::hasUrl()
{
    return m_url != QString();
}




void dcmWeb::updateRequestState(const QUuid &netReqId)
{
    // execution after 1s
    QTimer::singleShot(1000, [this, netReqId]()
    {
        if(m_requestMap.contains(netReqId))
        {
            if(m_requestMap[netReqId].upToDate)
            {
                m_requestMap[netReqId].upToDate = false;
                updateRequestState(netReqId);
            }
        }
        else if(m_requestMap[netReqId].response.code != SUCCESS_CODE)
        {
            emit syncAbort(netReqId);
        }
    });
}

QUuid dcmWeb::waitGetResult(QNetworkRequest &request)
{
    QUuid netReqId = QUuid::createUuid();

    // set the loop
    QEventLoop *waiter = new QEventLoop(nullptr);
    m_requestMap[netReqId].waiter = waiter;
    m_requestMap[netReqId].upToDate = false;

    // call request execution
    emit syncGet(netReqId, request);

    // follow request state
    updateRequestState(netReqId);

    // code for any error returned by the reponse 
    m_requestMap[netReqId].waiterCode = waiter->exec();
    waiter->deleteLater();

    return netReqId;
}

void dcmWeb::syncRequestSlot(QUuid netReqId, QByteArray payload, QList<QPair<QByteArray, QByteArray>> headers, int statusOrHttpCode)
{
    if(m_requestMap.contains(netReqId))
    {
        // retrieve response elements
        RequestResponse response = {statusOrHttpCode, payload, headers};
        m_requestMap[netReqId].response = response;
        
        if(statusOrHttpCode == DOWNLOAD_CODE)
        {
            m_requestMap[netReqId].upToDate = true;
        }
        else
        {
            m_requestMap[netReqId].waiter->exit();
        }
    }
}




int dcmWeb::isServerAvailable()
{
    int respCode = -1;

    QNetworkRequest request;
    request.setUrl(QUrl(m_url + "series"));
    QUuid netReqId = waitGetResult(request);

    if(m_requestMap[netReqId].response.code == SUCCESS_CODE)
    {
        respCode = m_requestMap[netReqId].waiterCode;
        medNotif::createNotif(notifLevel::success, "DICOMweb RESTful services", "Connected to " + m_url);
    }
    else
    {
        medNotif::createNotif(notifLevel::error, "DICOMweb RESTful services", "Unable to connect to " + m_url);
    }

    if(m_requestMap.contains(netReqId))
    {
        m_requestMap.remove(netReqId);
    }

    return respCode;
}


QList<QMap<DcmTagKey, QString>> dcmWeb::patientSearchService(const QMap<DcmTagKey, QString> &tags)
{
    QList<QMap<DcmTagKey, QString>> infos;

    // set request url
    QString requestUrl = m_url + "studies?";

    // add some tag values possibly according to filter parameters
    for(auto tagKey : tags.keys())
    {
        if(!tags.value(tagKey).isEmpty() && tagKey != DCM_QueryRetrieveLevel)
        {
            DcmTag tag(tagKey);
            requestUrl += tag.getTagName() + QString("=") + tags.value(tagKey) + QString("&");
        }
    }

    // send request
    QNetworkRequest request;
    request.setUrl(QUrl(requestUrl));
    QUuid netReqId = waitGetResult(request);

    // response processing
    if(m_requestMap[netReqId].response.code == SUCCESS_CODE)
    {
        QByteArray payload = m_requestMap[netReqId].response.payload;

        // read response body in json format
        QJsonDocument jsonDoc = QJsonDocument::fromJson(payload);
        QJsonArray jsonArray = jsonDoc.array();
        for(auto jsonValue : jsonArray)
        {
            QMap<DcmTagKey, QString> patientInfos;

            QJsonObject patientTags = jsonValue.toObject();

            // identify DcmTagKey of parameter 'tags' in the json : patient level keys
            for(auto tagKey : tags.keys())
            {
                if(tagKey != DCM_QueryRetrieveLevel)
                {
                    QString dcmKey = QString(tagKey.toString().c_str()).mid(1,9);
                    QString dcmUpperKey = dcmKey.remove(",").toUpper(); // toUpper() for tags with letters : (0008,103e)

                    // if defined in the json, retrieve their value
                    if(patientTags.contains(dcmUpperKey))
                    {
                        if(tagKey == DCM_PatientName)
                        {
                            QJsonObject patientNameValues = patientTags[dcmUpperKey].toObject();
                            QJsonValue patientNameValue = patientNameValues["Value"].toArray().first();
                            QString patientName = patientNameValue.toObject()["Alphabetic"].toString();
                            patientInfos.insert(DCM_PatientName, patientName);
                        }
                        else
                        {
                            QJsonObject jsonObj = patientTags[dcmUpperKey].toObject();
                            QString patientTagValue = jsonObj["Value"].toArray().first().toString();
                            patientInfos.insert(tagKey, patientTagValue);
                        }
                    }
                }
            }

            infos.append(patientInfos);
        }
    }
    else
    {
        medNotif::createNotif(notifLevel::error, "QIDO-RS (Query Service) request", "Error in DICOM objects search at patient level");
    }

    // once the request has been processed, deletes it from the QMap using the associated uuid
    if(m_requestMap.contains(netReqId))
    {
        m_requestMap.remove(netReqId);
    }

    return infos;
}


QList<QMap<DcmTagKey, QString>> dcmWeb::studySearchService(const QMap<DcmTagKey, QString> &tags)
{
    QList<QMap<DcmTagKey, QString>> infos;

    // set request url
    QString requestUrl = m_url + "studies?";

    // add some tag values from the parent level (patient) or possibly according to filter parameters
    for(auto tagKey : tags.keys())
    {
        if(!tags.value(tagKey).isEmpty() && tagKey != DCM_QueryRetrieveLevel)
        {
            DcmTag tag(tagKey);
            requestUrl += tag.getTagName() + QString("=") + tags.value(tagKey) + QString("&");
        }
    }
    
    // send request
    QNetworkRequest request;
    request.setUrl(QUrl(requestUrl));
    QUuid netReqId = waitGetResult(request);

    // response processing
    if(m_requestMap[netReqId].response.code == SUCCESS_CODE)
    {
        QByteArray payload = m_requestMap[netReqId].response.payload;

        // read response body in json format
        QJsonDocument jsonDoc = QJsonDocument::fromJson(payload);
        QJsonArray jsonArray = jsonDoc.array();
        for(auto jsonValue : jsonArray)
        {
            QMap<DcmTagKey, QString> studyInfos;

            QJsonObject studyTags = jsonValue.toObject();

            // identify DcmTagKey of parameter 'tags' in the json : study level keys
            for(auto tagKey : tags.keys())
            {
                if(tagKey != DCM_QueryRetrieveLevel)
                {
                    QString dcmKey = QString(tagKey.toString().c_str()).mid(1,9);
                    QString dcmUpperKey = dcmKey.remove(",").toUpper(); // toUpper() for tags with letters : (0008,103e)

                    // if defined in the json, retrieve their value
                    if(studyTags.contains(dcmUpperKey))
                    {
                        QJsonObject jsonObj = studyTags[dcmUpperKey].toObject();
                        QString studyTagValue = jsonObj["Value"].toArray().first().toString();
                        studyInfos.insert(tagKey, studyTagValue);
                    }
                }
            }

            infos.append(studyInfos);
        }
    }
    else
    {
        medNotif::createNotif(notifLevel::error, "QIDO-RS (Query Service) request", "Error in DICOM objects search at study level");
    }

    // once the request has been processed, deletes it from the QMap using the associated uuid
    if(m_requestMap.contains(netReqId))
    {
        m_requestMap.remove(netReqId);
    }

    return infos;
}

QList<QMap<DcmTagKey, QString>> dcmWeb::seriesSearchService(const QMap<DcmTagKey, QString> &tags)
{
    QList<QMap<DcmTagKey, QString>> infos;

    // set request url
    QString requestUrl;
    if(tags[DCM_StudyInstanceUID] != "")
    {
        requestUrl = m_url + "studies/" + tags[DCM_StudyInstanceUID] + "/series?";
    }
    else
    {
        requestUrl = m_url + "series?";
    }

    // add some tag values from the parent level (study) or possibly according to filter parameters
    for(auto tagKey : tags.keys())
    {
        if(!tags.value(tagKey).isEmpty() && tagKey != DCM_QueryRetrieveLevel && tagKey != DCM_StudyInstanceUID)
        {
            DcmTag tag(tagKey);
            requestUrl += tag.getTagName() + QString("=") + tags.value(tagKey) + QString("&");
        }
    }

    // send request
    QNetworkRequest request;
    request.setUrl(QUrl(requestUrl));
    QUuid netReqId = waitGetResult(request);

    // response processing
    if(m_requestMap[netReqId].response.code == SUCCESS_CODE)
    {
        QByteArray payload = m_requestMap[netReqId].response.payload;

        // read response body in json format
        QJsonDocument jsonDoc = QJsonDocument::fromJson(payload);
        QJsonArray jsonArray = jsonDoc.array();
        for(auto jsonValue : jsonArray)
        {
            QMap<DcmTagKey, QString> seriesInfos;

            QJsonObject seriesTags = jsonValue.toObject();

            // identify DcmTagKey of parameter 'tags' in the json : series level keys
            for(auto tagKey : tags.keys())
            {
                if(tagKey != DCM_QueryRetrieveLevel)
                {
                    QString dcmKey = QString(tagKey.toString().c_str()).mid(1,9);
                    QString dcmUpperKey = dcmKey.remove(",").toUpper(); // toUpper() for tags with letters : (0008,103e)

                    // if defined in the json, retrieve their value
                    if(seriesTags.contains(dcmUpperKey))
                    {
                        QJsonObject jsonObj = seriesTags[dcmUpperKey].toObject();
                        QString serieTagValue = jsonObj["Value"].toArray().first().toString();
                        seriesInfos.insert(tagKey, serieTagValue);
                    }
                }
            }

            infos.append(seriesInfos);
        }
    }
    else
    {
        medNotif::createNotif(notifLevel::error, "QIDO-RS (Query Service) request", "Error in DICOM objects search at series level");
    }

    // once the request has been processed, deletes it from the QMap using the associated uuid
    if(m_requestMap.contains(netReqId))
    {
        m_requestMap.remove(netReqId);
    }

    return infos;
}
