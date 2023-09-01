#include "AsyncNetwork.h"

#include <QDir>
#include <QHttpPart>

#include <medShanoir.h>
#include <medNotif.h>

#include <LocalInfo.h>
#include <Authenticator.h>
#include <RequestManager.h>
#include <SyncNetwork.h>
#include <RequestPreparation.h>

#include <FileHelper.h>
#include <JsonHelper.h>


AsyncNetwork::AsyncNetwork(medShanoir * parent, LocalInfo *info, Authenticator * authent, SyncNetwork *syncNet, RequestManager * requester): 
	QObject(parent), m_parent(parent), m_info(info), m_authent(authent), m_syncNet(syncNet), m_medReqId(0)
{
	QObject::connect(this, &AsyncNetwork::asyncGet, requester, &RequestManager::httpGet, Qt::ConnectionType::QueuedConnection);
	QObject::connect(this, &AsyncNetwork::asyncPost, requester, &RequestManager::httpPost, Qt::ConnectionType::QueuedConnection);
	QObject::connect(this, &AsyncNetwork::asyncPostMulti, requester, &RequestManager::httpPostMulti, Qt::ConnectionType::QueuedConnection);
	QObject::connect(this, &AsyncNetwork::asyncPut, requester, &RequestManager::httpPut, Qt::ConnectionType::QueuedConnection);

	QObject::connect(this, &AsyncNetwork::asyncAbort, requester, &RequestManager::abort, Qt::ConnectionType::QueuedConnection);

}

AsyncNetwork::~AsyncNetwork()
{
	deleteAllFolders(m_filesToRemove);
}


/**
 * 
 * GET DATA methods
 * 
*/

int AsyncNetwork::getAssyncData(unsigned int pi_uiLevel, QString key)
{
 	int medId = -1;

	// checking that the level is appropriate for a processed dataset download
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
		int medIdRequest = ++m_medReqId;
		QUuid netReqId = getDataset(medIdRequest, id_ds, false);
		medId = m_requestIdMap[netReqId].first;
 	}

 	return medId;
}


QUuid AsyncNetwork::getDataset(int medId, int id_ds, bool conversion)
{
	// generation of the requests id
	QUuid netReqId = QUuid::createUuid();

	m_requestIdMap[netReqId].first = medId;
	m_requestIdMap[netReqId].second = getData;
	// saving temporarily the informations about the request in the result map 
	m_idResultMap[medId] = QString::number(id_ds)+"."+ QString::number(conversion);

	// sending the request
	QNetworkRequest req;
	writeNiftiDatasetRetrievingRequest(req, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), id_ds, conversion);

	emit asyncGet(netReqId, req);

	return netReqId;
}

void AsyncNetwork::getAsyncDataInterpretation(QUuid netReqId, RequestResponse res)
{
	int medId = m_requestIdMap[netReqId].first;
	if (res.code == SUCCESS_CODE) // finished with success
	{
		// successCode is about success of the conversion of the retrieved data into a file
		int successCode = dataToFile(netReqId, res);
		QString tmpResult = m_idResultMap[medId].toString();
		if(successCode == 1 && tmpResult.size()>0)
		{
			QStringList parts = tmpResult.split('.');
			if(parts.size()==2 && parts[1].toInt() == 0)
			{
				getDataset(medId,parts[0].toInt(), true);
			}
			else
			{
				successCode = 0;
			} 
		}
		else if(successCode == -1) 
		{
			emit m_parent->progress(medId, eRequestStatus::faild);
		}
		if(successCode == 0)
		{
			emit m_parent->progress(medId, eRequestStatus::finish);
		}
		m_requestIdMap.remove(netReqId);
	}
	else if (res.code == UPLOAD_CODE || res.code == DOWNLOAD_CODE) // request in progress
	{
		int bytesSent = res.headers["bytesSent"].toInt();
		int bytesTotal = res.headers["bytesTotal"].toInt();
		emit m_parent->progress(medId, eRequestStatus::pending);
	}
	else // an error occured 
	{
		// notify
		QString errorMessage = "ERROR WHILE GETTING DATA \n NETWORK ERROR CODE :" + QString::number(res.code);
		medNotif::createNotif(notifLevel::error, "SHANOIR PLUGIN", errorMessage);
		// trace
		qDebug() << "\nNETWORKERROR (code = " << res.code << ") WHILE GETTING DATA ( request number ="<< medId << ")\nLOOK AT https://doc.qt.io/qt-5/qnetworkreply.html#NetworkError-enum for more information\n";
		// handling the error
		emit m_parent->progress(medId, eRequestStatus::faild);
		m_requestIdMap.remove(netReqId);
	}
}

