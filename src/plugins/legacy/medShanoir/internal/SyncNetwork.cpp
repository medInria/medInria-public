#include <QJsonObject>
#include <QHttpMultiPart>

#include <Authenticator.h>
#include <medShanoir.h>
#include <RequestManager.h>
#include <LocalInfo.h>
#include <JsonHelper.h>
#include <FileHelper.h>
#include <ShanoirRequestPreparation.h>

#include "SyncNetwork.h"

SyncNetwork::SyncNetwork(medShanoir * parent,  LocalInfo *info, Authenticator * authent, RequestManager * requester) :QObject(parent), m_info(info), m_authent(authent)
{
	QObject::connect(this, &SyncNetwork::syncGet, requester, &RequestManager::httpGet, Qt::ConnectionType::QueuedConnection);
	QObject::connect(this, &SyncNetwork::syncPost, requester, &RequestManager::httpPost, Qt::ConnectionType::QueuedConnection);
	QObject::connect(this, &SyncNetwork::syncPostMulti, requester, &RequestManager::httpPostMulti, Qt::ConnectionType::QueuedConnection);
	QObject::connect(this, &SyncNetwork::syncPut, requester, &RequestManager::httpPut, Qt::ConnectionType::QueuedConnection);
}

SyncNetwork::~SyncNetwork()
{
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
		QUuid netReqId = waitGetResult(req);

		// receiving the response
		RequestResponse res = m_requestMap[netReqId].second;
		m_requestMap.remove(netReqId);

		//TODO: there is a lot of code in common with the method dataToFile of AsyncNetwork -> mind about a function in order to avoid code duplication;

		// checking that the response is satisfactory
		if (res.payload.size() < 100)
		{
			// The request is not the good one : it must be done again with a conversion index added in query parameter
			writeNiftiDatasetRetrievingRequest(req, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), id_ds, true);
			netReqId = waitGetResult(req);
			res = m_requestMap[netReqId].second;
			m_requestMap.remove(netReqId);
		}


		QString fileName;

		if (verifyJsonKeys(res.headers, { "Content-Disposition" }))
		{
			fileName = res.headers.value("Content-Disposition").toString().split("filename=")[1].split(";").first();
		}

		if (!fileName.isEmpty())
		{
			QString filePath = m_info->getStoragePath() + QString::number(id_ds) + "/" + fileName;
			QString zipPath = saveFileData(res.payload, filePath);
			QString extractionPath = extractZipFile(zipPath);
			QDir folder(extractionPath);

			// Find the nifti file in the folder
			QStringList filters;
			filters << "*.nii" << "*.nii.gz";
			QStringList files = folder.entryList(filters, QDir::Files | QDir::NoDotAndDotDot);
			if (files.size() > 0)
			{
				pathRes = folder.absoluteFilePath(files[0]);

				m_requestMap.remove(netReqId);

				m_filesToRemove.push_back(QPair<qint64, QString>({ QDateTime::currentSecsSinceEpoch(), extractionPath }));
				//TODO: delete the file from the filesystem in a reasonnable time
			}
			else
			{
				//TODO: trace
			}
		}
		else
		{
			//TODO: trace
		}
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
			QUuid netReqId = waitPostMultiResult(req, multiPart);

			//  receiving the response
			RequestResponse res = m_requestMap[netReqId].second;
			m_requestMap.remove(netReqId);

			// handling the response
			if (res.code == 200 && !res.payload.isNull())
			{
				distant_path = QString::fromUtf8(res.payload);
			}
		}

		//                    2nd step : sending the context 

		if(distant_path.length()>0)
		{
			int psingId = parts[5].toInt();
			int dsId = parts[4].toInt();
			// building the context of the processed dataset
			DatasetDetails ds_details = getDatasetDetails(dsId);
			Study s =  getStudyDetails(ds_details.study_id);
			StudyOverview study = { s.id, s.name };
 			QString processedDatasetType = "RECONSTRUCTEDDATASET"; 
			QString processedDatasetName = pio_minimalEntries.name;
			QJsonObject parentDatasetProcessing = getDatasetProcessing(psingId);

			// construction of the request
			QNetworkRequest req;
			QByteArray postData;
			writeProcessedDatasetUploadContextRequest(req, postData, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), ds_details, study, distant_path, processedDatasetType, processedDatasetName, parentDatasetProcessing);

			// sending the request
			QUuid netReqId = waitPostResult(req, postData);

			//  receiving the response
			RequestResponse res = m_requestMap[netReqId].second;
			m_requestMap.remove(netReqId);

			success = res.code == 200;
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
 		QString processingType = "SEGMENTATION"; //TODO: It is written in raw, but normaly there is a list, and the user chooses 1 valid processing type from that list
 		DatasetProcessing in_processing_ds = { -1, processingType, processingDate,QList<DatasetOverview>() << input_dataset, QList<ProcessedDataset>(), studyId}; // -1 because it is not created

		// construction of the request 
		QNetworkRequest req;
		QByteArray postData;
		writeProcessingDatasetUploadRequest(req, postData, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), in_processing_ds, processingDate, studyId);

		// sending the request
		QUuid netReqId = waitPostResult(req, postData);

		// receiving the response
		RequestResponse res = m_requestMap[netReqId].second;
		m_requestMap.remove(netReqId);

		// handling the response
		success = res.code == 200;
	}

	return success;
}


