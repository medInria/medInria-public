#pragma once
#include <QString>
#include <QByteArray>
#include <QObject>
#include <QThreadPool>
#include <QJsonObject>
#include <QList>
#include <QStandardPaths>
#include <QCoreApplication>

#include <Authenticater.h>
#include <Network.h>
#include <Levels.h>

class RequestManager : public QObject
{
	Q_OBJECT
private:

	Authenticater& m_auth;
	Network & m_net;

	const QString SHANOIR_FILES_FOLDER = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/" + QCoreApplication::organizationName() + "/" + QCoreApplication::applicationName()+"/shanoirfiles/";

	QByteArray basicGetRequest(QString url);

	QAtomicInteger<int> m_fetch_number;

	QThreadPool *m_threadPool; 


public :
	RequestManager(Authenticater & authenticater, Network & network);

	QList<StudyOverview>  getStudies();

	Study getStudyById(int id);

	QList<Examination> getExaminationsByStudySubjectId(int stud_id, int subj_id);


    QString loadDicomDataset(int dataset_id);

    QString loadNiftiDataset(int dataset_id,int converterId = 4);


	// temporary function, in order to understand the process of async requests
	QString getAsyncExample();

public slots:

	void responseFromFetch(int id,QJsonObject jsondata);

};

