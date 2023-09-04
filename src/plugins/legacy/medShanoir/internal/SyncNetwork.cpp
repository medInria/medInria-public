#include <QJsonObject>
#include <QHttpMultiPart>

#include <medShanoir.h>
#include <medNotif.h>

#include <Authenticator.h>
#include <RequestManager.h>
#include <LocalInfo.h>
#include <JsonHelper.h>
#include <FileHelper.h>
#include <RequestPreparation.h>

#include "SyncNetwork.h"

SyncNetwork::SyncNetwork(medShanoir * parent,  LocalInfo *info, Authenticator * authent, RequestManager * requester) :QObject(parent), m_info(info), m_authent(authent)
{
	QObject::connect(this, &SyncNetwork::syncGet, requester, &RequestManager::httpGet, Qt::ConnectionType::QueuedConnection);
	QObject::connect(this, &SyncNetwork::syncPost, requester, &RequestManager::httpPost, Qt::ConnectionType::QueuedConnection);
	QObject::connect(this, &SyncNetwork::syncPostMulti, requester, &RequestManager::httpPostMulti, Qt::ConnectionType::QueuedConnection);
	QObject::connect(this, &SyncNetwork::syncPut, requester, &RequestManager::httpPut, Qt::ConnectionType::QueuedConnection);

	QObject::connect(this, &SyncNetwork::syncAbort, requester, &RequestManager::abort, Qt::ConnectionType::QueuedConnection);


	requestsDurations[searchWithSolr]               = 10000; // 10 seconds before considerating the request is lost
	requestsDurations[getTreeViewDatasetProcessing] = 10000;
	requestsDurations[getTreeViewDatasetProcessing] = 10000;
	requestsDurations[getTreeViewDatasetDetails]    = 10000;
	requestsDurations[getTreeViewStudyDetail]       = 10000;
	requestsDurations[getTreeViewExaminations]      = 10000;
	requestsDurations[getTreeViewStudies]           = 10000;
	requestsDurations[getData]                      = -1;
	requestsDurations[addDataFile]                  = -1;
	requestsDurations[addDataContext]               = -1;
	requestsDurations[addFolder]                    = 10000;

}

SyncNetwork::~SyncNetwork()
{
	deleteAllFolders(m_filesToRemove);
}



QList<levelMinimalEntries> SyncNetwork::getMinimalEntries(unsigned int pi_uiLevel, QString parentKey)
{
	QList<medAbstractSource::levelMinimalEntries> entries;
	switch (pi_uiLevel)
	{
	case 0:
		entries = getRootMinimalEntries();
		break;
	 case 1:
	 	entries = getStudyMinimalEntries(parentKey);
	 	break;
	 case 2:
	 	entries = getSubjectMinimalEntries(parentKey);
	 	break;
	 case 3:
	 	entries = getExaminationMinimalEntries(parentKey);
	 	break;
	 case 4:
	 	entries = getDatasetAcquisitionMinimalEntries(parentKey);
	 	break;
	 case 5:
	 	entries = getDatasetMinimalEntries           (parentKey);
	 	break;										 
	 case 6:										 
	 	entries = getProcessingDatasetMinimalEntries (parentKey);
	 	break;
	default:
		qDebug()<<pi_uiLevel<<" is not an accurate level";
		break;
	}
	return entries;
}

QList<QMap<QString, QString>> SyncNetwork::getMandatoryAttributes(unsigned int pi_uiLevel, QString parentKey)
{
	QList<QMap<QString, QString>> mandatories;

	QList<levelMinimalEntries> entries = getMinimalEntries(pi_uiLevel, parentKey);
	
	for(const levelMinimalEntries &entry : entries)
	{
		QString type = entry.type==entryType::folder?"folder":"dataset";
		QMap<QString, QString> mandatory ={{"key",entry.key}, {"name", entry.name}, {"description" , entry.description}, {"type", type}};

		mandatories.append(mandatory);
	}
	return mandatories;
}