void SyncNetwork::syncGetSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode)
{
	syncPostSlot(netReqId, payload, headers, statusOrHttpCode);
}

void SyncNetwork::syncPostSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode)
{
	if (m_requestMap.contains(netReqId))
	{
		if (statusOrHttpCode == 0)
		{ // The request failed

		}
		if (statusOrHttpCode < 100)
		{ // Request in progress
			int bytesSent = headers["bytesSent"].toInt();
			int bytesTotal = headers["bytesTotal"].toInt();
		}
		else
		{ // Request ended
			RequestResponse res = { statusOrHttpCode, headers, payload};
			m_requestMap[netReqId].second = res;
			m_requestMap[netReqId].first->exit();
		}
	}
}

void SyncNetwork::syncPutSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int statusOrHttpCode)
{
	syncPostSlot(netReqId, payload, headers, statusOrHttpCode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--WAITERS-----////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QUuid SyncNetwork::waitGetResult(QNetworkRequest &req)
{
	QUuid netReqId = QUuid::createUuid();

	QEventLoop *waiter = new QEventLoop(nullptr);
	m_requestMap[netReqId].first = waiter;

	emit syncGet(netReqId, req);

	waiter->exec();
	waiter->deleteLater();

	return netReqId;
}

QUuid SyncNetwork::waitPostResult(QNetworkRequest &req, QByteArray &postData)
{
	QUuid netReqId = QUuid::createUuid();

	QEventLoop *waiter = new QEventLoop(nullptr);
	m_requestMap[netReqId].first = waiter;

	emit syncPost(netReqId, req, postData);

	waiter->exec();
	waiter->deleteLater();

	return netReqId;
}

QUuid SyncNetwork::waitPostMultiResult(QNetworkRequest &req, QHttpMultiPart  *postData)
{
	QUuid netReqId = QUuid::createUuid();

	QEventLoop *waiter = new QEventLoop(nullptr);
	m_requestMap[netReqId].first = waiter;

	emit syncPostMulti(netReqId, req, postData);

	waiter->exec();
	waiter->deleteLater();

	return netReqId;
}

QUuid SyncNetwork::waitPutResult(QNetworkRequest &req, QByteArray &postData)
{
	QUuid netReqId = QUuid::createUuid();

	QEventLoop *waiter = new QEventLoop(nullptr);
	m_requestMap[netReqId].first = waiter;

	emit syncPut(netReqId, req, postData);

	waiter->exec();
	waiter->deleteLater();

	return netReqId;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--GETTERS-----////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


QList<StudyOverview>  SyncNetwork::getStudies()
{
	// creation of the request
	QNetworkRequest req;
	writeGetStudiesRequest(req, m_info->getBaseURL(), m_authent->getCurrentAccessToken());

	// sending the request
	QUuid netReqId = waitGetResult(req);

	// receiving the response
	RequestResponse res = m_requestMap[netReqId].second;
	m_requestMap.remove(netReqId);

	// handling the results : parsing
	QJsonArray studies_response = qbytearrayToQJsonArray(res.payload);

	return parseStudies(studies_response);
}

Study SyncNetwork::getStudyDetails(int studyId)
{
	// creation of the request
	QNetworkRequest req;
	writeGetStudyDetailsRequest(req, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), studyId);

	// sending the request
	QUuid netReqId = waitGetResult(req);

	// receiving the response
	RequestResponse res = m_requestMap[netReqId].second;
	m_requestMap.remove(netReqId);

	// handling the results : parsing
	QJsonObject study_response = qbytearrayToQJson(res.payload);

	return parseStudy(study_response);
}


QList<Examination> SyncNetwork::getExaminations(int stud_id, int subj_id)
{
	QList<Examination> examinations;

	// creation of the request
	QNetworkRequest req;
	writeGetExaminationsRequest(req, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), stud_id, subj_id);

	// sending the request
	QUuid netReqId = waitGetResult(req);

	// receiving the response
	RequestResponse res = m_requestMap[netReqId].second;
	m_requestMap.remove(netReqId);

	// handling the results : parsing
	QJsonArray examinations_response = qbytearrayToQJsonArray(res.payload);

	return parseExaminations(examinations_response);
}

DatasetDetails SyncNetwork::getDatasetDetails(int id)
{

	// creation of the request
	QNetworkRequest req;
	writeGetDatasetDetailsRequest(req, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), id);

	// sending the request
	QUuid netReqId = waitGetResult(req);

	// receiving the response
	RequestResponse res = m_requestMap[netReqId].second;
	m_requestMap.remove(netReqId);

	// handling the results (parsing the dataset details)
	QJsonObject dataset_response = qbytearrayToQJson(res.payload);
	return parseDatasetDetails(dataset_response);
}


