#pragma once
#include <QString>
#include <QByteArray>
#include <QObject>
#include <QThreadPool>
#include <QJsonObject>
#include <QList>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QAtomicInteger>
#include <QMap>

#include <Authenticater.h>
#include <Network.h>
#include <Levels.h>
#include <MetadataLoader.h>

class RequestManager : public QObject
{
	Q_OBJECT
private:

	Authenticater& m_auth;
	Network & m_net;
	MetadataLoader m_mloader;

	const QString SHANOIR_FILES_FOLDER = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/" + QCoreApplication::organizationName() + "/" + QCoreApplication::applicationName()+"/shanoirfiles/";


	QAtomicInteger<int> m_request_number;

	QThreadPool *m_threadPool; 

	QMap<int, QString> m_asyncResults;


public :
	RequestManager(Authenticater & authenticater, Network & network);

	~RequestManager();

	QList<StudyOverview>  getStudies();

	Study getStudyById(int id);

	QList<Examination> getExaminationsByStudySubjectId(int stud_id, int subj_id);

	DatasetDetails getDatasetById(int id);

    QString loadDicomDataset(int dataset_id);

    QString loadNiftiDataset(int dataset_id, int converterId = 4);

	int loadAsyncNiftiDataset(int dataset_id, int converterId = 4);

	QString getAsyncResult(int requestId);

	QJsonObject createProcessingDataset(DatasetProcessing in_dspsing);

	//TODO: void createAsyncProcessingDataset(DatasetProcessing in_dspsing);

	bool sendProcessedDataset(int datasetId, QString processingDate, QString processingType, ExportProcessedDataset processedDataset, QJsonObject datasetProcessing);

	void sendProcessedDatasetAsync(int datasetId, QString processingDate, QString processingType, ExportProcessedDataset processedDataset);

signals:
	void loadedDataset(int idRequest);

	void sentProcessedDataset(int idRequest);

public slots:

	void datasetFinishedDownload(int id, QString data);

	void processedDatasetFinishedUpload(int id);

};

