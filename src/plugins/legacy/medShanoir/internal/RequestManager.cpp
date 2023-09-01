#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QJsonObject>

#include <JsonHelper.h>
#include <QTimer>
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
	handleReply(HTTP_GET_VERB, reply, netReqId);
}

void RequestManager::httpPost(QUuid netReqId, QNetworkRequest req, QByteArray data)
{
	QNetworkReply *reply = m_qnam->post(req, data);
	handleReply(HTTP_POST_VERB, reply, netReqId);
}

void RequestManager::httpPostMulti(QUuid netReqId, QNetworkRequest req, QHttpMultiPart *data)
{
	QNetworkReply *reply = m_qnam->post(req, data);
	handleReply(HTTP_POST_VERB, reply, netReqId);
}

void RequestManager::httpPut(QUuid netReqId, QNetworkRequest req, QByteArray data)
{
	QNetworkReply *reply = m_qnam->put(req, data); 
	handleReply(HTTP_PUT_VERB, reply, netReqId);
}

void RequestManager::handleReply(int httpVerb, QNetworkReply * &reply, const QUuid &netReqId)
{

	m_replyUuidMap[httpVerb][reply] = netReqId;

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
		if (m_replyUuidMap[HTTP_GET_VERB].contains(reply))
		{
			currentResponse(reply, HTTP_GET_VERB, UPLOAD_CODE, bytesSent, bytesTotal);
		}
		else if (m_replyUuidMap[HTTP_POST_VERB].contains(reply))
		{
			currentResponse(reply, HTTP_POST_VERB, UPLOAD_CODE, bytesSent, bytesTotal);
		}
		else if (m_replyUuidMap[HTTP_PUT_VERB].contains(reply))
		{
			currentResponse(reply, HTTP_PUT_VERB, UPLOAD_CODE, bytesSent, bytesTotal);
		}
	}
}

void RequestManager::slotDownloadProgress(qint64 bytesSent, qint64 bytesTotal)
{
	QNetworkReply *reply = dynamic_cast<QNetworkReply*>(QObject::sender());
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
	QByteArray payload = QByteArray();
	QJsonObject headers;
	headers.insert("bytesSent", bytesSent);
	headers.insert("bytesTotal", bytesTotal);

	switch (httpVerb)
	{
	case HTTP_GET_VERB:  emit responseHttpGet(m_replyUuidMap[httpVerb][reply], payload, headers, status);  break;
	case HTTP_POST_VERB: emit responseHttpPost(m_replyUuidMap[httpVerb][reply], payload, headers, status); break;
	case HTTP_PUT_VERB:  emit responseHttpPut(m_replyUuidMap[httpVerb][reply], payload, headers, status);  break;
	default:
		break;
	}
}

void RequestManager::abort(QUuid idRequest)
{
	for (int i = 0; i < 3; ++i) {
        for (auto it = m_replyUuidMap[i].begin(); it != m_replyUuidMap[i].end(); ++it) {
            if (it.value() == idRequest) {
                QNetworkReply* reply = it.key();
                reply->abort();
				m_replyUuidMap[i].remove(reply);
				return;
            }
        }
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
	QNetworkReply *reply = dynamic_cast<QNetworkReply*>(QObject::sender());
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
	auto headers = qByteArrayPairListToJsonObject(reply->rawHeaderPairs());

	switch (httpVerb)
	{
	case HTTP_GET_VERB:  emit responseHttpGet(m_replyUuidMap[httpVerb][reply], payload, headers, statusOrHttpCode); m_replyUuidMap[httpVerb].remove(reply); break;
	case HTTP_POST_VERB: emit responseHttpPost(m_replyUuidMap[httpVerb][reply], payload, headers, statusOrHttpCode); m_replyUuidMap[httpVerb].remove(reply); break;
	case HTTP_PUT_VERB:  emit responseHttpPut(m_replyUuidMap[httpVerb][reply], payload, headers, statusOrHttpCode); m_replyUuidMap[httpVerb].remove(reply); break;
	default:
		break;
	}
	// Deletion of  the reply object
	m_replyUuidMap->remove(reply);
	reply->deleteLater();
}