QVariant SyncNetwork::getDirectData(unsigned int pi_uiLevel, QString key)
{
	QVariant pathRes;

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

		// Perform request
		QNetworkRequest req;
		writeNiftiDatasetRetrievingRequest(req, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), id_ds);
		QUuid netReqId = waitGetResult(getData, req);

		// receiving the response
		RequestResponse res = m_requestMap[netReqId].response;

		// checking that the response is satisfactory
		if (res.payload.size() < 100)
		{
			// The request is not the good one : it must be done again with a conversion index added in query parameter
			writeNiftiDatasetRetrievingRequest(req, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), id_ds, true);
			netReqId = waitGetResult(getData, req);
			res = m_requestMap[netReqId].response;
		}

		pathRes = decompressNiftiiFromRequest(m_info->getStoragePath() + QString::number(id_ds) + "/", res.headers, res.payload, m_filesToRemove, 5000); //5 seconds before deletion
		if(pathRes.type() == QVariant::String)
		{// everything went well, we receive the corresponding path
		}
		else if(pathRes.type() == QVariant::Int)
		{
			qDebug()<<"DECOMPRESSION ERROR "<<pathRes.toInt();
		}

		manageRequestDeath(netReqId);

	}

	return  pathRes;
}

bool SyncNetwork::addDirectData(QVariant data, levelMinimalEntries & pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey)
{
	bool success = false;
	// checking that the level is appropriate for a processed dataset creation
	int parent_level = pi_uiLevel - 1;
	QString path = data.toString(); 
	QStringList parts = parentKey.split('.');
	bool psing_dataset_level = parent_level == 5 && parts.size() == 6;

	if(psing_dataset_level && path.endsWith(".nii.gz"))
	{
		QString filepath = path;
		QString distant_path;
		// Two steps : sending the dataset, THEN sending the context 

		//                    1st step  : sending the dataset 
		// construction of the processed dataset to upload
		QFile *file = new QFile(filepath);
		file->open(QIODevice::ReadOnly);
		QFileInfo fileInfo(filepath);

		if (fileInfo.exists())
		{
			// construction of the request
			QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
			QNetworkRequest req;
			writeProcessedDatasetUploadRequest(req, multiPart, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), file, fileInfo);

			// sending the request
			QUuid netReqId = waitPostMultiResult(addDataFile, req, multiPart);

			//  receiving the response
			RequestResponse res = m_requestMap[netReqId].response;
			m_requestMap.remove(netReqId);

			// handling the response
			if (res.code == SUCCESS_CODE && !res.payload.isNull())
			{
				distant_path = QString::fromUtf8(res.payload);
			}
			manageRequestDeath(netReqId);
		}

		//                    2nd step : sending the context 

		if(distant_path.length()>0)
		{
			int psingId = parts[5].toInt();
			int dsId = parts[4].toInt();
			// building the context of the processed dataset
			DatasetDetails ds_details = getDatasetDetails(dsId);
			StudyDetails s =  getStudyDetails(ds_details.study_id);
			StudyOverview study = { s.id, s.name };
 			QString processedDatasetType = "RECONSTRUCTEDDATASET"; 
			QString processedDatasetName = pio_minimalEntries.name;
			QJsonObject parentDatasetProcessing = getDatasetProcessing(psingId);

			// construction of the request
			QNetworkRequest req;
			QByteArray postData;
			writeProcessedDatasetUploadContextRequest(req, postData, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), ds_details, study, distant_path, processedDatasetType, processedDatasetName, parentDatasetProcessing);

			// sending the request
			QUuid netReqId = waitPostResult(addDataContext, req, postData);

			//  receiving the response
			RequestResponse res = m_requestMap[netReqId].response;

			manageRequestDeath(netReqId);

			success = res.code == SUCCESS_CODE;
		}
	}

	return success;
}

bool SyncNetwork::createFolder(levelMinimalEntries & pio_minimalEntries, datasetAttributes const & pi_attributes, unsigned int pi_uiLevel, QString parentKey)
{
	// checking that the level is appropriate for processing dataset creation
	int parent_level = pi_uiLevel - 1;
 	QStringList parts = parentKey.split('.');
 	bool dataset_level = parent_level == 4 && parts.size() == 5; 
 	bool success = false;

	

	if (dataset_level)
	{   
		// construction of the processing dataset to upload
 		int studyId = parts[0].toInt();
 		int dsId = parts[4].toInt();
 		DatasetDetails details = getDatasetDetails(dsId);
 		DatasetOverview input_dataset = { details.id, details.name, details.type };
 		QString processingDate = QDate::currentDate().toString("yyyy-MM-dd"); // on the web version, the user indicates this date himself
 		QString processingType = "SEGMENTATION"; //TODO: It is written in raw, but normaly there is a list, and the user chooses 1 valid processing type from that list -- think about using pi_attributes that could carry this information

 		DatasetProcessingOverview in_processing_ds = { -1, processingType, processingDate,QList<DatasetOverview>() << input_dataset, QList<DatasetOverview>(), studyId}; // -1 because it is not created

		// construction of the request 
		QNetworkRequest req;
		QByteArray postData;
		writeProcessingDatasetUploadRequest(req, postData, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), in_processing_ds, processingDate, studyId);

		// sending the request
		QUuid netReqId = waitPostResult(addFolder, req, postData);

		// receiving the response
		RequestResponse res = m_requestMap[netReqId].response;

		manageRequestDeath(netReqId);

		// handling the response
		success = res.code == SUCCESS_CODE;
	}

	return success;
}

