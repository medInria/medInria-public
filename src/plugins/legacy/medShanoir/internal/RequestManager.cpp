#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QJsonObject>

#include <JsonHelper.h>
#include "RequestManager.h"



RequestManager::RequestManager(QObject * parent):QObject(parent)
{
	m_qnam = new QNetworkAccessManager(this);
}

RequestManager::~RequestManager()
{

}

void RequestManager::httpGet(QUuid netReqId, QNetworkRequest req)
{
	QNetworkReply *reply = m_qnam->get(req);
	handleReply(HTTTP_GET_VERBE, reply, netReqId);
}

void RequestManager::httpPost(QUuid netReqId, QNetworkRequest req, QByteArray data)
{
	QNetworkReply *reply = m_qnam->post(req, data);
	handleReply(HTTTP_POST_VERBE, reply, netReqId);
}

void RequestManager::httpPostMulti(QUuid netReqId, QNetworkRequest req, QHttpMultiPart *data)
{
	QNetworkReply *reply = m_qnam->post(req, data);
	handleReply(HTTTP_POST_VERBE, reply, netReqId);
}

void RequestManager::httpPut(QUuid netReqId, QNetworkRequest req, QByteArray data)
{
	QNetworkReply *reply = m_qnam->put(req, data); 
	handleReply(HTTTP_PUT_VERBE, reply, netReqId);
}

void RequestManager::handleReply(int httpVerbe, QNetworkReply * &reply, const QUuid &netReqId)
{

	m_replyUuidMap[httpVerbe][reply] = netReqId;

	QObject::connect(reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(slotUploadProgress(qint64, qint64)));
	QObject::connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(slotDownloadProgress(qint64, qint64)));
	QObject::connect(reply, SIGNAL(finished()), this, SLOT(slotFinished()));
	QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
}



//
// THE REQUEST IS PENDING 
//

void RequestManager::slotUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
	QNetworkReply *reply = dynamic_cast<QNetworkReply*>(QObject::sender());
	if (reply)
	{
		if (m_replyUuidMap[HTTTP_GET_VERBE].contains(reply))
		{
			currentResponse(reply, HTTTP_GET_VERBE, 1, bytesSent, bytesTotal);
		}
		else if (m_replyUuidMap[HTTTP_POST_VERBE].contains(reply))
		{
			currentResponse(reply, HTTTP_POST_VERBE, 1, bytesSent, bytesTotal);
		}
		else if (m_replyUuidMap[HTTTP_PUT_VERBE].contains(reply))
		{
			currentResponse(reply, HTTTP_PUT_VERBE, 1, bytesSent, bytesTotal);
		}
	}
}

void RequestManager::slotDownloadProgress(qint64 bytesSent, qint64 bytesTotal)
{
	QNetworkReply *reply = dynamic_cast<QNetworkReply*>(QObject::sender());
	if (reply)
	{
		if (m_replyUuidMap[HTTTP_GET_VERBE].contains(reply))
		{
			currentResponse(reply, HTTTP_GET_VERBE, 2, bytesSent, bytesTotal);
		}
		else if (m_replyUuidMap[HTTTP_POST_VERBE].contains(reply))
		{
			currentResponse(reply, HTTTP_POST_VERBE, 2, bytesSent, bytesTotal);
		}
		else if (m_replyUuidMap[HTTTP_PUT_VERBE].contains(reply))
		{
			currentResponse(reply, HTTTP_PUT_VERBE, 2, bytesSent, bytesTotal);
		}
	}
}

void RequestManager::currentResponse(QNetworkReply *reply, int httpVerbe, int status, qint64 bytesSent, qint64 bytesTotal)
{
	QByteArray payload = QByteArray();
	QJsonObject headers;
	headers.insert("bytesSent", bytesSent);
	headers.insert("bytesTotal", bytesTotal);

	switch (httpVerbe)
	{
	case HTTTP_GET_VERBE:  emit responseHttpGet(m_replyUuidMap[httpVerbe][reply], payload, headers, status);  break;
	case HTTTP_POST_VERBE: emit responseHttpPost(m_replyUuidMap[httpVerbe][reply], payload, headers, status); break;
	case HTTTP_PUT_VERBE:  emit responseHttpPut(m_replyUuidMap[httpVerbe][reply], payload, headers, status);  break;
	default:
		break;
	}
}



//
// THE REQUEST FINISHED
//
void RequestManager::slotFinished()
{
	QNetworkReply *reply = dynamic_cast<QNetworkReply*>(QObject::sender());
	if (reply)
	{
		auto httpCode =	reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		if (m_replyUuidMap[HTTTP_GET_VERBE].contains(reply))
		{
			lastResponse(reply, HTTTP_GET_VERBE, httpCode);
		}
		else if (m_replyUuidMap[HTTTP_POST_VERBE].contains(reply))
		{
			lastResponse(reply, HTTTP_POST_VERBE, httpCode);
		}
		else if (m_replyUuidMap[HTTTP_PUT_VERBE].contains(reply))
		{
			lastResponse(reply, HTTTP_PUT_VERBE, httpCode);
		}
	}
}

void RequestManager::slotError(QNetworkReply::NetworkError err)
{
	QNetworkReply *reply = dynamic_cast<QNetworkReply*>(QObject::sender());
	if (reply)
	{
		auto httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		if (m_replyUuidMap[HTTTP_GET_VERBE].contains(reply))
		{
			lastResponse(reply, HTTTP_GET_VERBE, httpCode);
		}
		else if (m_replyUuidMap[HTTTP_POST_VERBE].contains(reply))
		{
			lastResponse(reply, HTTTP_POST_VERBE, httpCode);
		}
		else if (m_replyUuidMap[HTTTP_PUT_VERBE].contains(reply))
		{
			lastResponse(reply, HTTTP_PUT_VERBE, httpCode);
		}
	}
}

void RequestManager::lastResponse(QNetworkReply * reply, int httpVerbe, int statusOrHttpCode)
{
	auto payload = reply->readAll();
	auto headers = qByteArrayPairListToJsonObject(reply->rawHeaderPairs());

	switch (httpVerbe)
	{
	case HTTTP_GET_VERBE:  emit responseHttpGet(m_replyUuidMap[httpVerbe][reply], payload, headers, statusOrHttpCode); m_replyUuidMap[httpVerbe].remove(reply); break;
	case HTTTP_POST_VERBE: emit responseHttpPost(m_replyUuidMap[httpVerbe][reply], payload, headers, statusOrHttpCode); m_replyUuidMap[httpVerbe].remove(reply); break;
	case HTTTP_PUT_VERBE:  emit responseHttpPut(m_replyUuidMap[httpVerbe][reply], payload, headers, statusOrHttpCode); m_replyUuidMap[httpVerbe].remove(reply); break;
	default:
		break;
	}
	// Deletion of  the reply object
	m_replyUuidMap->remove(reply);
	reply->deleteLater();
}
