#include <QByteArray>
#include <QDebug>
#include <QJsonArray>
#include <QThreadPool>
#include <QFile>
#include <QDir>
#include <QNetworkReply>
#include <QFileInfo>
#include <QDate>
#include <QProcess>

#include <Network.h>
#include <JsonReaderWriter.h>
#include <FileManager.h>
#include <DicomRetriever.h>
#include <NiftiRetriever.h>
#include <ProcessedDatasetSender.h>
#include <ProcessingDatasetSender.h>

#include "RequestManager.h"

RequestManager::RequestManager(Authenticater & authenticater, Network & network)
	:m_auth(authenticater),m_net(network), m_mloader(authenticater, network), m_threadPool(QThreadPool::globalInstance()),m_request_number(0)
{
}

RequestManager::~RequestManager()
{
	m_threadPool->waitForDone();
	m_threadPool->clear();
}

QList<StudyOverview>  RequestManager::getStudies() 
{
	return m_mloader.getStudies();
}

Study RequestManager::getStudyById(int id)
{
	return m_mloader.getStudyById(id);
}

QList<Examination> RequestManager::getExaminationsByStudySubjectId(int stud_id, int subj_id)
{
	return m_mloader.getExaminationsByStudySubjectId(stud_id, subj_id);
}

DatasetDetails RequestManager::getDatasetById(int id)
{
	return m_mloader.getDatasetById(id);
}

QJsonObject RequestManager::getDatasetProcessingById(int id)
{
	return m_mloader.getDatasetProcessingById(id);
}


QString RequestManager::loadDicomDataset(int dataset_id)
{
	QString filepath = SHANOIR_FILES_FOLDER + QString::number(dataset_id) + "/";
	DicomRetriever dr(dataset_id, -1, m_auth, filepath);
	dr.run();
	return dr.getDataPath();
}

QString RequestManager::loadNiftiDataset(int dataset_id, int converter_id)
{
	QString filepath = SHANOIR_FILES_FOLDER + QString::number(dataset_id) + "/";
	NiftiRetriever nr(dataset_id, converter_id, -1, m_auth, filepath);
	nr.run();
	return nr.getDataPath();
}


int RequestManager::loadAsyncNiftiDataset(int dataset_id, int converter_id)
{
	int request_id = ++m_request_number;
	QString filepath = SHANOIR_FILES_FOLDER + QString::number(dataset_id) + "/";
	DataRetriever *niftiretriever = new NiftiRetriever(dataset_id, converter_id, request_id, m_auth, filepath);
	niftiretriever->setAutoDelete(true);
	QObject::connect(niftiretriever, &DataRetriever::dataRetrieved, this, &RequestManager::datasetFinishedDownload);
	m_threadPool->start(niftiretriever);
	return request_id;
}


QString RequestManager::getAsyncResult(int requestId)
{
	QString result = "";
	if (m_asyncResults.contains(requestId))
	{
		result = m_asyncResults.value(requestId);
	//	m_asyncResults.remove(requestId);
	}
	return result;
}


void RequestManager::datasetFinishedDownload(int id, QString data)
{
	m_asyncResults.insert(id, data);
	emit loadedDataset(id);
}


QJsonObject RequestManager::createProcessingDataset(DatasetProcessing in_dspsing)
{
	ProcessingDatasetSender psing_sender(-1, m_auth, in_dspsing);
	psing_sender.run();	
	return psing_sender.getResponse();
}


bool RequestManager::sendProcessedDataset(int datasetId, ExportProcessedDataset processedDataset, QJsonObject datasetProcessing)
{
	DatasetDetails ds_details = m_mloader.getDatasetById(datasetId);
	Dataset dataset = { ds_details.id, ds_details.name, ds_details.type };
	Study s =  m_mloader.getStudyById(ds_details.study_id);
	StudyOverview study = { s.id, s.name };
	QString subjectName = ds_details.subject_name;
	
	ProcessedDatasetSender pds(-1, m_auth, study, subjectName, dataset, processedDataset, datasetProcessing);
	pds.run();
	return pds.isSuccessful();
}

void RequestManager::processedDatasetFinishedUpload(int id)
{
	m_asyncResults.insert(id, "success");
	emit sentProcessedDataset(id);
}

void RequestManager::sendProcessedDatasetAsync(int datasetId, QString processingDate, QString processingType, ExportProcessedDataset processedDataset)
{
	//int request_id = ++m_request_number;
	//DatasetDetails ds_details =  m_mloader.getDatasetById(datasetId);
	//Dataset dataset = { ds_details.id, ds_details.name, ds_details.type };
	//Study s =  m_mloader.getStudyById(ds_details.study_id);
	//StudyOverview study = { s.id, s.name };
	//QString subjectName = ds_details.subject_name;

	//ProcessedDatasetSender *pds = new ProcessedDatasetSender(request_id, m_auth, study, subjectName, dataset, processingDate, processingType, processedDataset);
	//pds->setAutoDelete(true);
	//QObject::connect(pds, &DataSender::dataSent, this, &RequestManager::processedDatasetFinishedUpload);
	//m_threadPool->start(pds);
}