void AsyncNetwork::asyncGetSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode)
{
	if (m_requestIdMap.contains(netReqId))
	{
		getAsyncDataInterpretation(netReqId , { statusOrHttpCode, headers, payload });
	}
}

QVariant AsyncNetwork::getAsyncResults(int pi_iRequest)
{
	QVariant qRes;
	if (m_idResultMap.contains(pi_iRequest)) qRes = m_idResultMap[pi_iRequest];
	return qRes;
}

int AsyncNetwork::dataToFile(QUuid netReqId, RequestResponse res)
{
	int successCode = (res.payload.size() < 100) ? 1 : -1;
	int medId = m_requestIdMap[netReqId].first;
	if (successCode != 1)
	{
		QVariant pathRes = decompressNiftiiFromRequest(m_info->getStoragePath() + QString::number(medId) + "/", res.headers, res.payload, m_filesToRemove, 60000);//1 minute before deletion
		if (pathRes.type() == QVariant::String)
		{ // everything went well, we receive the corresponding path
			m_idResultMap[medId] = pathRes;
			m_requestIdMap.remove(netReqId);
			successCode = 0;
		}
		else if (pathRes.type() == QVariant::Int)
		{
			qDebug() << "DECOMPRESSION ERROR " << pathRes.toInt();
			m_idResultMap[medId];
		}
	}
	return successCode;
}

/**
 * 
 * ADD DATA methods
 * 
*/

int AsyncNetwork::addAssyncData(QVariant data, levelMinimalEntries & pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey)
{
	int medId = -1;
	QString path = data.toString(); 

	// checking that the level is appropriate for a processed dataset creation
	int parent_level = pi_uiLevel - 1; // working with parent of the data to create (easier for understanding)
	QStringList parts = parentKey.split('.');
	bool psing_dataset_level = parent_level == 5 && parts.size() == 6;
	if(psing_dataset_level && path.endsWith(".nii.gz"))
	{
		QVariant sending = sendProcessedDataset(path, pio_minimalEntries.name, parts[4].toInt(), parts[5].toInt());
		if(sending.isValid() && sending.canConvert<QUuid>())
		{
			QUuid netReqId = sending.value<QUuid>();
			medId =  m_requestIdMap[netReqId].first;
		}
	}
	return medId;
}

QVariant AsyncNetwork::sendProcessedDataset(QString &filepath, QString name, int idDataset, int idProcessing)
{
	QString distant_path;

	// construction of the processed dataset to upload
	QFile *file = new QFile(filepath);
	file->open(QIODevice::ReadOnly);
	QFileInfo fileInfo(filepath);

	if (fileInfo.exists())
	{
		// generation of the requests ids 
		int medId = ++m_medReqId;
		QUuid netReqId = QUuid::createUuid();

		m_requestIdMap[netReqId].first = medId;
		m_requestIdMap[netReqId].second = addDataFile;

		// saving temporarily the informations about the request in the result map 
		m_idResultMap[medId] =  QString::number(idDataset) +"."+ QString::number(idProcessing)+"."+ name;

		// sending the request
		QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
		QNetworkRequest req;
		writeProcessedDatasetUploadRequest(req, multiPart, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), file, fileInfo);

		emit asyncPostMulti(netReqId, req, multiPart);

		return netReqId;
	}
	else 
	{
		return false;
	}
}

