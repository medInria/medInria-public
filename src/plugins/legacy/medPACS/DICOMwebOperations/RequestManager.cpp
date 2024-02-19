/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "RequestManager.h"

RequestManager::RequestManager(QObject * parent):QObject(parent)
{
	// initializing the network access manager that will be used by the plugin instance
	m_qnam = new QNetworkAccessManager(this);
}

RequestManager::~RequestManager()
{

}

void RequestManager::httpGet(QUuid netReqId, QNetworkRequest req)
{
	// sending the get request
	QNetworkReply *reply = m_qnam->get(req);   
	handleReply(HTTP_GET_VERB, reply, netReqId);
}

void RequestManager::handleReply(int httpVerb, QNetworkReply * &reply, const QUuid &netReqId)
{
	// storing the reply object in the reply map
	m_replyUuidMap[httpVerb][reply] = netReqId;

	// Connecting the request manager slots to the signals sent by the reply object
	QObject::connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(slotDownloadProgress(qint64, qint64)));
	QObject::connect(reply, SIGNAL(finished()), this, SLOT(slotFinished()));
	QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
}

void RequestManager::slotDownloadProgress(qint64 bytesSent, qint64 bytesTotal)
{
	// retrieving the reply object who sent a download signal
	QNetworkReply *reply = dynamic_cast<QNetworkReply*>(QObject::sender());

	//handling the reply progression information
	if (reply)
	{
		if (m_replyUuidMap[HTTP_GET_VERB].contains(reply))
		{
			currentResponse(reply, HTTP_GET_VERB, DOWNLOAD_CODE, bytesSent, bytesTotal);
		}
		else if (m_replyUuidMap[HTTP_POST_VERB].contains(reply))
		{
			currentResponse(reply, HTTP_POST_VERB, DOWNLOAD_CODE, bytesSent, bytesTotal);
		}
		else if (m_replyUuidMap[HTTP_PUT_VERB].contains(reply))
		{
			currentResponse(reply, HTTP_PUT_VERB, DOWNLOAD_CODE, bytesSent, bytesTotal);
		}
	}
}

void RequestManager::currentResponse(QNetworkReply *reply, int httpVerb, int status, qint64 bytesSent, qint64 bytesTotal)
{
	// the response is in progress, therefore, the payload is empty. 
	QByteArray payload = QByteArray();
	// storing the bytes information of the reply in the header
	QList<QPair<QByteArray, QByteArray>> headers;

	// sending the signal for the network classes to handle the reply information 
	switch (httpVerb)
	{
        case HTTP_GET_VERB:  
            emit responseHttpGet(m_replyUuidMap[httpVerb][reply], payload, headers, status);  
            break;
        case HTTP_POST_VERB: 
            emit responseHttpPost(m_replyUuidMap[httpVerb][reply], payload, headers, status); 
            break;
        case HTTP_PUT_VERB:  
            emit responseHttpPut(m_replyUuidMap[httpVerb][reply], payload, headers, status);  
            break;
        default:
            break;
	}
}

void RequestManager::slotFinished()
{
	// retrieving the reply object who sent a finish signal
	QNetworkReply *reply = dynamic_cast<QNetworkReply*>(QObject::sender());

	//handling the reply finish information
	if (reply)
	{
		auto httpCode =	reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		if (m_replyUuidMap[HTTP_GET_VERB].contains(reply))
		{
			lastResponse(reply, HTTP_GET_VERB, httpCode);
		}
		else if (m_replyUuidMap[HTTP_POST_VERB].contains(reply))
		{
			lastResponse(reply, HTTP_POST_VERB, httpCode);
		}
		else if (m_replyUuidMap[HTTP_PUT_VERB].contains(reply))
		{
			lastResponse(reply, HTTP_PUT_VERB, httpCode);
		}
	}
}

void RequestManager::slotError(QNetworkReply::NetworkError err)
{
	// retrieving the reply object who sent an error signal
	QNetworkReply *reply = dynamic_cast<QNetworkReply*>(QObject::sender());
	//handling the reply error information
	if (reply)
	{
		int httpCode = static_cast<int>(reply->error());
		if (m_replyUuidMap[HTTP_GET_VERB].contains(reply))
		{
			lastResponse(reply, HTTP_GET_VERB, httpCode);
		}
		else if (m_replyUuidMap[HTTP_POST_VERB].contains(reply))
		{
			lastResponse(reply, HTTP_POST_VERB, httpCode);
		}
		else if (m_replyUuidMap[HTTP_PUT_VERB].contains(reply))
		{
			lastResponse(reply, HTTP_PUT_VERB, httpCode);
		}
	}
}

void RequestManager::lastResponse(QNetworkReply * reply, int httpVerb, int statusOrHttpCode)
{
	auto payload = reply->readAll();
	auto headers = reply->rawHeaderPairs();

	// sending the signal for the network classes to handle the reply information 
	switch (httpVerb)
	{
        case HTTP_GET_VERB:  
            emit responseHttpGet(m_replyUuidMap[httpVerb][reply], payload, headers, statusOrHttpCode); 
            m_replyUuidMap[httpVerb].remove(reply); 
            break;
        case HTTP_POST_VERB: 
            emit responseHttpPost(m_replyUuidMap[httpVerb][reply], payload, headers, statusOrHttpCode); 
            m_replyUuidMap[httpVerb].remove(reply); 
            break;
        case HTTP_PUT_VERB:  
            emit responseHttpPut(m_replyUuidMap[httpVerb][reply], payload, headers, statusOrHttpCode); 
            m_replyUuidMap[httpVerb].remove(reply); 
            break;
        default:
            break;
	}
	
	// Deletion of the reply object
	m_replyUuidMap->remove(reply);
	reply->deleteLater();
}


void RequestManager::abort(QUuid idRequest)
{
	// retrieving the reply object with the idRequest  
	for(int i = 0; i < 3; ++i)
    {
        for (auto it = m_replyUuidMap[i].begin(); it != m_replyUuidMap[i].end(); ++it)
        {
            if (it.value() == idRequest)
            {
                QNetworkReply* reply = it.key();
                reply->abort();
				m_replyUuidMap[i].remove(reply);
				return;
            }
        }
    }
}