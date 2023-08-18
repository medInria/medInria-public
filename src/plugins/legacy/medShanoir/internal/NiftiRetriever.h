#include <QString>

#include <DataRetriever.h>

class NiftiRetriever : public DataRetriever
{
private:
	int m_datasetId;
	int m_converterId;

public:

	NiftiRetriever(int datasetId, int converterId, int requestId, Authenticater & auth, QString storagePath);

	void run();
};