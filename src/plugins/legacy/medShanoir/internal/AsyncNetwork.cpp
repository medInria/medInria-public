#include "AsyncNetwork.h"

#include <QDir>
#include <QHttpPart>

#include <medShanoir.h>
#include <LocalInfo.h>
#include <Authenticator.h>
#include <RequestManager.h>
#include <SyncNetwork.h>

#include <FileHelper.h>
#include <JsonHelper.h>


AsyncNetwork::AsyncNetwork(medShanoir * parent, LocalInfo *info, Authenticator * authent, SyncNetwork *syncNet, RequestManager * requester): 
	QObject(parent), m_parent(parent), m_info(info), m_authent(authent), m_syncNet(syncNet), m_medReqId(0)
{
	QObject::connect(this, &AsyncNetwork::asyncGet, requester, &RequestManager::httpGet, Qt::ConnectionType::QueuedConnection);
	QObject::connect(this, &AsyncNetwork::asyncPost, requester, &RequestManager::httpPost, Qt::ConnectionType::QueuedConnection);
	QObject::connect(this, &AsyncNetwork::asyncPostMulti, requester, &RequestManager::httpPostMulti, Qt::ConnectionType::QueuedConnection);
	QObject::connect(this, &AsyncNetwork::asyncPut, requester, &RequestManager::httpPut, Qt::ConnectionType::QueuedConnection);
}

AsyncNetwork::~AsyncNetwork()
{

}

int AsyncNetwork::getAssyncData(unsigned int pi_uiLevel, QString key)
{
 	int idRequest = -1;

 	QStringList parts = key.split('.');
 	bool dataset_level = pi_uiLevel == 4 && parts.size() == 5;
 	bool ps_dataset_level = pi_uiLevel == 6 && parts.size() == 7;
 	if (dataset_level || ps_dataset_level) // dataset level or processed dataset
 	{
 		int id_ds = parts[4].toInt();
 		if (ps_dataset_level)
 		{
 			id_ds = parts[6].toInt();
 		}
		
		// Declare and register medId and http request Id
		QUuid netReqId = QUuid::createUuid();
		idRequest = ++m_medReqId;

		m_requestResultMap[ASYNC_GET_DATA][netReqId];
		m_requestIdMap[netReqId] = idRequest;

		// Perform request
		QNetworkRequest req(m_info->getBaseURL() + "datasets/datasets/download/" + QString::number(id_ds) + "?format=nii");
		req.setRawHeader("Authorization", ("Bearer " + m_authent->getCurrentAccessToken()).toUtf8());
		emit asyncGet(netReqId, req);
 	}

 	return idRequest;
}

int AsyncNetwork::addAssyncData(QVariant data, levelMinimalEntries & pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey)
{
	int idRequest = -1;
	// checking that the level is appropriate for a processed dataset creation
	int parent_level = pi_uiLevel - 1;
	QString path = data.toString(); 
	QStringList parts = parentKey.split('.');
	bool psing_dataset_level = parent_level == 5 && parts.size() == 6;
	if(psing_dataset_level && path.endsWith(".nii.gz"))
	{
		idRequest = sendProcessedDataset(path, pio_minimalEntries, parts);
	}
	return idRequest;
}

int AsyncNetwork::sendProcessedDataset(QString &filepath, levelMinimalEntries & pio_minimalEntries, QStringList &parts)
{
	int idRequest = -1;

	QString distant_path;

	// construction of the processed dataset to upload
	QFile *file = new QFile(filepath);
	file->open(QIODevice::ReadOnly);
	QFileInfo fileInfo(filepath);

	if (fileInfo.exists())
	{
		// construction of the request
		QString url = m_info->getBaseURL() + "import/importer/upload_processed_dataset/";
		QNetworkRequest req(url);
		req.setRawHeader("Authorization", ("Bearer " + m_authent->getCurrentAccessToken()).toUtf8());
		QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
		QHttpPart imagePart;
		imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(R"(form-data; name="image"; filename=")" + fileInfo.fileName() + R"(")"));
		imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-gzip"));
		imagePart.setBodyDevice(file);
		file->setParent(multiPart);
		multiPart->append(imagePart);

		// sending the request
		// Declare and register medId and http request Id
		QUuid netReqId = QUuid::createUuid();
		idRequest = ++m_medReqId;

		m_requestResultMap[ASYNC_ADD_DATA_FILE][netReqId];
		m_requestIdMap[netReqId] = idRequest;

		m_idResultMap[idRequest] = parts[4]+"."+parts[5]+"."+ pio_minimalEntries.name;

		// Perform request
		emit asyncPostMulti(netReqId, req, multiPart);
	}
	return idRequest;
}

