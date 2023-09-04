#include <QNetworkRequest>
#include <QHttpMultiPart>
#include <QJsonDocument>
#include <QFile>
#include <QFileInfo>

#include <Levels.h>

//
// AUTHENTICATION FUNCTIONS
//
inline void writeInitAuthenticationRequest(QNetworkRequest &req, QByteArray &postData, const QString domain,  const QString username, const QString password)
{
	req.setUrl("https://"+domain+"/auth/realms/shanoir-ng/protocol/openid-connect/token");
	req.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

	postData.append("client_id=" + QUrl::toPercentEncoding("shanoir-uploader"));
	postData.append("&grant_type=" + QUrl::toPercentEncoding("password"));
	postData.append("&scope=" + QUrl::toPercentEncoding("offline_access"));
	postData.append("&username=" + QUrl::toPercentEncoding(username));
	postData.append("&password=" + QUrl::toPercentEncoding(password));
}

inline void writeTokenUpdateRequest(QNetworkRequest &req, QByteArray &postData, const QString domain, const QString refreshToken)
{
	req.setUrl("https://" + domain + "/auth/realms/shanoir-ng/protocol/openid-connect/token");
	req.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
	
	postData.append("client_id="+QUrl::toPercentEncoding("shanoir-uploader"));
	postData.append("&grant_type="+ QUrl::toPercentEncoding("refresh_token"));
	postData.append("&refresh_token="+  QUrl::toPercentEncoding(refreshToken));
}

///
/// COMMON FUNCTIONS TO SYNCNETWORK AND ASYNCNETWORK (get & add data from shanoir)
///

inline void writeNiftiDatasetRetrievingRequest(QNetworkRequest &req, QString baseUrl, QString token, int datasetId, bool conversion = false)
{
    QString queryParameter = conversion ?"?format=nii&converterId=4" :"?format=nii";
    req.setUrl(baseUrl + "datasets/datasets/download/" + QString::number(datasetId) + queryParameter);
	req.setRawHeader("Authorization", ("Bearer " + token).toUtf8());
}

inline void writeProcessedDatasetUploadRequest(QNetworkRequest &req, QHttpMultiPart *multiPart, QString baseUrl, QString token, QFile *file, QFileInfo &fileInfo)
{
    req.setUrl(baseUrl + "import/importer/upload_processed_dataset/");
    req.setRawHeader("Authorization", ("Bearer " + token).toUtf8());
    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(R"(form-data; name="image"; filename=")" + fileInfo.fileName() + R"(")"));
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-gzip"));
    imagePart.setBodyDevice(file);
    file->setParent(multiPart);
    multiPart->append(imagePart);
}

inline void writeProcessedDatasetUploadContextRequest(QNetworkRequest &req, QByteArray &postData, QString baseUrl, QString token, DatasetDetails dsDetails, StudyOverview study, QString distantPath, QString processedDatasetType,
	QString processedDatasetName, QJsonObject parentDatasetProcessing)
{
    req.setUrl(baseUrl + "datasets/datasets/processedDataset");
    req.setRawHeader("Authorization", ("Bearer " + token).toUtf8());
    req.setRawHeader("Content-Type", "application/json");

	QJsonObject data;
	data["subjectId"] = dsDetails.subject_id;
	data["subjectName"] = dsDetails.subject_name;
	data["studyName"] = study.name;
	data["studyId"] = study.id;
	data["datasetType"] = dsDetails.type;
	data["processedDatasetFilePath"] = distantPath;
	data["processedDatasetType"] = processedDatasetType;
	data["processedDatasetName"] = processedDatasetName;
	data["datasetProcessing"] = parentDatasetProcessing;
	data["timestamp"] = QDateTime::currentMSecsSinceEpoch();

    QJsonDocument bodyDocument(data);
    postData = bodyDocument.toJson();
}

///
/// FUNCTIONS ONLY USED IN SYNCNETWORK 
///

inline void writeProcessingDatasetUploadRequest(QNetworkRequest &req, QByteArray &postData, QString baseUrl, QString token, DatasetProcessingOverview in_processing_ds, QString processingDate, int studyId)
{
	req.setUrl(baseUrl + "datasets/datasetProcessing");
	req.setRawHeader("Authorization", ("Bearer " + token).toUtf8());
	req.setRawHeader("Content-Type", "application/json");

	QJsonObject inputDatasets; // possibility to loop here for adding several inputDatasets
	inputDatasets.insert("id", in_processing_ds.inputDatasets.first().id);
	inputDatasets.insert("type", in_processing_ds.inputDatasets.first().type);
	inputDatasets.insert("processings", QJsonArray());

	QJsonObject data;
	data.insert("datasetProcessingType", in_processing_ds.type);
	data.insert("inputDatasets", QJsonArray() << inputDatasets);
	data.insert("outputDatasets", QJsonArray());
	data.insert("processingDate", processingDate);
	data.insert("studyId", studyId);

	QJsonDocument bodyDocument(data);
	postData = bodyDocument.toJson();
}

// TREE-VIEW FUNCTIONS

inline void writeGetStudiesRequest(QNetworkRequest &req, QString baseUrl, QString token)
{
	req.setUrl(baseUrl + "studies/studies");
	req.setRawHeader("Authorization", ("Bearer " + token).toUtf8());
}

inline void writeGetStudyRequest(QNetworkRequest &req, QString baseUrl, QString token, int studyId)
{
	req.setUrl(baseUrl + "studies/studies/" + QString::number(studyId));
	req.setRawHeader("Authorization", ("Bearer " + token).toUtf8());
}

inline void writeGetExaminationsRequest(QNetworkRequest &req, QString baseUrl, QString token, int studyId, int subjectId)
{
	req.setUrl(baseUrl + "datasets/examinations/subject/" + QString::number(subjectId) + "/study/" + QString::number(studyId));
	req.setRawHeader("Authorization", ("Bearer " + token).toUtf8());
}

inline void writeGetDatasetRequest(QNetworkRequest &req, QString baseUrl, QString token, int datasetId)
{
	req.setUrl(baseUrl + "datasets/datasets/" + QString::number(datasetId));
	req.setRawHeader("Authorization", ("Bearer " + token).toUtf8());
}

inline void writeGetDatasetProcessingRequest(QNetworkRequest &req, QString baseUrl, QString token, int datasetProcessingId)
{
	req.setUrl(baseUrl +  "datasets/datasetProcessing/" + QString::number(datasetProcessingId));
	req.setRawHeader("Authorization", ("Bearer " + token).toUtf8());
}

// SOLR REQUEST

inline void writeGetSolrRequest(QNetworkRequest &req, QByteArray &postData, QString baseUrl, QString token, QString solrRequest = "", int pageSize = 50, int pageId = 0, QString sort = "id,ASC", bool expertMode = false)
{
	req.setUrl(baseUrl +  "datasets/solr?page=" + QString::number(pageId)+"&size="+ QString::number(pageSize) + "&sort="+sort);
	req.setRawHeader("Authorization", ("Bearer " + token).toUtf8());
  req.setRawHeader("Content-Type", "application/json");
  
  QJsonObject data;
  data.insert("expertMode", expertMode);
  data.insert("solrRequest", solrRequest);

  QJsonDocument bodyDocument(data);
	postData = bodyDocument.toJson();
}
