#pragma once
#include <QString>
#include <QByteArray>
#include <QObject>
#include <QThreadPool>
#include <QJsonObject>
#include <QList>

#include <Authenticater.h>
#include <Network.h>
#include <Levels.h>

class RequestManager : public QObject
{
	Q_OBJECT
private:

	Authenticater& m_auth;
	Network & m_net;

	QByteArray basicGetRequest(QString url);

	QAtomicInteger<int> m_fetch_number;

	QString getBaseURL();

	/**
	 * This function requests a file, and downloads it if it is big enough.
	 * 
	 *  
	 * @param dataset_id the id of the dataset to request
	 * @param query_string the parameters that indicate the nature of the file to download (ex : "format=nii&converterId=4")
	 * @param folder_path the path of the folder that will contain the file. It can be relative, and will be created if it does not exist already.
	 * @param min_size the minimum size of the retrieved QByteArray in order to start the download. This prevents the download of files that don't contain enough data. 
	 * 
	 * @return the success of the download
	*/
	void loadFile(int dataset_id, QString query_string, QByteArray &fileData, QString &filename);



public :
	RequestManager(Authenticater & authenticater, Network & network);

	QList<StudyOverview>  getStudies();

	Study getStudyById(int id);

	QList<Examination> getExaminationsByStudySubjectId(int stud_id, int subj_id);


    QString loadDicomDataset(int dataset_id);

    QString loadNiftiDataset(int dataset_id,int converterId = 4);


	// temporary function, in order to understand the process of async requests
	int getAsyncExample();
	QThreadPool *m_threadPool; //TODO: switch back this variable to private when the async fetches will be well implemented 

public slots:

	void responseFromFetch(int id,QJsonObject jsondata);

};

