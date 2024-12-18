/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "dcmRetrieveData.h"
#include "dcmWeb.h"

#include <medNotif.h>
#include <medNewLogger.h>

#include <dcmtk/dcmdata/dcdeftag.h>

dcmRetrieveData::dcmRetrieveData(QObject *parent, RequestManager *rqManager, dcmWeb *dicomWeb) : QObject(parent), m_dicomWeb(dicomWeb)
{
    QObject::connect(this, &dcmRetrieveData::asyncGet, rqManager, &RequestManager::httpGet, Qt::ConnectionType::QueuedConnection);
    QObject::connect(this, &dcmRetrieveData::asyncAbort, rqManager, &RequestManager::abort, Qt::ConnectionType::QueuedConnection);
}

dcmRetrieveData::~dcmRetrieveData()
{
}

void dcmRetrieveData::updateUrl(const QString &url)
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




bool dcmRetrieveData::retrieveDataService(int pi_requestId, unsigned int pi_uiLevel, const QString &uid)
{
    bool bRes = false;
    
    // set request url according to the level from which the operation is performed
    QString requestUrl;
    if(pi_uiLevel == 1)
    { // move at level study
        requestUrl = m_url + "studies/" + uid + "/";
    }
    else if(pi_uiLevel == 2)
    { // move at level series
        QMap<DcmTagKey, QString> tags;
        tags[DCM_SeriesInstanceUID] = uid;
        tags[DCM_StudyInstanceUID] = "";

        // retrieve the value of DCM_StudyInstanceUID key to complete the request url
        QList<QMap<DcmTagKey, QString>> seriesTags = m_dicomWeb->seriesSearchService(tags);

        QString studyUid = seriesTags.first().value(DCM_StudyInstanceUID);

        requestUrl = m_url + "studies/" + studyUid + "/series/" + uid + "/";
    }

    // send request
    QUuid netReqId = QUuid::createUuid();

    m_requestIdMap[netReqId].m_requestId = pi_requestId;

    QNetworkRequest request;
    request.setUrl(QUrl(requestUrl));

    emit asyncGet(netReqId, request);

    bRes = true;

    return bRes;
}

void dcmRetrieveData::asyncGetSlot(QUuid netReqId, QByteArray payload, QList<QPair<QByteArray, QByteArray>> headers, int statusOrHttpCode)
{
    if(m_requestIdMap.contains(netReqId))
    {
        // retrieve response elements
        m_requestIdMap[netReqId].response = {statusOrHttpCode, payload, headers};
        getData(netReqId);
    }
}

void dcmRetrieveData::getData(QUuid netReqId)
{
    if(m_requestIdMap[netReqId].response.code == DOWNLOAD_CODE)
    {
        emit inProgress(m_requestIdMap[netReqId].m_requestId, medAbstractSource::eRequestStatus::pending);
    }
    else if(m_requestIdMap[netReqId].response.code == SUCCESS_CODE)
    {
        QByteArray payload = m_requestIdMap[netReqId].response.payload;
        QList<QPair<QByteArray, QByteArray>> headers = m_requestIdMap[netReqId].response.headers;

        // analyze response main header and data to extract each part (instance) of the response data
        httpResponse httpResp(payload, headers);

        QString dirPath;
        int i = 1;
        if(m_temporaryDir.isValid())
        {
            // create a temporary directory 
            QDir dir(m_temporaryDir.path());
            dir.mkdir(QString::number(m_requestIdMap[netReqId].m_requestId));

            // create a file for each instance and saves it in the temporary directory
            for(auto &part : httpResp.getParts())
            {
                dirPath = dir.filePath(QString::number(m_requestIdMap[netReqId].m_requestId));
                QString filePath = dir.filePath(QString::number(m_requestIdMap[netReqId].m_requestId) + "/tmpData" + QString::number(i));

                QFile fileHeader(filePath);
                fileHeader.open(QIODevice::WriteOnly);
                fileHeader.write(part.payload);
                fileHeader.close();

                i++;
            }

            // store path to the files and send finish status
            mInfo << "Path where DICOM files had to be downloaded : " << dirPath;
            m_requestIdMap[netReqId].m_path = dirPath;
            emit inProgress(m_requestIdMap[netReqId].m_requestId, medAbstractSource::eRequestStatus::finish, dirPath);
        }
        
        m_requestIdMap.remove(netReqId);
    }
    else
    {
        medNotif::createNotif(notifLevel::error, "WADO-RS (Retrieve Service) request", "DICOM object retrieval error");
        emit inProgress(m_requestIdMap[netReqId].m_requestId, medAbstractSource::eRequestStatus::faild);
        m_requestIdMap.remove(netReqId);
    }

}




bool dcmRetrieveData::isCachedDataPath(int requestId)
{
    bool bRes = false;

    // search for the query stored with the same requestId as the one passed in parameters
    for(auto it = m_requestIdMap.begin(); it != m_requestIdMap.end(); ++it)
    {
        if(it.value().m_requestId == requestId)
        {
            bRes = true;
            // send a finish status if such a requestId exists
            emit inProgress(requestId, medAbstractSource::eRequestStatus::finish, it.value().m_path);
        }
    }

    return bRes;
}

void dcmRetrieveData::abortDownload(int pi_requestId)
{
    // search for the query stored with the same requestId as the one passed in parameters
    for(auto it = m_requestIdMap.begin(); it != m_requestIdMap.end(); ++it)
    {
        if(it.value().m_requestId == pi_requestId)
        {
            // abort connection represented by the netReqId Uuid, by emitting a signal, if such a requestId exists
            QUuid netReqId = it.key();
            emit asyncAbort(netReqId);
            return;
        }
    }
}