QJsonObject SyncNetwork::applySolrRequest()
{
	// construction of the request
	QNetworkRequest req;
	QByteArray postData;
	writeGetSolrRequest(req, postData, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), "MR_DATASET");

	// sending the request
	QUuid netReqId = waitPostResult(searchWithSolr, req, postData);

	//  receiving the response
	RequestResponse res = m_requestMap[netReqId].response;
	manageRequestDeath(netReqId);

	return qbytearrayToQJson(res.payload);
}



void SyncNetwork::syncRequestSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode)
{
	if (m_requestMap.contains(netReqId))
	{
		RequestResponse res = { statusOrHttpCode, headers, payload };
		m_requestMap[netReqId].response = res;
		if (statusOrHttpCode == UPLOAD_CODE || statusOrHttpCode == DOWNLOAD_CODE)
		{ // Request in progress
			int bytesSent = headers["bytesSent"].toInt();
			int bytesTotal = headers["bytesTotal"].toInt();
			m_requestMap[netReqId].upToDate = true;
		}
		else
		{ // Request ended -- it can be an error
			m_requestMap[netReqId].waiter->exit();
		}
	}
}

void SyncNetwork::manageRequestDeath(QUuid netReqId)
{
	if (m_requestMap.contains(netReqId))
	{
		RequestResponse res = m_requestMap[netReqId].response;
		SyncRequestType type = m_requestMap[netReqId].type;
		int no_content_code = 204;
		bool isTreeViewRequest = type == getTreeViewDatasetProcessing || type == getTreeViewDatasetDetails || type == getTreeViewExaminations || type == getTreeViewStudies;
		if (res.code == no_content_code && isTreeViewRequest)
		{
			//204 stands for "no content found". 
			//It is not an error in a tree-view context, 
			// it only means that no children element is present in the parent. It is normal
			medNotif::createNotif(notifLevel::info , "SHANOIR PLUGIN", "This tree-view element has no children");
		}
		else if (res.code != SUCCESS_CODE)
		{
			// trace if an error occured
			qDebug() << "\nNETWORKERROR (code = " << res.code << ") - SYNC REQUEST TYPE ID : " << m_requestMap[netReqId].type << "\nLOOK AT https://doc.qt.io/qt-5/qnetworkreply.html#NetworkError-enum for more information\n";
			// notify
			QString activity;
			qDebug() << type;
			switch (type)
			{
			case searchWithSolr: activity = "searchWithSolr"; break;
			case getTreeViewDatasetProcessing: activity = "FETCHING Dataset Processing"; break;
			case getTreeViewDatasetDetails: activity = "FETCHING Dataset Details"; break;
			case getTreeViewStudyDetail: activity = "FETCHING StudyDetails Details"; break;
			case getTreeViewExaminations: activity = "FETCHING Examinations"; break;
			case getTreeViewStudies: activity = "FETCHING Studies"; break;
			case getData: activity = "DOWNLOADING DATA"; break;
			case addDataFile: activity = "ADDING A DATA FILE"; break;
			case addDataContext: activity = "SENDING A DATA CONTEXT"; break;
			case addFolder: activity = "CREATING A FOLDER"; break;
			default:    break;
			}
			QString errorMessage = "ERROR WHILE " + activity + "\n NETWORK ERROR CODE :" + QString::number(res.code);
			medNotif::createNotif(notifLevel::error, "SHANOIR PLUGIN", errorMessage);
		}
		m_requestMap.remove(netReqId);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--WAITERS-----////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QUuid SyncNetwork::waitGetResult(SyncRequestType type, QNetworkRequest &req)
{
	QUuid netReqId = QUuid::createUuid();

	QEventLoop *waiter = new QEventLoop(nullptr);
	m_requestMap[netReqId].waiter = waiter;
	m_requestMap[netReqId].type = type;
	m_requestMap[netReqId].upToDate = false;

	emit syncGet(netReqId, req);

	updateRequestState(type, netReqId);

	waiter->exec();
	waiter->deleteLater();

	return netReqId;
}

void SyncNetwork::updateRequestState(SyncRequestType &type, const QUuid &netReqId)
{
	// aborts the request if it did not finished after 1 second of wait
	int duration = requestsDurations[type];
	if (duration > 0)
	{
		QTimer::singleShot(duration, [this, netReqId]() {
			if (m_requestMap.contains(netReqId))
			{
				SyncRequest request = m_requestMap[netReqId];
				if (request.upToDate)
				{
					m_requestMap[netReqId].upToDate = false;
					updateRequestState(request.type, netReqId);
				}
				else if (request.response.code != SUCCESS_CODE)
				{
					emit syncAbort(netReqId);
				}
			}
		});
	}

}

QUuid SyncNetwork::waitPostResult(SyncRequestType type, QNetworkRequest &req, QByteArray &postData)
{
	QUuid netReqId = QUuid::createUuid();

	QEventLoop *waiter = new QEventLoop(nullptr);
	m_requestMap[netReqId].waiter = waiter;
	m_requestMap[netReqId].type = type;
	m_requestMap[netReqId].upToDate = false;

	emit syncPost(netReqId, req, postData);

	updateRequestState(type, netReqId);

	waiter->exec();
	waiter->deleteLater();

	return netReqId;
}

QUuid SyncNetwork::waitPostMultiResult(SyncRequestType type, QNetworkRequest &req, QHttpMultiPart  *postData)
{
	QUuid netReqId = QUuid::createUuid();

	QEventLoop *waiter = new QEventLoop(nullptr);
	m_requestMap[netReqId].waiter = waiter;
	m_requestMap[netReqId].type = type;
	m_requestMap[netReqId].upToDate = false;

	emit syncPostMulti(netReqId, req, postData);

	updateRequestState(type, netReqId);

	waiter->exec();
	waiter->deleteLater();

	return netReqId;
}

QUuid SyncNetwork::waitPutResult(SyncRequestType type, QNetworkRequest &req, QByteArray &postData)
{
	QUuid netReqId = QUuid::createUuid();

	QEventLoop *waiter = new QEventLoop(nullptr);
	m_requestMap[netReqId].waiter = waiter;
	m_requestMap[netReqId].type = type;
	m_requestMap[netReqId].upToDate = false;

	emit syncPut(netReqId, req, postData);

	updateRequestState(type, netReqId);

	waiter->exec();
	waiter->deleteLater();

	return netReqId;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--GETTERS-----////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


QList<StudyOverview>  SyncNetwork::getStudies()
{
	QList<StudyOverview> studies;

	// creation of the request
	QNetworkRequest req;
	writeGetStudiesRequest(req, m_info->getBaseURL(), m_authent->getCurrentAccessToken());

	// sending the request
	QUuid netReqId = waitGetResult(getTreeViewStudies, req);

	// receiving the response
	RequestResponse res = m_requestMap[netReqId].response;

	if (res.code == SUCCESS_CODE)
	{
		// handling the results : parsing
		QJsonArray studies_response = qbytearrayToQJsonArray(res.payload);
		studies = parseStudies(studies_response);
	}

	manageRequestDeath(netReqId);

	return studies;
}

StudyDetails SyncNetwork::getStudyDetails(int studyId)
{
	StudyDetails study;

	// creation of the request
	QNetworkRequest req;
	writeGetStudyRequest(req, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), studyId);

	// sending the request
	QUuid netReqId = waitGetResult(getTreeViewStudyDetail, req);

	// receiving the response
	RequestResponse res = m_requestMap[netReqId].response;

	if(res.code == SUCCESS_CODE)
	{
		// handling the results : parsing
		QJsonObject study_response = qbytearrayToQJson(res.payload);
		study = parseStudy(study_response);
	}

	manageRequestDeath(netReqId);

	return study;
}


QList<ExaminationOverview> SyncNetwork::getExaminations(int stud_id, int subj_id)
{
	QList<ExaminationOverview> examinations;

	// creation of the request
	QNetworkRequest req;
	writeGetExaminationsRequest(req, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), stud_id, subj_id);

	// sending the request
	QUuid netReqId = waitGetResult(getTreeViewExaminations, req);

	// receiving the response
	RequestResponse res = m_requestMap[netReqId].response;


	if(res.code == SUCCESS_CODE) 
	{
		// handling the results : parsing
		QJsonArray examinations_response = qbytearrayToQJsonArray(res.payload);
		examinations = parseExaminations(examinations_response);
	}

	manageRequestDeath(netReqId);

	return examinations;
}

DatasetDetails SyncNetwork::getDatasetDetails(int id)
{
	DatasetDetails dsDetails;

	// creation of the request
	QNetworkRequest req;
	writeGetDatasetRequest(req, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), id);

	// sending the request
	QUuid netReqId = waitGetResult(getTreeViewDatasetDetails, req);

	// receiving the response
	RequestResponse res = m_requestMap[netReqId].response;

	if(res.code == SUCCESS_CODE)
	{
		// handling the results (parsing the dataset details)
		QJsonObject dataset_response = qbytearrayToQJson(res.payload);
		dsDetails =  parseDatasetDetails(dataset_response);
	}

	manageRequestDeath(netReqId);

	return dsDetails;
}


