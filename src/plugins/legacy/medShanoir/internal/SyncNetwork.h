#include <QObject>
#include <QEventLoop>
#include <QUuid>
#include <QVariant>
#include <QVector>
#include <QList>
#include <QJsonArray>

#include <medAbstractSource.h>

#include <RequestManager.h>
#include <Levels.h>

class Authenticator;
class medShanoir;
class LocalInfo;

using levelMinimalEntries = medAbstractSource::levelMinimalEntries;
using datasetAttributes = medAbstractSource::datasetAttributes;
using entryTypeLocal = medAbstractSource::entryType;

/**
 * @class SyncNetwork
 * @brief This class is used to make all the operation that require a request to the shanoir server in a synchronous way by the plugin. 
 * @details 
 * Synchroneous means that the plugin will wait for the request to be finished before continuing its execution.
 * A timeout management also implemented to avoid the plugin to wait forever for a request to be finished.
 * 
*/
class SyncNetwork : public QObject
{
	Q_OBJECT

public:
//////////////////////////
////  Constructor/Destructor
//////////////////////////

    SyncNetwork(medShanoir * parent,  LocalInfo *info, Authenticator * authent, RequestManager * requestMgr);
    
	~SyncNetwork();

//////////////////////////
////  MedShanoir methods
//////////////////////////  

	QList<levelMinimalEntries>    getMinimalEntries(unsigned int pi_uiLevel, QString parentKey);

	QList<QMap<QString, QString>> getMandatoryAttributes(unsigned int pi_uiLevel, QString parentKey);

	//bool getAdditionalAttributes(unsigned int pi_uiLevel, QString key, datasetAttributes &po_attributes); // it may be interesting to implement this function in the future

	QVariant getDirectData(unsigned int pi_uiLevel, QString key);

	bool addDirectData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey);
	

	bool createFolder(levelMinimalEntries &pio_minimalEntries, datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString parentKey);

	// The following functions have no reality in shanoir, and so in the plugin
	//bool createPath(QList<levelMinimalEntries> &pio_path, datasetAttributes const &pi_attributes, unsigned int pi_uiLevel = 0, QString parentKey = "");
	//bool alterMetaData(datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString key);
	//bool getThumbnail(QPixmap &po_thumbnail, unsigned int pi_uiLevel, QString key);
	//bool setThumbnail(QPixmap &pi_thumbnail, unsigned int pi_uiLevel, QString key);

public slots:
	/**
	 * @brief slot received from the request manager updating a request progression status
	 */
	void syncRequestSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);

signals:
//////////////////////////
////  signals emitted for sending a request with the request manager
//////////////////////////  

	void syncGet(QUuid, QNetworkRequest);
	void syncPost(QUuid, QNetworkRequest, QByteArray); 
	void syncPostMulti(QUuid, QNetworkRequest, QHttpMultiPart *);
	void syncPut(QUuid, QNetworkRequest, QByteArray);
	void syncAbort(QUuid);

private:
//////////////////////////
////  representations for synchronous requests management
//////////////////////////

	// represents the type of request that can be made to the shanoir server
	enum SyncRequestType : int
	{
		searchWithSolr = -6,
		getTreeViewDatasetProcessing = -5,
		getTreeViewDatasetDetails = -4,
		getTreeViewStudyDetail = -3,
		getTreeViewExaminations = -2,
		getTreeViewStudies = -1,
		getData = 1,
		addDataFile = 2,
		addDataContext = 3,
		addFolder = 4
	};

	// synchroneous request representation
	struct SyncRequest
	{
		QEventLoop* waiter;
		SyncRequestType type;
		RequestResponse response;
		bool upToDate;
	};

