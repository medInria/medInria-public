#include <QObject>

#include <QEventLoop>
#include <QUuid>
#include <QVariant>
#include <QVector>
#include <QList>
#include <QJsonArray>

#include <RequestManager.h>
#include <Levels.h>

class Authenticator;
class medShanoir;
class LocalInfo;

#include <medAbstractSource.h>

using levelMinimalEntries = medAbstractSource::levelMinimalEntries;
using datasetAttributes = medAbstractSource::datasetAttributes;
using entryTypeLocal = medAbstractSource::entryType;


class SyncNetwork : public QObject
{
	Q_OBJECT

public:
    SyncNetwork(medShanoir * parent,  LocalInfo *info, Authenticator * authent, RequestManager * requestMgr);
    ~SyncNetwork();


	/* ***********************************************************************/
	/* *************** Get elements data *************************************/
	/* ***********************************************************************/
	QList<levelMinimalEntries>    getMinimalEntries(unsigned int pi_uiLevel, QString parentKey);
	QList<QMap<QString, QString>> getMandatoryAttributes(unsigned int pi_uiLevel, QString parentKey);

	// Not implemented yet
	//bool getAdditionalAttributes(unsigned int pi_uiLevel, QString key, datasetAttributes &po_attributes);


	/* ***********************************************************************/
	/* *************** Get data          *************************************/
	/* ***********************************************************************/
	QVariant getDirectData(unsigned int pi_uiLevel, QString key);

	/* ***********************************************************************/
	/* *************** Store data          ***********************************/
	/* ***********************************************************************/
	bool addDirectData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey);
	
	//bool createPath(QList<levelMinimalEntries> &pio_path, datasetAttributes const &pi_attributes, unsigned int pi_uiLevel = 0, QString parentKey = "");
	bool createFolder(levelMinimalEntries &pio_minimalEntries, datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString parentKey);

	// The following functions have no effect
	//bool alterMetaData(datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString key);
	//bool getThumbnail(QPixmap &po_thumbnail, unsigned int pi_uiLevel, QString key);
	//bool setThumbnail(QPixmap &pi_thumbnail, unsigned int pi_uiLevel, QString key);

public slots:
	void syncGetSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);
	void syncPostSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);
	void syncPutSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode);

signals:
	void syncGet(QUuid, QNetworkRequest);
	void syncPost(QUuid, QNetworkRequest, QByteArray); 
	void syncPostMulti(QUuid, QNetworkRequest, QHttpMultiPart *);
	void syncPut(QUuid, QNetworkRequest, QByteArray);

private:
	//     WAITERS
	QUuid waitGetResult(QNetworkRequest &req);
	QUuid waitPostResult(QNetworkRequest &req, QByteArray &postData);
	QUuid waitPostMultiResult(QNetworkRequest &req, QHttpMultiPart *postData);
	QUuid waitPutResult(QNetworkRequest &req, QByteArray &postData);

	// TREE-VIEW RETRIEVING FUNCTIONS
	//      GETTERS
	QList<StudyOverview> getStudies();
	QList<Examination>   getExaminations(int studyId, int subjectId);

public:
	Study                getStudyDetails(int studyId);
	DatasetDetails       getDatasetDetails(int studyId);  // not used for the tree-view but for the upload of data
	QJsonObject          getDatasetProcessing(int id); // not used for the tree-view but for the upload of data
	/**
	 *  not used for now. Everything is in place to recuperate a result from a solr request.
	 * modify the signature to adapt to the request you want to make (see writeGetSolrRequest from FileHelper.h)
	 */
	QJsonObject                 applySolrRequest();   

private:
	//      PARSERS
	QList<StudyOverview>      parseStudies(QJsonArray studies_response);
	QList<Examination>        parseExaminations(QJsonArray examinations_response);
	QList<DatasetAcquisition> parseDatasetAcquisitions(QJsonArray dataset_acquisitions_response);
	QList<Dataset>            parseDatasets(QJsonArray datasets_response);
	QList<DatasetProcessing>  parseDatasetProcessings(QJsonArray processings_response);
	Study                     parseStudy(QJsonObject &study_response);

	DatasetDetails            parseDatasetDetails(QJsonObject &dataset_response);

	//      CASTERS
	QList<levelMinimalEntries> getRootMinimalEntries                ();
	QList<levelMinimalEntries> getStudyMinimalEntries               (QString id);
	QList<levelMinimalEntries> getSubjectMinimalEntries             (QString id);
	QList<levelMinimalEntries> getExaminationMinimalEntries			(QString id);
	QList<levelMinimalEntries> getDatasetAcquisitionMinimalEntries	(QString id);
	QList<levelMinimalEntries> getDatasetMinimalEntries				(QString id);
	QList<levelMinimalEntries> getProcessingDatasetMinimalEntries	(QString id);

private:
	LocalInfo           * m_info;
	Authenticator       * m_authent;
	QAtomicInt            m_medReqId;

	QMap<QUuid, QPair<QEventLoop*, RequestResponse>> m_requestMap;


	QStringList m_filesToRemove;
};