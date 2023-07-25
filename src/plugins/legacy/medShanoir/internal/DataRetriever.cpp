#include <QNetworkReply>

#include <JsonReaderWriter.h>
#include "DataRetriever.h"

void DataRetriever::loadFile(int dataset_id, QString query_string, QByteArray & fileData, QString & filename)
{
	QString url = m_auth.getBaseURL() + "datasets/datasets/download/" + QString::number(dataset_id) + "?" + query_string;
	QNetworkReply *reply = m_net.httpGetFetch(url, { {"Authorization", "Bearer " + m_auth.getCurrentAccessToken()} });
	QJsonObject responseHeaders = m_net.replyHeaders(reply);
	if (JsonReaderWriter::verifyKeys(responseHeaders, { "Content-Disposition" }))
	{
		filename = responseHeaders.value("Content-Disposition").toString().split("filename=")[1].split(";").first();
		if (filename.length() > 0)
		{
			fileData = reply->readAll();
		}
	}
}

DataRetriever::DataRetriever(int id, Authenticater & auth, QString storagePath, QObject *parent) : QObject(parent), m_id(id), m_auth(auth), m_storagePath(storagePath), m_dataPath("INVALID_PATH")
{
}

QString DataRetriever::getDataPath()
{
	return m_dataPath;
}

int DataRetriever::getId()
{
	return m_id;
}