private:
//////////////////////////
//// request sending methods
//////////////////////////

	//     WAITERS : methods that send a request and wait for a it to be finished
	QUuid waitGetResult(SyncRequestType type, QNetworkRequest &req);
	QUuid waitPostResult(SyncRequestType type, QNetworkRequest &req, QByteArray &postData);
	QUuid waitPostMultiResult(SyncRequestType type, QNetworkRequest &req, QHttpMultiPart *postData);
	QUuid waitPutResult(SyncRequestType type, QNetworkRequest &req, QByteArray &postData);

	/**
	 * @param type type of the netReqId request
	 * @param netReqId id of a request 
	 * @brief keeps the given request up to date, by checking if any new interaction has been made with the server since the last time the request was checked. If it is not the case, the request is aborted.
	*/
	void updateRequestState(SyncRequestType &type, const QUuid &netReqId);

	/**
	 * @brief properly manage the death of a request, by removing it from the request map and showing an error message if needed
	 * @param netReqId the id of a finished request
	 */
	void manageRequestDeath(QUuid netReqId);

public:
//////////////////////////
////  Tree-view elements retrieving methods
//////////////////////////

	//      GETTERS : methods that send a request and parse the response to return the wanted data
	QList<StudyOverview> getStudies();
	QList<ExaminationOverview>   getExaminations(int studyId, int subjectId);
	StudyDetails                getStudyDetails(int studyId);
	DatasetDetails       getDatasetDetails(int studyId);  
	QJsonObject          getDatasetProcessing(int id); 

	/**
	 *  not used for now. Everything is in place to recuperate a result from a solr request.
	 * modify the signature to adapt to the request you want to make (see writeGetSolrRequest from ShanoirRequestPreparation.h)
	 */
	QJsonObject                 applySolrRequest();   

private:

	//      PARSERS : methods that parse the response of a request to return the wanted data
	QList<StudyOverview>      parseStudies(QJsonArray studies_response);
	QList<ExaminationOverview>        parseExaminations(QJsonArray examinations_response);
	QList<DatasetAcquisitionOverview> parseDatasetAcquisitions(QJsonArray dataset_acquisitions_response);
	QList<Dataset>            parseDatasets(QJsonArray datasets_response);
	QList<DatasetProcessingOverview>  parseDatasetProcessings(QJsonArray processings_response);
	StudyDetails                     parseStudy(QJsonObject &study_response);
	DatasetDetails            parseDatasetDetails(QJsonObject &dataset_response);

	//      CASTERS : methods that retrieve the tree-view children elements of a given parent id
	/**
	 * @brief  retrieve a list of studies
	*/
	QList<levelMinimalEntries> getRootMinimalEntries                ();           
	/**
	 * @brief retrieve a list of subjects contained by the given study
	*/
	QList<levelMinimalEntries> getStudyMinimalEntries               (QString id); 
	/**
	 * @brief  retrieve a list of examinations contained by the given subject
	*/
	QList<levelMinimalEntries> getSubjectMinimalEntries             (QString id); 
	/**
	 * @brief  retrieve a list of datasets-acquisitions contained by the given examination
	*/
	QList<levelMinimalEntries> getExaminationMinimalEntries			(QString id); 
	/**
	 * @brief  retrieve a list of datasets contained by the given dataset-acquisition
	*/
	QList<levelMinimalEntries> getDatasetAcquisitionMinimalEntries	(QString id); 
	/**
	 * @brief retrieve a list of dataset processings contained by the given dataset
	*/
	QList<levelMinimalEntries> getDatasetMinimalEntries				(QString id); 
	/**
	 * @brief retrieve a list of processed datasets contained by the given processing dataset
	*/
	QList<levelMinimalEntries> getProcessingDatasetMinimalEntries	(QString id); 



private:
	LocalInfo           * m_info;    // pointer to the plugin instance info
	Authenticator       * m_authent; // pointer to the plugin instance authenticator
	
	QMap<SyncRequestType, int> requestsDurations; // duration of each request type (in milliseconds). The value if -1 if it should not have a timer

	QMap<QUuid, SyncRequest> m_requestMap; // map of the active requests sent by the syncNetwork object.

	QStringList m_filesToRemove; // list of the files to remove when the plugin instance is closed
};