void AsyncNetwork::sendProcessedDatasetContext(int medId)
{
	QUuid netReqId = m_requestIdMap.key(medId);

	RequestResponse res = m_requestResultMap[ASYNC_ADD_DATA_FILE][netReqId];

	QString distant_path;
	if (res.code == 200 && !res.payload.isNull())
	{
		distant_path = QString::fromUtf8(res.payload);
	}

	QStringList parts = m_idResultMap[medId].toString().split('.');  // format idDs.idPsingDs.psDsname

	if(distant_path.length()>0 && parts.size()==3)
	{
		// preparation of the request
		int dsId = parts[0].toInt();
		int psingId = parts[1].toInt();
		QString name = parts[2];
		// building the context of the processed dataset
		DatasetDetails ds_details = m_syncNet->getDatasetDetails(dsId);
		Study s = m_syncNet->getStudyDetails(ds_details.study_id);
		StudyOverview study = {s.id, s.name};
		QString processedDatasetType = "RECONSTRUCTEDDATASET";
		QString processedDatasetName = name;
		QJsonObject parentDatasetProcessing = m_syncNet->getDatasetProcessing(psingId);

		// construction of the request
		QString url = m_info->getBaseURL() + "datasets/datasets/processedDataset";
		QNetworkRequest req(url);
		req.setRawHeader("Authorization", ("Bearer " + m_authent->getCurrentAccessToken()).toUtf8());
		req.setRawHeader("Content-Type", "application/json");

		QJsonObject data;
		data["subjectId"] = ds_details.subject_id;
		data["subjectName"] = ds_details.subject_name;
		data["studyName"] = study.name;
		data["studyId"] = study.id;
		data["datasetType"] = ds_details.type;
		data["processedDatasetFilePath"] = distant_path;
		data["processedDatasetType"] = processedDatasetType;
		data["processedDatasetName"] = processedDatasetName;
		data["datasetProcessing"] = parentDatasetProcessing;
		data["timestamp"] = QDateTime::currentMSecsSinceEpoch();

		QJsonDocument bodyDocument(data);
		QByteArray postData = bodyDocument.toJson();

		// sending the request
		// Declare and register medId and http request Id
		QUuid netReqId = QUuid::createUuid();

		m_requestResultMap[ASYNC_ADD_DATA_CONTEXT][netReqId];
		m_requestIdMap[netReqId] = medId;

		// Perform request
		emit asyncPost(netReqId, req, postData);
	}
}

void AsyncNetwork::sentDatasetInterpretation(int medId)
{
	QUuid netReqId = m_requestIdMap.key(medId);

	RequestResponse res = m_requestResultMap[ASYNC_ADD_DATA_CONTEXT][netReqId];
	if(res.code==200)
	{
		m_idResultMap[medId] = true;
		emit m_parent->progress(medId, eRequestStatus::finish);
	}
	else 
	{
		emit m_parent->progress(medId, eRequestStatus::faild);
	}
}

void AsyncNetwork::asyncGetSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode)
{
	if (m_requestResultMap[ASYNC_GET_DATA].contains(netReqId))
	{
		if (statusOrHttpCode == 0) // error
		{
			//TODO:
		}
		else if (statusOrHttpCode == 1 || statusOrHttpCode == 2) // in progress
		{
			int bytesSent = headers["bytesSent"].toInt();
			int bytesTotal = headers["bytesTotal"].toInt();
			emit m_parent->progress(m_requestIdMap[netReqId], eRequestStatus::pending);
		}
		else if (statusOrHttpCode == 200) // finsih ok
		{
			m_requestResultMap[ASYNC_GET_DATA][netReqId] = { statusOrHttpCode, headers, payload };
			dataToFile(m_requestIdMap[netReqId]);
		}
		else if (statusOrHttpCode/100 == 4) // error
		{
			emit m_parent->progress(m_requestIdMap[netReqId], eRequestStatus::faild);
			m_requestResultMap[ASYNC_GET_DATA].remove(netReqId);
			m_requestIdMap.remove(netReqId);
		}
	}
}

QVariant AsyncNetwork::getAsyncResults(int pi_iRequest)
{
	QVariant qRes;

	if (m_idResultMap.contains(pi_iRequest)) qRes = m_idResultMap[pi_iRequest];

	return qRes;
}