QJsonObject SyncNetwork::getDatasetProcessing(int id)
{
	QJsonObject dsProcessing;
	
	// creation of the request
	QNetworkRequest req;
	writeGetDatasetProcessingRequest(req, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), id);


	// sending the request
	QUuid netReqId = waitGetResult(getTreeViewDatasetProcessing, req);

	// receiving the response
	RequestResponse res = m_requestMap[netReqId].response;

	if(res.code == SUCCESS_CODE)
	{
		dsProcessing =  qbytearrayToQJson(res.payload);
	}

	manageRequestDeath(netReqId);

	return dsProcessing;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--CASTERS-----////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



QList<levelMinimalEntries>  SyncNetwork::getRootMinimalEntries()
{
	QList<levelMinimalEntries> entries;
	QList<StudyOverview> studies = getStudies();
	for (StudyOverview study : studies)
	{
		QString description = study.name;
		levelMinimalEntries entry = { QString::number(study.id), study.name, description, entryTypeLocal::folder };
		entries.append(entry);
	}
	return entries;
}

QList<levelMinimalEntries>  SyncNetwork::getStudyMinimalEntries(QString id)
{
	QList<levelMinimalEntries> entries;
	StudyDetails study = getStudyDetails(id.toInt());
	for (SubjectOverview subject : study.subjects)
	{
		QString key = QString::number(study.id) + "." + QString::number(subject.id);
		QString description = subject.name + (subject.type.size() > 0 ? (" (" + subject.type + ")") : "");
		entries.append({ key, subject.name,  description, entryTypeLocal::folder });
	}
	return entries;
}

