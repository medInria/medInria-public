#ifndef ASYNC_NETWORK
#define ASYNC_NETWORK

#include <QObject>
#include <QUuid>
#include <QVariant>
#include <QPair>

#include <medAbstractSource.h>
#include <RequestManager.h>


using levelMinimalEntries = medAbstractSource::levelMinimalEntries;
using datasetAttributes = medAbstractSource::datasetAttributes;
using eRequestStatus = medAbstractSource::eRequestStatus;

class Authenticator;
class medShanoir;
class LocalInfo;
class SyncNetwork;


/**
 * @class AsyncNetwork
 * @brief This class is used to make all the operation that require a request to the shanoir server in an asynchronous way by the plugin. 
 * @details 
 * Asynchroneous means that the plugin will follow the execution of the requests using signals and slots with the request manager.
*/
class AsyncNetwork : public QObject
{
	Q_OBJECT
public:
//////////////////////////
////  Constructor/Destructor
//////////////////////////

	AsyncNetwork(medShanoir * parent, LocalInfo *info, Authenticator * authent, SyncNetwork *syncNet, RequestManager * requestMgr);

	~AsyncNetwork();

//////////////////////////
////  MedShanoir methods
//////////////////////////  

	int getAssyncData(unsigned int pi_uiLevel, QString key);

	int addAssyncData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey);

	QVariant getAsyncResults(int pi_iRequest);

    void abort(int pi_iRequest); 

public slots:
//////////////////////////
////  slots received from the request manager updating a request progression status
//////////////////////////  

	void asyncGetSlot (QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);
	void asyncPostSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);
	void asyncPutSlot (QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);
	
signals:
//////////////////////////
////  signals emitted for sending a request with the request manager
//////////////////////////  

	void asyncGet(QUuid, QNetworkRequest);
	void asyncPost(QUuid, QNetworkRequest, QByteArray);
	void asyncPostMulti(QUuid, QNetworkRequest, QHttpMultiPart *);
	void asyncPut(QUuid, QNetworkRequest, QByteArray);
	void asyncAbort(QUuid);

private:
//////////////////////////
////  representation for asynchronous requests management
//////////////////////////

	// represents the type of request that can be made to the shanoir server
	enum AsyncRequestType : int
	{
		getData = 1,
		addDataFile = 2,
		addDataContext = 3
	};

private:
//////////////////////////
//// methods for the interpretation of request progression
//////////////////////////

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

	
//////////////////////////
//// sending request methods
//////////////////////////

	QUuid  getDataset(int idRequest, int idDataset, bool conversion);
	
	/**
	 * @param filepath
	 * @param name
	 * @param idDataset
	 * @param idProcessing
	 * @return a QVariant containing whether a bool with the false value or a QUuid of the request if the sending process went well
	*/
	QVariant sendProcessedDataset(QString &filepath, QString name, int idDataset, int idProcessing);

//////////////////////////
//// handling request results methods
//////////////////////////

	/**
	 * @param netReqId
	 * @param res
	 * @return a code : -1 fail, 0 success , 1 : retry necessary (not enough bytes to create the file)
	*/
	int dataToFile(QUuid netReqId, RequestResponse res);


private:
	medShanoir            * m_parent;   // pointer to the plugin instance
	LocalInfo             * m_info;     // pointer to the plugin instance info
	Authenticator         * m_authent;  // pointer to the plugin instance authenticator
	SyncNetwork           * m_syncNet;  // pointer to the plugin instance tree-view retriever

	QStringList m_filesToRemove; // list of the files that were created by async network, and that need to be automatically deleted
	QString m_rootPath; // path where all the creation of the folders and files will be done by the object.
	
//////////////////////////
////  request management variables
//////////////////////////
/**
 * Each request sent by the plugin instance is identified by a unique id. (QUuid)
 * medInria identifies each asynchronous operation by an id (int).
 * This separation of id is necessary because for some operations, multiple requests need to be done to the server.
*/
	QAtomicInt              m_medReqId; // id of the next async operation to be made by the plugin instance
	
	QMap<QUuid, QPair<int, AsyncRequestType>>   m_requestIdMap; // map of the active requests sent by the asyncNetwork object, pairing operation id with the request type

	QMap<int, QVariant>                    m_idResultMap; // map of the results associated to each asynchronous operation id. Those results can be temporary, helping to build the final result of the operation after all the requests are done.
	
};
#endif // !ASYNC_NETWORK