void AsyncNetwork::asyncPostSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode)
{
	if (m_requestResultMap[ASYNC_ADD_DATA_CONTEXT].contains(netReqId))
	{
		if (statusOrHttpCode == 0) // error
		{
			//TODO:
		}
		else if (statusOrHttpCode == 1 || statusOrHttpCode == 2) // in progress
		{
			int bytesSent = headers["bytesSent"].toInt();
			int bytesTotal = headers["bytesTotal"].toInt();
			emit m_parent->progress(m_requestIdMap[netReqId], eRequestStatus::pending);
		}
		else if (statusOrHttpCode == 200) // finiash ok
		{
			m_requestResultMap[ASYNC_ADD_DATA_CONTEXT][netReqId] = { statusOrHttpCode, headers, payload };
			sentDatasetInterpretation(m_requestIdMap[netReqId]);
		}
		else if (statusOrHttpCode / 100 == 4) // error
		{
			emit m_parent->progress(m_requestIdMap[netReqId], eRequestStatus::faild);
			m_requestResultMap[ASYNC_ADD_DATA_CONTEXT].remove(netReqId);
			m_requestIdMap.remove(netReqId);
		}
	}
	else if (m_requestResultMap[ASYNC_ADD_DATA_FILE].contains(netReqId))
	{
		if (statusOrHttpCode == 0) // error
		{
			//TODO:
		}
		else if (statusOrHttpCode == 1 || statusOrHttpCode == 2) // in progress
		{
			int bytesSent = headers["bytesSent"].toInt();
			int bytesTotal = headers["bytesTotal"].toInt();
			emit m_parent->progress(m_requestIdMap[netReqId], eRequestStatus::pending);
		}
		else if (statusOrHttpCode == 200) // finish ok
		{
			m_requestResultMap[ASYNC_ADD_DATA_FILE][netReqId] = { statusOrHttpCode, headers, payload };
			sendProcessedDatasetContext(m_requestIdMap[netReqId]);
		}
		else if (statusOrHttpCode / 100 == 4) // error
		{
			emit m_parent->progress(m_requestIdMap[netReqId], eRequestStatus::faild);
			m_requestResultMap[ASYNC_ADD_DATA_FILE].remove(netReqId);
			m_requestIdMap.remove(netReqId);
		}
	}
}

void AsyncNetwork::asyncPutSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode)
{
	//TODO OR NOT NEED IMPLEMENTED
}

void AsyncNetwork::dataToFile(int medId)
{

	QUuid netReqId = m_requestIdMap.key(medId);

	int & code     = m_requestResultMap[ASYNC_GET_DATA][netReqId].code;
	QJsonObject & headers  = m_requestResultMap[ASYNC_GET_DATA][netReqId].headers;
	QByteArray & fileData = m_requestResultMap[ASYNC_GET_DATA][netReqId].payload;

	QString fileName;

	if (verifyJsonKeys(headers, { "Content-Disposition" }))
	{
		fileName = headers.value("Content-Disposition").toString().split("filename=")[1].split(";").first();
	}

	if (!fileName.isEmpty())
	{
		QString filePath = m_info->getStoragePath() + QString::number(medId) + "/" + fileName;
		QString zipPath = saveFileData(fileData, filePath);
		QString extractionPath = extractZipFile(zipPath);
		QDir folder(extractionPath);

		// Find the nifti file in the folder
		QStringList filters;
		filters << "*.nii" << "*.nii.gz";
		QStringList files = folder.entryList(filters, QDir::Files | QDir::NoDotAndDotDot);
		if (files.size() > 0)
		{
			QString dataPath = folder.absoluteFilePath(files[0]);

			m_idResultMap[medId] = dataPath;
		
			m_requestResultMap[ASYNC_GET_DATA].remove(netReqId);
			m_requestIdMap.remove(netReqId);

			emit m_parent->progress(medId, eRequestStatus::finish);

			m_filesToRemove.push_back(QPair<qint64, QString>({QDateTime::currentSecsSinceEpoch(), extractionPath}));
			//TODO: delete the file from the filesystem in a reasonnable time and DON'T FORGET clean m_idResultMap (call cleaner)
		}
		else
		{
			//TODO trace
			emit m_parent->progress(medId, eRequestStatus::faild);
		}
	}
	else
	{
		//TODO trace
		emit m_parent->progress(medId, eRequestStatus::faild);
	}
}
