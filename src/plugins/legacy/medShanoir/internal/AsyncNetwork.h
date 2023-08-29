#ifndef ASYNC_NETWORK
#define ASYNC_NETWORK

#include <QObject>
#include <QUuid>
#include <QVariant>
#include <QVector>

#include <medAbstractSource.h>
#include <RequestManager.h>

#define ASYNC_GET_DATA 0
#define ASYNC_ADD_DATA_CONTEXT 1
#define ASYNC_ADD_DATA_FILE 2

using levelMinimalEntries = medAbstractSource::levelMinimalEntries;
using datasetAttributes = medAbstractSource::datasetAttributes;
using eRequestStatus = medAbstractSource::eRequestStatus;

class Authenticator;
class medShanoir;
class LocalInfo;
class SyncNetwork;

class AsyncNetwork : public QObject
{
	Q_OBJECT
public:
	AsyncNetwork(medShanoir * parent, LocalInfo *info, Authenticator * authent, SyncNetwork *syncNet, RequestManager * requestMgr);
	~AsyncNetwork();


	/* ***********************************************************************/
	/* *************** Get data          *************************************/
	/* ***********************************************************************/
	int getAssyncData(unsigned int pi_uiLevel, QString key);

	/* ***********************************************************************/
	/* *************** Store data          ***********************************/
	/* ***********************************************************************/
	int addAssyncData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey);
	//int  push(unsigned int pi_uiLevel, QString key);

	/* ***********************************************************************/
	/* *************** Handle async results   ********************************/
	/* ***********************************************************************/
	QVariant getAsyncResults(int pi_iRequest);

public slots:
	void asyncGetSlot (QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);
	void asyncPostSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);
	void asyncPutSlot (QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);

signals:
	void asyncGet(QUuid, QNetworkRequest);
	void asyncPost(QUuid, QNetworkRequest, QByteArray);
	void asyncPostMulti(QUuid, QNetworkRequest, QHttpMultiPart *);
	void asyncPut(QUuid, QNetworkRequest, QByteArray);

private:
	void dataToFile(int medId);
	void cleaner(bool all = false); //Clean filesystem and m_idResultMap|m_filesToRemove

	int sendProcessedDataset(QString &filepath, levelMinimalEntries & pio_minimalEntries, QStringList& parts);
	void sendProcessedDatasetContext(int medId);
	void sentDatasetInterpretation(QUuid netReqId);

private:
	medShanoir            * m_parent;
	LocalInfo             * m_info;
	Authenticator         * m_authent;
	SyncNetwork           * m_syncNet;
	QAtomicInt              m_medReqId;

	QMap<QUuid, RequestResponse>     m_requestResultMap[3];
	QMap<QUuid, int>                 m_requestIdMap;
	QMap<int, QVariant>              m_idResultMap;
	
	QVector < QPair<qint64, QString> >  m_filesToRemove;
};
#endif // !ASYNC_NETWORK