QVariant AsyncNetwork::sentDatasetFileInterpretation(QUuid netReqIdFile, RequestResponse res)
{
	int medId = m_requestIdMap[netReqIdFile].first;

	QString distant_path;
	if (res.code == SUCCESS_CODE && !res.payload.isNull())
	{
		distant_path = QString::fromUtf8(res.payload);
	}

	QStringList parts = m_idResultMap[medId].toString().split('.');  // format idDs.idPsingDs.psDsname

	if(distant_path.length()>0 && parts.size()==3)
	{
		// generation of the request id
		QUuid netReqIdContext = QUuid::createUuid();

		m_requestIdMap[netReqIdContext].first = medId;
		m_requestIdMap[netReqIdContext].second = addDataContext;

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

		// sending the request
		QNetworkRequest req;
		QByteArray postData;
		writeProcessedDatasetUploadContextRequest(req, postData, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), ds_details, study, distant_path, processedDatasetType, processedDatasetName, parentDatasetProcessing);

		emit asyncPost(netReqIdContext, req, postData);

		return netReqIdContext;
	}
	return false;
}

void AsyncNetwork::sentDatasetContextInterpretation(QUuid netReqId, RequestResponse res)
{
	int medId = m_requestIdMap[netReqId].first;
	m_idResultMap[medId] = true;
	emit m_parent->progress(medId, eRequestStatus::finish);
}


void AsyncNetwork::addAsyncDataInterpretation(QUuid netReqId, RequestResponse res)
{
	int medId = m_requestIdMap[netReqId].first;
	
	if (res.code == SUCCESS_CODE) // finished with success
	{
		// checking the add data step 
		AsyncRequestType step = m_requestIdMap[netReqId].second;
		if(step == addDataFile)
		{
			sentDatasetFileInterpretation(netReqId, res);
		}
		else if(step == addDataContext)
		{
			sentDatasetContextInterpretation(netReqId, res);
		}
	}
	else if (res.code == UPLOAD_CODE || res.code == DOWNLOAD_CODE) //request in progress
	{
		int bytesSent = res.headers["bytesSent"].toInt();
		int bytesTotal = res.headers["bytesTotal"].toInt();
		emit m_parent->progress(medId, eRequestStatus::pending);
	}
	else // an error occured during the request sending (http error)
	{
		// trace
		qDebug() << "\nNETWORKERROR (code = " << res.code << ") WHILE ADDING DATA ( request number ="<< medId << ")\nLOOK AT https://doc.qt.io/qt-5/qnetworkreply.html#NetworkError-enum for more information\n";
		// notify
		QString activity;
		if(m_requestIdMap[netReqId].second == addDataFile)
		{
			activity = "UPLOADING A DATASET";
		}
		else 
		{
			activity = "UPLOADING A DATASET CONTEXT";
		}
		QString errorMessage =  "ERROR WHILE "+ activity +"\n NETWORK ERROR CODE :"+QString::number(res.code);
		medNotif::createNotif(notifLevel::error, "SHANOIR PLUGIN", errorMessage);
		// handling the error
		emit m_parent->progress(medId, eRequestStatus::faild);
		m_requestIdMap.remove(netReqId);
		m_idResultMap[medId] = false;
	}
}

void AsyncNetwork::asyncPostSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode)
{
	if(m_requestIdMap.contains(netReqId))
	{
		addAsyncDataInterpretation(netReqId , { statusOrHttpCode, headers, payload });
	}
}

void AsyncNetwork::asyncPutSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode)
{
	if(m_requestIdMap.contains(netReqId))
	{
		// HERE call the interpretation(s) function that is/are based on PUT request 
		// for now, none of them exist
	}
}

void AsyncNetwork::abort(int medId)
{
	for (auto it = m_requestIdMap.begin(); it != m_requestIdMap.end(); ++it)
	{
		if (it.value().first == medId)
		{
			QUuid netReqId = it.key();
			emit asyncAbort(netReqId);
			return;
		}
	}
}