QList<levelMinimalEntries>  SyncNetwork::getSubjectMinimalEntries(QString id)
{
	QList<levelMinimalEntries> entries;

	QStringList parts = id.split('.');
	if (parts.size() == 2)
	{
		int id_study = parts[0].toInt();
		int id_subject = parts[1].toInt();
		QList<ExaminationOverview> examinations = getExaminations(id_study, id_subject);
		for (ExaminationOverview examination : examinations)
		{
			QString key = id + "." + QString::number(examination.id);
			QString name = examination.date.toString("dd/MM/yyyy") + ", " + examination.comment + "(id=" + QString::number(examination.id) + ")";
			QString description = "Contains " + QString::number(examination.ds_acquisitions.size()) + " dataset-acquisitions";
			entries.append({ key, name, description, entryTypeLocal::folder });
		}
	}
	return entries;
}

QList<levelMinimalEntries> SyncNetwork::getExaminationMinimalEntries(QString id)
{
	QList<levelMinimalEntries> entries;

	QStringList parts = id.split('.');
	if (parts.size() == 3)
	{
		int id_study = parts[0].toInt();
		int id_subject = parts[1].toInt();
		int id_exam = parts[2].toInt();
		QList<ExaminationOverview> examinations = getExaminations(id_study, id_subject);
		auto exam_it = findLevelElement(examinations, id_exam);
		if (exam_it!=nullptr && exam_it != examinations.end())
		{
			ExaminationOverview exam = *exam_it;
			QList<DatasetAcquisitionOverview> ds_acq = exam.ds_acquisitions;
			for (DatasetAcquisitionOverview acquisition : ds_acq)
			{
				QString key = id + "." + QString::number(acquisition.id);
				QString description = "Contains " + QString::number(acquisition.datasets.size()) + " datasets";
				entries.append({ key, acquisition.name, description, entryTypeLocal::folder });
			}
		}
	}

	return entries;
}

