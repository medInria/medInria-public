#include <QString>

#include <DataRetriever.h>

class DicomRetriever : public DataRetriever
{
private:
	int m_datasetId;
public:
	DicomRetriever(int datasetId, int requestId, Authenticater & auth, QString storagePath);

	void run() ;
};