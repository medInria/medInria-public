#ifndef REQUEST_MANAGER
#define REQUEST_MANAGER

#include <QObject>

#include <QUuid>

#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>

// Special codes for the requests status
#define SUCCESS_CODE 200
#define UPLOAD_CODE 1001
#define DOWNLOAD_CODE 1002

// Ids of the http verbs used in the request manager
#define HTTP_GET_VERB  0
#define HTTP_POST_VERB 1
#define HTTP_PUT_VERB  2

/**
 * @struct RequestResponse
 * @brief represents the response of a request. This response can be in progress or finished.
*/
struct RequestResponse
{
	int code; // whether UPLOAD_CODE, DOWNLOAD_CODE SUCCESS_CODE, or a QNetworkReply::NetworkError code
 	QJsonObject headers;
	QByteArray payload; 
};

/**
 * @class RequestManager
 * @brief class for managing http requests. It is used by all the class that need to send one. Each request is identified by a QUuid, that is generated outside the class.
 * @details This class needed to be made, for isolating the QNetworkAccessManager in order to have one instance of it for the whole plugin. All the exchanges are made with signals and slots, permitting be asynchronous and thread safe.
*/
class RequestManager : public QObject
{
	Q_OBJECT

public:
//////////////////////////
////  Constructor/Destructor
//////////////////////////

    RequestManager(QObject * parent);
    
	~RequestManager();

public slots:
//////////////////////////
////  slots received from the different classes that need to send a request
////////////////////////// 

	void httpGet(QUuid netReqId, QNetworkRequest req);
	void httpPost(QUuid netReqId, QNetworkRequest req, QByteArray data);
	void httpPostMulti(QUuid netReqId, QNetworkRequest req, QHttpMultiPart *data);
	void httpPut(QUuid netReqId, QNetworkRequest req, QByteArray data);
	void abort(QUuid idRequest);
    
signals:
//////////////////////////
////  signals emitted for informing the different classes about their request status
//////////////////////////

	void responseHttpGet (QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);
	void responseHttpPost(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);
	void responseHttpPut (QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);

private slots:
//////////////////////////
////  slots received from the QNetworkReply objects
////////////////////////// 

	void slotUploadProgress  (qint64 progess, qint64 total);
	void slotDownloadProgress(qint64 progess, qint64 total);
	void slotFinished();
	void slotError(QNetworkReply::NetworkError);

private:

	/**
	 * @brief connects the reply objects that are created just after the request is sent to the request manager and adds them to the replyUuidMap.
	*/
	void handleReply(int httpVerb, QNetworkReply * &reply, const QUuid &netReqId);

	/**
	 * @brief handle the last response of a request. It can be an error or a finished request.
	*/
	void lastResponse(QNetworkReply * reply, int httpVerb, int statusOrHttpCode);

	/**
	 * @brief handle the current response of a request. It can be an upload or a download progression.
	*/
	void currentResponse(QNetworkReply *reply, int httpVerb, int status, qint64 bytesSent, qint64 bytesTotal);

private:
	QNetworkAccessManager * m_qnam;                 // the network access manager used to send the requests
	QMap<QNetworkReply*, QUuid>   m_replyUuidMap[3]; // map that links the reply objects to the QUuid of the request. It is on three dimensions because there are currently three http verbs: GET, POST and PUT.
};
#endif
