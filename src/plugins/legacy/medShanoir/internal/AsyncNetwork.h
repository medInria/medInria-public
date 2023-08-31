#ifndef ASYNC_NETWORK
#define ASYNC_NETWORK

#include <QObject>
#include <QUuid>
#include <QVariant>
#include <QVector>

#include <medAbstractSource.h>
#include <RequestManager.h>

// #define ASYNC_GET_DATA 0
// #define ASYNC_GET_DATA_AGAIN 1
// #define ASYNC_ADD_DATA_CONTEXT 2
// #define ASYNC_ADD_DATA_FILE 3

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

    void abort(int pi_iRequest); 

public slots:
	void asyncGetSlot (QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);
	void asyncPostSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);
	void asyncPutSlot (QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);
	
signals:
	void asyncGet(QUuid, QNetworkRequest);
	void asyncPost(QUuid, QNetworkRequest, QByteArray);
	void asyncPostMulti(QUuid, QNetworkRequest, QHttpMultiPart *);
	void asyncPut(QUuid, QNetworkRequest, QByteArray);
	void asyncAbort(QUuid);

private:

	// main interpretation methods 
	// --they are called from the slots
	void getAsyncDataInterpretation(QUuid netReqId, RequestResponse res);
	void addAsyncDataInterpretation(QUuid netReqId, RequestResponse res);

	// sub interpretation methods
	// -- they are called from the main interpretation methods
	// -- the given response is successful (the code worth SUCCESS_CODE)

	/**
	 * sends the dataset context if the file upload went well
	 * @param netReqId
	 * @param res 
	 * @return a QVariant containing whether a bool with the false value or a QUuid of the context upload request if the file upload went well
	*/
	QVariant sentDatasetFileInterpretation   (QUuid netReqId, RequestResponse res);
	void sentDatasetContextInterpretation(QUuid netReqId, RequestResponse res);

	
	QUuid  getDataset(int idRequest, int idDataset, bool conversion);
	
	/**
	 * @param netReqId
	 * @param res
	 * @return a code : -1 fail, 0 success , 1 : retry necessary (not enough bytes to create the file)
	*/
	int dataToFile(QUuid netReqId, RequestResponse res);

	/**
	 * @param filepath
	 * @param name
	 * @param idDataset
	 * @param idProcessing
	 * @return a QVariant containing whether a bool with the false value or a QUuid of the request if the sending process went well
	*/
	QVariant sendProcessedDataset(QString &filepath, QString name, int idDataset, int idProcessing);

private:
	medShanoir            * m_parent;
	LocalInfo             * m_info;
	Authenticator         * m_authent;
	SyncNetwork           * m_syncNet;
	QAtomicInt              m_medReqId;

	QMap<QUuid, int>                 m_requestIdMap;
	QMap<int, QVariant>              m_idResultMap;
	
	QStringList m_filesToRemove;
};
#endif // !ASYNC_NETWORK