QList<levelMinimalEntries> SyncNetwork::getDatasetAcquisitionMinimalEntries(QString id)
{
	QList<levelMinimalEntries> entries;

	QStringList parts = id.split('.');
	if (parts.size() == 4)
	{
		int id_study = parts[0].toInt();
		int id_subject = parts[1].toInt();
		int id_exam = parts[2].toInt();
		int id_acq = parts[3].toInt();
		QList<ExaminationOverview> examinations = getExaminations(id_study, id_subject);
		auto exam_it = findLevelElement(examinations, id_exam);
		if (exam_it !=nullptr && exam_it != examinations.end())
		{
			ExaminationOverview exam = *exam_it;
			QList<DatasetAcquisitionOverview> ds_acquisitions = exam.ds_acquisitions;
			auto dsacq_it = findLevelElement(ds_acquisitions, id_acq);
			if (dsacq_it !=nullptr && dsacq_it != ds_acquisitions.end())
			{
				DatasetAcquisitionOverview ds_acq = *dsacq_it;
				QList<Dataset> datasets = ds_acq.datasets;
				for (Dataset ds : datasets)
				{
					QString key = id + "." + QString::number(ds.id);
					QString description = "Dataset " + QString::number(ds.id) + " of type " + ds.type + " with " + QString::number(ds.processings.size()) + " processings";
					entries.append({ key, ds.name, description, entryTypeLocal::both });
				}
			}
		}
	}

	return entries;
}

QList<levelMinimalEntries> SyncNetwork::getDatasetMinimalEntries(QString id)
{
	QList<levelMinimalEntries> entries;

	QStringList parts = id.split('.');
	if (parts.size() == 5)
	{
		int id_study = parts[0].toInt();
		int id_subject = parts[1].toInt();
		int id_exam = parts[2].toInt();
		int id_acq = parts[3].toInt();
		int id_ds = parts[4].toInt();
		QList<ExaminationOverview> examinations = getExaminations(id_study, id_subject);
		auto exam_it = findLevelElement(examinations, id_exam);
		if (exam_it !=nullptr && exam_it != examinations.end())
		{
			ExaminationOverview exam = *exam_it;
			QList<DatasetAcquisitionOverview> ds_acquisitions = exam.ds_acquisitions;
			auto dsacq_it = findLevelElement(ds_acquisitions, id_acq);
			if (dsacq_it !=nullptr && dsacq_it != ds_acquisitions.end())
			{
				DatasetAcquisitionOverview ds_acq = *dsacq_it;
				QList<Dataset> datasets = ds_acq.datasets;
				auto ds_it = findLevelElement(datasets, id_ds);
				if (ds_it !=nullptr && ds_it != datasets.end())
				{
					Dataset ds = *ds_it;
					QList<DatasetProcessingOverview> processings = ds.processings;
					for (auto processing : processings)
					{
						QString key = id + "." + QString::number(processing.id);
						QString description = "dataset processing with " + QString::number(processing.inputDatasets.size()) + " input datasets and " + QString::number(processing.outputDatasets.size()) + " output datasets";
						entries.append({ key, processing.type, description, entryTypeLocal::folder });
					}
				}
			}
		}
	}

	return entries;
}

