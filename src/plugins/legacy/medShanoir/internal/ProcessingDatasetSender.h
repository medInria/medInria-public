#include <DataSender.h>

class ProcessingDatasetSender : public DataSender
{
private:
	QJsonObject m_response;
    DatasetProcessing m_dspsing;

	QJsonObject sendDatasetProcessing(int studyId, DatasetOverview dataset,  QString processingDate, QString processingType);
	
public:
	ProcessingDatasetSender(int id, Authenticater & auth, DatasetProcessing dspsing);

	void run();

	QJsonObject getResponse();
};
