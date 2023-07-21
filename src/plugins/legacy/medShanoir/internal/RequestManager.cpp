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
#include <FetchGet.h>
#include <JsonReaderWriter.h>
#include <FileManager.h>

#include "RequestManager.h"


QByteArray RequestManager::basicGetRequest(QString url)
{
	QMap<QString, QString> headers {{"Authorization", "Bearer " + m_auth.getCurrentAccessToken()}};
	return m_net.httpGetFetch(url, headers)->readAll();
}



RequestManager::RequestManager(Authenticater & authenticater, Network & network)
	:m_auth(authenticater),m_net(network),m_threadPool(QThreadPool::globalInstance()),m_fetch_number(0)
{

}


QString RequestManager::getBaseURL() 
{
	return "https://"+ m_auth.getCurrentDomain()+"/shanoir-ng/";
}

QList<StudyOverview>  RequestManager::getStudies() 
{
	QString url = getBaseURL()+ "studies/studies";
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
	QString url = getBaseURL()+ "studies/studies/" + QString::number(id);
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
	QString url = getBaseURL()+ "datasets/examinations/subject/"+QString::number(subj_id)+"/study/" + QString::number(stud_id);
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


void RequestManager::loadFile(int dataset_id, QString query_string, QByteArray &fileData, QString &filename)
{
    QString url = getBaseURL() + "datasets/datasets/download/" + QString::number(dataset_id) + "?" + query_string;
    QNetworkReply *reply = m_net.httpGetFetch(url, {{"Authorization", "Bearer " + m_auth.getCurrentAccessToken()}});
    QJsonObject responseHeaders = m_net.replyHeaders(reply);
	if(JsonReaderWriter::verifyKeys(responseHeaders,{"Content-Disposition"}))
	{
    	filename = responseHeaders.value("Content-Disposition").toString().split("filename=")[1].split(";").first();
		if(filename.length()>0)
		{
    		fileData = reply->readAll();
		}
	}
}


QString RequestManager::loadDicomDataset(int dataset_id)
{
	QByteArray data;
	QString filename;
	loadFile(dataset_id, "format=dcm", data, filename);
	QString filepath =SHANOIR_FILES_FOLDER + QString::number(dataset_id) + "/" + filename;
	QString zippath =  FileManager::saveFileData(data, filepath);
	QString folderpath = FileManager::extractZipFile(zippath,"dcm");
	return folderpath;
}

QString RequestManager::loadNiftiDataset(int dataset_id, int converter_id)
{
	QByteArray data;
	QString filename;
	loadFile(dataset_id, "format=nii", data, filename);
	QString filepath = SHANOIR_FILES_FOLDER + QString::number(dataset_id) + "/" + filename;
	if(data.size()<100) // the zip received is empty
	{
		loadFile(dataset_id,"format=nii&converterId="+QString::number(converter_id), data, filepath);
	}
	QString zippath =  FileManager::saveFileData(data, filepath);
	QString folderpath =  FileManager::extractZipFile(zippath, "nii");
	return folderpath;
}


int RequestManager::getAsyncExample()
{
	int fetch_id = ++m_fetch_number;

	Fetch *f = new FetchGet(fetch_id, "https://apiexample/item");
    f->setAutoDelete(true);

	QObject::connect(f, &Fetch::fetchEnded, this, &RequestManager::responseFromFetch);

	m_threadPool->start(f);

	return fetch_id;
}



void RequestManager::responseFromFetch(int id, QJsonObject jsondata)
{
	qDebug() << "Data Received from fetch" << id << " size = "<< jsondata.size();
}