QList<levelMinimalEntries> SyncNetwork::getProcessingDatasetMinimalEntries(QString id)
{
	QList<levelMinimalEntries> entries;

	QStringList parts = id.split('.');
	if (parts.size() == 6)
	{
		int id_study = parts[0].toInt();
		int id_subject = parts[1].toInt();
		int id_exam = parts[2].toInt();
		int id_acq = parts[3].toInt();
		int id_ds = parts[4].toInt();
		int id_processing = parts[5].toInt();
		QList<ExaminationOverview> examinations = getExaminations(id_study, id_subject);
		auto exam_it = findLevelElement(examinations, id_exam);
		if (exam_it !=nullptr && exam_it != examinations.end())
		{
			ExaminationOverview exam = *exam_it;
			QList<DatasetAcquisitionOverview> ds_acquisitions = exam.ds_acquisitions;
			auto dsacq_it = findLevelElement(ds_acquisitions, id_acq);
			if (dsacq_it !=nullptr && dsacq_it != ds_acquisitions.end())
			{
				DatasetAcquisitionOverview ds_acq = *dsacq_it;
				QList<Dataset> datasets = ds_acq.datasets;
				auto ds_it = findLevelElement(datasets, id_ds);
				if (ds_it!=nullptr && ds_it != datasets.end())
				{
					Dataset ds = *ds_it;
					QList<DatasetProcessingOverview> processings = ds.processings;
					auto processing_it = findLevelElement(processings, id_processing);
					if (processing_it!=nullptr && processing_it != processings.end())
					{
						DatasetProcessingOverview processing = *processing_it;
						QList<DatasetOverview> outputDatasets = processing.outputDatasets;
						for (auto outputDataset : outputDatasets)
						{
							QString key = id + "." + QString::number(outputDataset.id);
							QString description = "processed dataset of type " + outputDataset.type;
							entries.append({ key, outputDataset.name, description, entryTypeLocal::dataset });
						}
					}
				}
			}
		}
	}

	return entries;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--PARSERS-----////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

StudyDetails SyncNetwork::parseStudy(QJsonObject &study_response)
{
	StudyDetails studyRes;

	if (verifyJsonKeys(study_response, { "subjectStudyList", "id", "name" }, { "Array", "Number", "String" }))
	{
		QJsonArray subjectlist = study_response.value("subjectStudyList").toArray();
		QList<SubjectOverview> subjects;
		for (const QJsonValue &value : subjectlist)
		{
			QJsonObject subject = value.toObject().value("subject").toObject();
			if (verifyJsonKeys(subject, { "id", "name" }, { "Number", "String" }))
			{
				int subj_id = subject.value("id").toInt();
				QString subj_name = subject.value("name").toString();
				QString type = value.toObject().value("subjectType").toString();
				subjects.append({ subj_id, subj_name, type });
			}
		}

		int studid = study_response.value("id").toInt();
		QString studname = study_response.value("name").toString();
		studyRes = { studid, studname, subjects };
	}

	return studyRes;
}

QList<StudyOverview> SyncNetwork::parseStudies(QJsonArray studies_response)
{
	QList<StudyOverview> studies;
	for (const QJsonValue& value : studies_response)
	{
		QJsonObject study_response = value.toObject();
		if (verifyJsonKeys(study_response, { "id", "name" }, { "Number", "String" }))
		{
			int id = study_response.value("id").toInt();
			QString name = study_response.value("name").toString();
			StudyOverview study = { id,name };
			studies.append(study);
		}
	}
	return studies;
}

QList<ExaminationOverview> SyncNetwork::parseExaminations(QJsonArray examinations_response)
{
	QList<ExaminationOverview> examinations;

	for (const QJsonValue& value : examinations_response)
	{ // EXAMINATIONS 
		QJsonObject examination_response = value.toObject();
		if (verifyJsonKeys(examination_response, { "datasetAcquisitions","id","comment","examinationDate" }, { "Array", "Number", "String", "String" }))
		{
			QJsonArray dataset_acquisitions_response = examination_response.value("datasetAcquisitions").toArray();
			QList<DatasetAcquisitionOverview> dataset_acquisitions = parseDatasetAcquisitions(dataset_acquisitions_response);
			int id = examination_response.value("id").toInt();
			QString comment = examination_response.value("comment").toString();
			QDate date = QDate::fromString(examination_response.value("examinationDate").toString(), "yyyy-MM-dd");
			examinations.append({ id, comment, date, dataset_acquisitions });
		}
	}

	return examinations;
}

QList<DatasetAcquisitionOverview> SyncNetwork::parseDatasetAcquisitions(QJsonArray dataset_acquisitions_response)
{
	QList<DatasetAcquisitionOverview> dataset_acquisitions;
	for (const QJsonValue &value : dataset_acquisitions_response)
	{ // DATASETS ACQUISITIONS
		QJsonObject dataset_acquisition = value.toObject();
		if (verifyJsonKeys(dataset_acquisition, { "datasets", "id", "name" }, { "Array", "Number", "String" }))
		{
			QJsonArray datasets_response = dataset_acquisition.value("datasets").toArray();
			QList<Dataset> datasets = parseDatasets(datasets_response);
			int id = dataset_acquisition.value("id").toInt();
			QString name = dataset_acquisition.value("name").toString();
			dataset_acquisitions.append({ id, name, datasets });
		}
	}
	return dataset_acquisitions;
}

QList<Dataset> SyncNetwork::parseDatasets(QJsonArray datasets_response)
{
	QList<Dataset> datasets;
	for (const QJsonValue &value : datasets_response)
	{ // DATASETS
		QJsonObject dataset_response = value.toObject();
		if (verifyJsonKeys(dataset_response, { "id", "name", "type", "processings" }, { "Number", "String", "String", "Array" }))
		{
			int id = dataset_response.value("id").toInt();
			QString name = dataset_response.value("name").toString();
			QString type = dataset_response.value("datasetAcquisition").toObject().value("type").toString();
			QJsonArray processings_response = dataset_response.value("processings").toArray();
			QList<DatasetProcessingOverview> processings = parseDatasetProcessings(processings_response);
			datasets.append({ id, name, type, processings });
		}
	}
	return datasets;
}

QList<DatasetProcessingOverview> SyncNetwork::parseDatasetProcessings(QJsonArray processings_response)
{
	QList<DatasetProcessingOverview> processings;
	for (const QJsonValue &value : processings_response)
	{ // DATASET PROCESSINGS
		QJsonObject processing_response = value.toObject();
		if (verifyJsonKeys(processing_response, { "id", "datasetProcessingType", "inputDatasets", "outputDatasets", "processingDate", "studyId" }, { "Number", "String", "Array", "Array", "String", "Number" }))
		{
			int id = processing_response.value("id").toInt();
			QString type = processing_response.value("datasetProcessingType").toString();
			QString date = processing_response.value("processingDate").toString();
			QJsonArray input_datasets_response = processing_response.value("inputDatasets").toArray();
			QList<DatasetOverview> input_datasets;
			for (auto input_dataset_response : input_datasets_response)
			{
				if (verifyJsonKeys(input_dataset_response.toObject(), { "id", "name", "type" }, { "Number", "String", "String" }))
				{
					QJsonObject input_dataset = input_dataset_response.toObject();
					int id = input_dataset.value("id").toInt();
					QString name = input_dataset.value("name").toString();
					QString type = input_dataset.value("type").toString();
					input_datasets.append({ id, name, type });
				}
			}
			QJsonArray output_datasets_response = processing_response.value("outputDatasets").toArray();
			QList<DatasetOverview> output_processed_datasets;
			for (auto output_dataset_response : output_datasets_response)
			{
				if (verifyJsonKeys(output_dataset_response.toObject(), { "id" }, { "Number" }))
				{
					QJsonObject output_dataset = output_dataset_response.toObject();
					int id = output_dataset.value("id").toInt();
					QString name = output_dataset.value("name").toString();
					QString type = output_dataset.value("type").toString();
					output_processed_datasets.append({ id, name, type });
				}
			}
			int study_id = processing_response.value("studyId").toInt();
			processings.append({ id, type, date, input_datasets, output_processed_datasets, study_id });
		}
	}
	return processings;
}

DatasetDetails SyncNetwork::parseDatasetDetails(QJsonObject &dataset_response)
{
	DatasetDetails dataset;
	if (verifyJsonKeys(dataset_response, { "id","studyId", "subjectId", "name", "type" }, { "Number", "Number", "Number" "String", "String" }))
	{
		int id = dataset_response.value("id").toInt();
		int studyId = dataset_response.value("studyId").toInt();
		int subjectId = dataset_response.value("subjectId").toInt();
		QString name = dataset_response.value("name").toString();
		QString type = dataset_response.value("type").toString();
		dataset = { id, name, type, studyId, subjectId };
		if (dataset_response.contains("datasetAcquisition") && dataset_response.value("datasetAcquisition").toObject().contains("examination") && dataset_response.value("datasetAcquisition").toObject().value("examination").toObject().contains("subject"))
		{
			dataset.subject_name = dataset_response.value("datasetAcquisition").toObject().value("examination").toObject().value("subject").toObject().value("name").toString();
		}
	}
	return dataset;
}