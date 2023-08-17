#pragma once

#include <QString>
#include <QMap>
#include <QtNetwork/QNetworkAccessManager>
#include <QObject>

class Network : public QObject
{
	Q_OBJECT

private: 
	QNetworkAccessManager m_manager;

	QNetworkRequest initRequest(QString url, const QMap<QString, QString> & headers);

public:

	static QJsonObject replyContentObject(QNetworkReply *reply);
	static QJsonArray replyContentArray(QNetworkReply *reply);

	static QJsonObject replyHeaders(QNetworkReply *reply);

	Network();

	QNetworkReply * httpGetFetch(QString url, const QMap<QString, QString>& headers= QMap<QString, QString>());

	QNetworkReply * httpPostFetch(QString url, const QMap<QString, QString>& headers = QMap<QString, QString>(), const QMap<QString, QString>& formData= QMap<QString, QString>());

	QNetworkReply * httpPostFileSender(QString url, const QMap<QString, QString>& headers, QString filepath);

	QNetworkReply * httpPostFormDataSender(QString url, const QJsonObject& header, const QJsonObject &formData);

};