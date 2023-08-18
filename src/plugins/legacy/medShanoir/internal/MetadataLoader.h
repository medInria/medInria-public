#pragma once

#include <Authenticater.h>
#include <Network.h>
#include <Levels.h>
#include <QList>

class MetadataLoader{
private:
	Authenticater& m_auth;
	Network & m_net;
	
	QByteArray basicGetRequest(QString url);

	QList<StudyOverview> parseStudies(QJsonArray studies_response);

	QList<DatasetProcessing> parseDatasetProcessings(QJsonArray processings_response);

	QList<Dataset> parseDatasets(QJsonArray datasets_response);

	QList<DatasetAcquisition> parseDatasetAcquisitions(QJsonArray dataset_acquisitions_response);

	QList<Examination> parseExaminations(QJsonArray examinations_response);
		
public:


    MetadataLoader(Authenticater & authenticater, Network & network);

    QList<StudyOverview>  getStudies();

	Study getStudyById(int id);

	QList<Examination> getExaminationsByStudySubjectId(int stud_id, int subj_id);

	DatasetDetails getDatasetById(int id);
};