#include <QString>

#include <DataSender.h>
#include <Levels.h>

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
	bool m_success;

public:
	ProcessedDatasetSender(int id, Authenticater & auth, StudyOverview study, QString subjectName, Dataset dataset, QString processingDate, QString processingType, ExportProcessedDataset processedDataset);

	void run();

	bool isSuccessful();
	
};
