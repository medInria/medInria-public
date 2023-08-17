#include <QMap>
#include <QJsonObject>
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
#include <DataRetriever.h>
#include <DataSender.h>

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

class DicomRetriever : public DataRetriever
{
private:
	int m_datasetId;
public:

	DicomRetriever(int datasetId, int requestId, Authenticater & auth, QString storagePath) :DataRetriever(requestId, auth, storagePath), m_datasetId(datasetId)
	{}

	void run() 
	{
		QByteArray data;
		QString filename;
		loadFile(m_datasetId, "format=dcm", data, filename);
		QString zippath =  FileManager::saveFileData(data, m_storagePath);
		QString folderpath = FileManager::extractZipFile(zippath);
	}
};


class NiftiRetriever : public DataRetriever
{
private:
	int m_datasetId;
	int m_converterId;

public:

	NiftiRetriever(int datasetId, int converterId, int requestId, Authenticater & auth, QString storagePath) :DataRetriever(requestId, auth, storagePath), m_datasetId(datasetId), m_converterId(converterId)
	{
	}

	void run()
	{
		QByteArray data;
		QString filename;
		loadFile(m_datasetId, "format=nii", data, filename);

		if(data.size()<100) // the zip received is empty
		{
			loadFile(m_datasetId,"format=nii&converterId="+QString::number(m_converterId), data, filename);
		}
		QString filepath = m_storagePath + filename;
		QString zippath =  FileManager::saveFileData(data, filepath);
		m_dataPath =  FileManager::extractZipFile(zippath);
		QDir folder(getDataPath());
		// Find the nifti file in the folder
		QStringList filters;
		filters << "*.nii" << "*.nii.gz";
		QStringList files = folder.entryList(filters, QDir::Files | QDir::NoDotAndDotDot);
		if (files.size() > 0)
		{
			m_dataPath = folder.absoluteFilePath(files[0]);
		}
		else
		{
			m_dataPath = "";
		}
		emit dataRetrieved(getId(), getDataPath());
	}
};


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


class ProcessedDatasetSender : public DataSender
{
private:
	QString m_filepath;
	Dataset m_dataset;
	QString m_processingDate;
	QString m_processingType;
	QString m_subjectName;
	StudyOverview m_study;
	ProcessedDataset m_processedDataset;

public:
	ProcessedDatasetSender(int id, Authenticater & auth, StudyOverview study, QString subjectName, Dataset dataset, QString processingDate, QString processingType, ProcessedDataset processedDataset) :DataSender(id, auth), m_study(study),  m_subjectName(subjectName), m_dataset(dataset), m_processingDate(processingDate), m_processingType(processingType), m_processedDataset(processedDataset)
	{
	}

	void ProcessedDatasetSender::run()
	{
		QString path = sendProcessedDataset(m_processedDataset.filepath);
		QJsonObject datasetProcessing =  sendDatasetProcessing(m_study.id, m_dataset, m_processingDate, m_processingType);
		m_processedDataset.filepath = path;
		bool success = sendProcessedDatasetContext(m_processedDataset, m_study, m_subjectName, m_dataset.type, datasetProcessing);
		emit dataSent(getId());
	}
	
};	
void RequestManager::sendProcessedDataset(int datasetId, QString processingDate, QString processingType, ProcessedDataset processedDataset)
{
	DatasetDetails ds_details = m_mloader.getDatasetById(datasetId);
	Dataset dataset = { ds_details.id, ds_details.name, ds_details.type };
	Study s =  m_mloader.getStudyById(ds_details.study_id);
	StudyOverview study = { s.id, s.name };
	QString subjectName = ds_details.subject_name;
	
	ProcessedDatasetSender pds(-1, m_auth, study, subjectName, dataset, processingDate, processingType, processedDataset);
	pds.run();
}

void RequestManager::processedDatasetFinishedUpload(int id)
{
	m_asyncResults.insert(id, "success");
	emit sentProcessedDataset(id);
}

void RequestManager::sendProcessedDatasetAsync(int datasetId, QString processingDate, QString processingType, ProcessedDataset processedDataset)
{
	int request_id = ++m_request_number;
	DatasetDetails ds_details =  m_mloader.getDatasetById(datasetId);
	Dataset dataset = { ds_details.id, ds_details.name, ds_details.type };
	Study s =  m_mloader.getStudyById(ds_details.study_id);
	StudyOverview study = { s.id, s.name };
	QString subjectName = ds_details.subject_name;

	ProcessedDatasetSender *pds = new ProcessedDatasetSender(request_id, m_auth, study, subjectName, dataset, processingDate, processingType, processedDataset);
	pds->setAutoDelete(true);
	QObject::connect(pds, &DataSender::dataSent, this, &RequestManager::processedDatasetFinishedUpload);
	m_threadPool->start(pds);
}

