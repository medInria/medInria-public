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

#include "RequestManager.h"


QByteArray RequestManager::basicGetRequest(QString url)
{
	QMap<QString, QString> headers {{"Authorization", "Bearer " + m_auth.getCurrentAccessToken()}};
	return m_net.httpGetFetch(url, headers)->readAll();
}



RequestManager::RequestManager(Authenticater & authenticater, Network & network)
	:m_auth(authenticater),m_net(network),m_threadPool(QThreadPool::globalInstance()),m_request_number(0)
{

}


QList<StudyOverview>  RequestManager::getStudies() 
{
	QString url = m_auth.getBaseURL()+ "studies/studies";
	QJsonArray studies_response = JsonReaderWriter::qbytearrayToQJsonArray(basicGetRequest(url));
	QList<StudyOverview> studies;
	for (const QJsonValue& value : studies_response) 
	{
		QJsonObject study_response = value.toObject();
		if(JsonReaderWriter::verifyKeys(study_response,{"id", "name"},{"Number", "String"}))
		{
			int id = study_response.value("id").toInt();
			QString name = study_response.value("name").toString();
			StudyOverview study = {id,name};
			studies.append(study);
		}
	}
	return studies;
}

Study RequestManager::getStudyById(int id)
{
	QString url = m_auth.getBaseURL()+ "studies/studies/" + QString::number(id);
	QJsonObject study_response = JsonReaderWriter::qbytearrayToQJson(basicGetRequest(url));
	Study study;
	if (JsonReaderWriter::verifyKeys(study_response, {"subjectStudyList", "id", "name"}, {"Array", "Number", "String"}))
	{
		QJsonArray subjectlist = study_response.value("subjectStudyList").toArray();
		QList<SubjectOverview> subjects;
		for (const QJsonValue &value : subjectlist)
		{
			QJsonObject subject = value.toObject().value("subject").toObject();
			if (JsonReaderWriter::verifyKeys(subject, {"id", "name"}, {"Number", "String"}))
			{
				int subj_id = subject.value("id").toInt();
				QString subj_name = subject.value("name").toString();
				QString type = value.toObject().value("subjectType").toString();
				subjects.append({subj_id, subj_name, type});
			}
		}
		// sorting subjects in order to have the same order as in the web version
		std::sort(subjects.begin(), subjects.end(), [](const SubjectOverview &a, const SubjectOverview &b)
				  { return QString::compare(a.name, b.name, Qt::CaseInsensitive) < 0; });
		int studid = study_response.value("id").toInt();
		QString studname = study_response.value("name").toString();
		study = {studid, studname, subjects};
	}
	return study;
}

QList<Examination> RequestManager::getExaminationsByStudySubjectId(int stud_id, int subj_id)
{
	QString url = m_auth.getBaseURL()+ "datasets/examinations/subject/"+QString::number(subj_id)+"/study/" + QString::number(stud_id);
	QJsonArray examinations_response = JsonReaderWriter::qbytearrayToQJsonArray(basicGetRequest(url));
	QList<Examination> examinations;
	for (const QJsonValue& value : examinations_response)
	{ // EXAMINATIONS 
		QJsonObject examination_response = value.toObject();
		if (JsonReaderWriter::verifyKeys(examination_response, {"datasetAcquisitions","id","comment","examinationDate"}, {"Array", "Number", "String", "String"}))
		{
			QJsonArray dataset_acquisitions_response = examination_response.value("datasetAcquisitions").toArray();
			QList<DatasetAcquisition> dataset_acquisitions;
			for (const QJsonValue &value : dataset_acquisitions_response)
			{ // DATASETS ACQUISITIONS
				QJsonObject dataset_acquisition = value.toObject();
				if(JsonReaderWriter::verifyKeys(dataset_acquisition, {"datasets","id","name"}, {"Array", "Number", "String"}))
				{
					QJsonArray datasets_response = dataset_acquisition.value("datasets").toArray();
					QList<Dataset> datasets;
					for (const QJsonValue &value : datasets_response)
					{ // DATASETS
						QJsonObject dataset_response = value.toObject();
						if(JsonReaderWriter::verifyKeys(dataset_response,{"id","name"}, {"Number", "String"}))
						{ 
							int id = dataset_response.value("id").toInt();
							QString name = dataset_response.value("name").toString();
							datasets.append({id, name});
						}
					}
					int id = dataset_acquisition.value("id").toInt();
					QString name = dataset_acquisition.value("name").toString();
					dataset_acquisitions.append({id, name, datasets});
				}
			}
			int id = examination_response.value("id").toInt();
			QString comment = examination_response.value("comment").toString();
			QDate date = QDate::fromString(examination_response.value("examinationDate").toString(), "yyyy-MM-dd");
			examinations.append({id, comment, date, dataset_acquisitions});
		}
	}
	return examinations;
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
		int size = data.size();

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
	QObject::connect(niftiretriever, &DataRetriever::dataRetrieved, this, &RequestManager::dataResponseHandling);
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


void RequestManager::dataResponseHandling(int id, QString data)
{
	m_asyncResults.insert(id, data);
	emit loadedDataset(id);
}
