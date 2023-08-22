#include <DataSender.h>


class ProcessedDatasetSender : public DataSender
{
private:
	QString m_filepath;
	DatasetDetails m_dsDetails;
	// Dataset m_dataset;
	// QString m_subjectName;
	// int m_subjectid;
	StudyOverview m_study;
	ExportProcessedDataset m_processedDataset;
	QJsonObject m_datasetProcessing;

	bool m_success;

    QString sendProcessedDataset(QString filepath);
	
    bool sendProcessedDatasetContext(ExportProcessedDataset processedDataset, StudyOverview study, QString subjectName, int subjectId, QString datasetType, QJsonObject datasetProcessing);

public:
	ProcessedDatasetSender(int id, Authenticater & auth, StudyOverview study, DatasetDetails ds_details, ExportProcessedDataset processedDataset, QJsonObject datasetProcessing);

	void run();

	bool isSuccessful();

	
};
