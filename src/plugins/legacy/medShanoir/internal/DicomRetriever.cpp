#include <FileManager.h>
#include <DicomRetriever.h>

DicomRetriever::DicomRetriever(int datasetId, int requestId, Authenticater & auth, QString storagePath) :DataRetriever(requestId, auth, storagePath), m_datasetId(datasetId)
{}

void DicomRetriever::run() 
{
    QByteArray data;
    QString filename;
    loadFile(m_datasetId, "format=dcm", data, filename);
    QString zippath =  FileManager::saveFileData(data, m_storagePath);
    QString folderpath = FileManager::extractZipFile(zippath);
}