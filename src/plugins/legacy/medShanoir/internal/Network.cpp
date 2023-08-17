#include <QCoreApplication>
#include <QtNetwork/QNetworkReply>
#include <QEventLoop>
#include <QDebug>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <QThread>
#include <QJsonParseError>
#include <QFile>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QFileInfo>

#include "JsonReaderWriter.h"
#include "Network.h"

QJsonObject Network::replyContentObject(QNetworkReply * reply)
{
	return JsonReaderWriter::qbytearrayToQJson(reply->readAll());
}

QJsonArray Network::replyContentArray(QNetworkReply * reply)
{
	return JsonReaderWriter::qbytearrayToQJsonArray(reply->readAll());
}

QJsonObject Network::replyHeaders(QNetworkReply * reply)
{
	return JsonReaderWriter::qByteArrayPairListToJsonObject(reply->rawHeaderPairs());
}

Network::Network()
{
}

QNetworkRequest Network::initRequest(QString url, const QMap<QString, QString> & headers) {
	QNetworkRequest request;
	request.setUrl(QUrl(url));
	for (const QString& key : headers.keys()) {
		QString value = headers.value(key);
		request.setRawHeader(key.toUtf8(), value.toUtf8());
	}
	return request;
}

QNetworkReply * Network::httpGetFetch(QString url, const QMap<QString, QString>& headers)
{
	//qDebug().noquote()<<"[GET] " << url;
	QNetworkRequest request=initRequest(url,headers) ;
	
	// Sending the GET request
	QNetworkReply *reply = m_manager.get(request);

	// Waiting for the request to finish
	QEventLoop loop;
	QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	loop.exec();
	reply->deleteLater();

	return reply;
}

QNetworkReply * Network::httpPostFetch(QString url, const QMap<QString, QString>& headers, const QMap<QString, QString>& data)
{
	//qDebug().noquote()<< "[POST] " << url;
	QNetworkRequest request = initRequest(url, headers);

	QUrlQuery postData;
	for (const QString& key : data.keys()) {
		QString value = data.value(key);
		postData.addQueryItem(key, value);
	}

	// Sending the POST request 
	QNetworkReply *reply = m_manager.post(request, postData.query().toUtf8());

	// Waiting for the request to finish
	QEventLoop loop;
	QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	loop.exec();
	reply->deleteLater();

	return reply;
}

QNetworkReply * Network::httpPostFileSender(QString url, const QMap<QString, QString> & headers, const QString filepath)
{
	QNetworkRequest request = initRequest(url, headers);

	QFile *file = new QFile(filepath);
	file->open(QIODevice::ReadOnly);
	QFileInfo fileInfo(filepath);

	QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	QHttpPart imagePart;
	imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(R"(form-data; name="image"; filename=")" + fileInfo.fileName() + R"(")"));
	imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-gzip"));
	imagePart.setBodyDevice(file);
	file->setParent(multiPart); // the multiPart will delete the file later (no need to delete the file manually)
	multiPart->append(imagePart);


	QNetworkReply *reply = m_manager.post(request, multiPart);

	QEventLoop loop;
	QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	loop.exec();

	reply->deleteLater();
	multiPart->deleteLater();
	return reply;
}


QNetworkReply * Network::httpPostFormDataSender(QString url, const QJsonObject& header, const QJsonObject &formData)
{
	QUrl urlObject(url);
	QNetworkRequest request(urlObject);
	
	for(const QString& key : header.keys())
	{
		QString value = header.value(key).toString();
		request.setRawHeader(key.toUtf8(), value.toUtf8());
	}

	QJsonDocument bodyDocument(formData);
	QByteArray bodyData = bodyDocument.toJson();

	
	QNetworkReply* reply = m_manager.post(request, bodyData);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, [&]() {
        loop.quit();
    });

    loop.exec();

	reply->deleteLater();

	return reply;
}