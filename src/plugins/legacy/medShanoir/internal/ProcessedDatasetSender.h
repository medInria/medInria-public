#include <DataSender.h>
/**
 * WORK IN PROGRESS FOR THIS CLASS. THINGS  WILL CHANGE SOON
*/
class ProcessedDatasetSender : public DataSender
{
private:
	QString m_filepath;
	Dataset m_dataset;
	QString m_processingDate;
	QString m_processingType;
	QString m_subjectName;
	StudyOverview m_study;
	ExportProcessedDataset m_processedDataset;
	QJsonObject m_datasetProcessing;

	bool m_success;

    QString sendProcessedDataset(QString filepath);
	
    bool sendProcessedDatasetContext(ExportProcessedDataset processedDataset, StudyOverview study, QString subjectName, QString datasetType, QJsonObject datasetProcessing);

public:
	ProcessedDatasetSender(int id, Authenticater & auth, StudyOverview study, QString subjectName, Dataset dataset, QString processingDate, QString processingType, ExportProcessedDataset processedDataset, QJsonObject datasetProcessing);

	void run();

	bool isSuccessful();

	
};
