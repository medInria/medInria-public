#ifndef REQUEST_MANAGER
#define REQUEST_MANAGER

#include <QObject>

#include <QUuid>

#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
// #include <QUrlQuery>
#include <QByteArray>


#define HTTTP_GET_VERBE  0
#define HTTTP_POST_VERBE 1
#define HTTTP_PUT_VERBE  2

struct RequestResponse
{
	int code;
	QJsonObject headers;
	QByteArray payload;
};

class RequestManager : public QObject
{
	Q_OBJECT

public:
    RequestManager(QObject * parent);
    ~RequestManager();

public slots:
	void httpGet(QUuid netReqId, QNetworkRequest req);
	void httpPost(QUuid netReqId, QNetworkRequest req, QByteArray data);
	void httpPostMulti(QUuid netReqId, QNetworkRequest req, QHttpMultiPart *data);
	void httpPut(QUuid netReqId, QNetworkRequest req, QByteArray data);
    
signals:
	//payload & headers are empty if the status is not an http code 
	void responseHttpGet (QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);
	void responseHttpPost(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);
	void responseHttpPut (QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);

private slots:
	void slotUploadProgress  (qint64 progess, qint64 total);
	void slotDownloadProgress(qint64 progess, qint64 total);
	void slotFinished();
	void slotError(QNetworkReply::NetworkError);

private:
	void handleReply(int httpVerbe, QNetworkReply * &reply, const QUuid &netReqId);
	void lastResponse(QNetworkReply * reply, int httpVerbe, int statusOrHttpCode);
	void currentResponse(QNetworkReply *reply, int httpVerbe, int status, qint64 bytesSent, qint64 bytesTotal);

private:
	QNetworkAccessManager * m_qnam;
	QMap<QNetworkReply*, QUuid>   m_replyUuidMap[3];
};
#endif
