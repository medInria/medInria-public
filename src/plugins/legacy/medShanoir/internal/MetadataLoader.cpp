#include <QJsonArray>
#include <QNetworkReply>
#include <JsonReaderWriter.h>

#include "MetadataLoader.h"


QByteArray MetadataLoader::basicGetRequest(QString url)
{
	QMap<QString, QString> headers {{"Authorization", "Bearer " + m_auth.getCurrentAccessToken()}};
	return m_net.httpGetFetch(url, headers)->readAll();
}

MetadataLoader::MetadataLoader(Authenticater & authenticater, Network & network) 
    :   m_auth(authenticater), m_net(network)
{
}


QList<StudyOverview>  MetadataLoader::getStudies() 
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


Study MetadataLoader::getStudyById(int id)
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

QList<Examination> MetadataLoader::getExaminationsByStudySubjectId(int stud_id, int subj_id)
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
						if(JsonReaderWriter::verifyKeys(dataset_response,{"id","name", "type", "processings"}, {"Number", "String", "String", "Array"}))
						{ 
							int id = dataset_response.value("id").toInt();
							QString name = dataset_response.value("name").toString();
							QString type = dataset_response.value("datasetAcquisition").toObject().value("type").toString();
							QJsonArray processings_response = dataset_response.value("processings").toArray(); 
							QList<DatasetProcessing> processings;
							for (const QJsonValue &value : processings_response)
							{ // DATASET PROCESSINGS
								QJsonObject processing_response = value.toObject();
								if(JsonReaderWriter::verifyKeys(processing_response,{"id","datasetProcessingType", "inputDatasets", "outputDatasets", "processingDate", "studyId" }, {"Number", "String", "Array", "Array", "String", "Number"}))
								{
									int id = processing_response.value("id").toInt();
									QString type = processing_response.value("datasetProcessingType").toString();
									QString date = processing_response.value("processingDate").toString();
									QJsonArray input_datasets_response = processing_response.value("inputDatasets").toArray();
									QList<int> input_datasets_ids;
									for(auto input_dataset : input_datasets_response)
									{
										if(JsonReaderWriter::verifyKeys(input_dataset.toObject(),{"id"}, {"Number"}))
										{
											input_datasets_ids.append(input_dataset.toObject().value("id").toInt());
										}
									}
									QJsonArray output_datasets_response = processing_response.value("outputDatasets").toArray();
									QList<ProcessedDataset> output_processed_datasets;
									for(auto output_dataset_response : output_datasets_response)
									{
										if(JsonReaderWriter::verifyKeys(output_dataset_response.toObject(),{"id"}, {"Number"}))
										{
											QJsonObject output_dataset = output_dataset_response.toObject();
											int id = output_dataset.value("id").toInt();
											QString name = output_dataset.value("name").toString();
											QString type = output_dataset.value("type").toString();
											output_processed_datasets.append({id, name, type});
										}
									}
									int study_id = processing_response.value("studyId").toInt();
									processings.append({id, type, date, input_datasets_ids, output_processed_datasets, study_id});
								}
							}
							datasets.append({id, name, type, processings});
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

DatasetDetails MetadataLoader::getDatasetById(int id)
{
	QString url = m_auth.getBaseURL()+ "datasets/datasets/" + QString::number(id);
	QJsonObject dataset_response = JsonReaderWriter::qbytearrayToQJson(basicGetRequest(url));
	DatasetDetails dataset;
	if (JsonReaderWriter::verifyKeys(dataset_response, {"id","studyId", "subjectId", "name", "type"}, {"Number", "Number", "Number" "String", "String"}))
	{
		int id = dataset_response.value("id").toInt();
		int studyId = dataset_response.value("studyId").toInt();
		int subjectId = dataset_response.value("subjectId").toInt();
		QString name = dataset_response.value("name").toString();
		QString type = dataset_response.value("type").toString();
		dataset = {id, name, type, studyId, subjectId};
		if(dataset_response.contains("datasetAcquisition") && dataset_response.value("datasetAcquisition").toObject().contains("examination") && dataset_response.value("datasetAcquisition").toObject().value("examination").toObject().contains("subject"))
		{
			dataset.subject_name = dataset_response.value("datasetAcquisition").toObject().value("examination").toObject().value("subject").toObject().value("name").toString();
		}
	}
	return dataset;
}