QJsonObject SyncNetwork::getDatasetProcessing(int id)
{
	// creation of the request
	QNetworkRequest req;
	writeGetDatasetProcessingRequest(req, m_info->getBaseURL(), m_authent->getCurrentAccessToken(), id);


	// sending the request
	QUuid netReqId = waitGetResult(req);

	// receiving the response
	RequestResponse res = m_requestMap[netReqId].second;
	m_requestMap.remove(netReqId);

	return qbytearrayToQJson(res.payload);
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
	Study study = getStudyDetails(id.toInt());
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
		QList<Examination> examinations = getExaminations(id_study, id_subject);
		for (Examination examination : examinations)
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
		QList<Examination> examinations = getExaminations(id_study, id_subject);
		auto exam_it = findLevelElement(examinations, id_exam);
		if (exam_it!=nullptr && exam_it != examinations.end())
		{
			Examination exam = *exam_it;
			QList<DatasetAcquisition> ds_acq = exam.ds_acquisitions;
			for (DatasetAcquisition acquisition : ds_acq)
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
		QList<Examination> examinations = getExaminations(id_study, id_subject);
		auto exam_it = findLevelElement(examinations, id_exam);
		if (exam_it !=nullptr && exam_it != examinations.end())
		{
			Examination exam = *exam_it;
			QList<DatasetAcquisition> ds_acquisitions = exam.ds_acquisitions;
			auto dsacq_it = findLevelElement(ds_acquisitions, id_acq);
			if (dsacq_it !=nullptr && dsacq_it != ds_acquisitions.end())
			{
				DatasetAcquisition ds_acq = *dsacq_it;
				QList<Dataset> datasets = ds_acq.datasets;
				for (Dataset ds : datasets)
				{
					QString key = id + "." + QString::number(ds.id);
					QString description = "Dataset n�" + QString::number(ds.id) + " of type " + ds.type + " with " + QString::number(ds.processings.size()) + " processings";
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
		QList<Examination> examinations = getExaminations(id_study, id_subject);
		auto exam_it = findLevelElement(examinations, id_exam);
		if (exam_it !=nullptr && exam_it != examinations.end())
		{
			Examination exam = *exam_it;
			QList<DatasetAcquisition> ds_acquisitions = exam.ds_acquisitions;
			auto dsacq_it = findLevelElement(ds_acquisitions, id_acq);
			if (dsacq_it !=nullptr && dsacq_it != ds_acquisitions.end())
			{
				DatasetAcquisition ds_acq = *dsacq_it;
				QList<Dataset> datasets = ds_acq.datasets;
				auto ds_it = findLevelElement(datasets, id_ds);
				if (ds_it !=nullptr && ds_it != datasets.end())
				{
					Dataset ds = *ds_it;
					QList<DatasetProcessing> processings = ds.processings;
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
		QList<Examination> examinations = getExaminations(id_study, id_subject);
		auto exam_it = findLevelElement(examinations, id_exam);
		if (exam_it !=nullptr && exam_it != examinations.end())
		{
			Examination exam = *exam_it;
			QList<DatasetAcquisition> ds_acquisitions = exam.ds_acquisitions;
			auto dsacq_it = findLevelElement(ds_acquisitions, id_acq);
			if (dsacq_it !=nullptr && dsacq_it != ds_acquisitions.end())
			{
				DatasetAcquisition ds_acq = *dsacq_it;
				QList<Dataset> datasets = ds_acq.datasets;
				auto ds_it = findLevelElement(datasets, id_ds);
				if (ds_it!=nullptr && ds_it != datasets.end())
				{
					Dataset ds = *ds_it;
					QList<DatasetProcessing> processings = ds.processings;
					auto processing_it = findLevelElement(processings, id_processing);
					if (processing_it!=nullptr && processing_it != processings.end())
					{
						DatasetProcessing processing = *processing_it;
						QList<ProcessedDataset> outputDatasets = processing.outputDatasets;
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

Study SyncNetwork::parseStudy(QJsonObject &study_response)
{
	Study studyRes;

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

QList<Examination> SyncNetwork::parseExaminations(QJsonArray examinations_response)
{
	QList<Examination> examinations;

	for (const QJsonValue& value : examinations_response)
	{ // EXAMINATIONS 
		QJsonObject examination_response = value.toObject();
		if (verifyJsonKeys(examination_response, { "datasetAcquisitions","id","comment","examinationDate" }, { "Array", "Number", "String", "String" }))
		{
			QJsonArray dataset_acquisitions_response = examination_response.value("datasetAcquisitions").toArray();
			QList<DatasetAcquisition> dataset_acquisitions = parseDatasetAcquisitions(dataset_acquisitions_response);
			int id = examination_response.value("id").toInt();
			QString comment = examination_response.value("comment").toString();
			QDate date = QDate::fromString(examination_response.value("examinationDate").toString(), "yyyy-MM-dd");
			examinations.append({ id, comment, date, dataset_acquisitions });
		}
	}

	return examinations;
}

QList<DatasetAcquisition> SyncNetwork::parseDatasetAcquisitions(QJsonArray dataset_acquisitions_response)
{
	QList<DatasetAcquisition> dataset_acquisitions;
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
			QList<DatasetProcessing> processings = parseDatasetProcessings(processings_response);
			datasets.append({ id, name, type, processings });
		}
	}
	return datasets;
}

QList<DatasetProcessing> SyncNetwork::parseDatasetProcessings(QJsonArray processings_response)
{
	QList<DatasetProcessing> processings;
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
			QList<ProcessedDataset> output_processed_datasets;
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