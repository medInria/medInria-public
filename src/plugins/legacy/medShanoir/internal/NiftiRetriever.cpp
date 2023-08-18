#include <QByteArray>
#include <QDir>
#include <QStringList>

#include <FileManager.h>
#include "NiftiRetriever.h"

NiftiRetriever::NiftiRetriever(int datasetId, int converterId, int requestId, Authenticater & auth, QString storagePath) :DataRetriever(requestId, auth, storagePath), m_datasetId(datasetId), m_converterId(converterId)
{
}

void NiftiRetriever::run()
{
	QByteArray data;
	QString filename;
	loadFile(m_datasetId, "format=nii", data, filename);

	if(data.size()<100) // the zip received is empty
	{
		loadFile(m_datasetId,"format=nii&converterId="+QString::number(m_converterId), data, filename);
	}
	QString filepath = m_storagePath + filename;
	QString zippath =  FileManager::saveFileData(data, filepath);
	m_dataPath =  FileManager::extractZipFile(zippath);
	QDir folder(getDataPath());
	// Find the nifti file in the folder
	QStringList filters;
	filters << "*.nii" << "*.nii.gz";
	QStringList files = folder.entryList(filters, QDir::Files | QDir::NoDotAndDotDot);
	if (files.size() > 0)
	{
		m_dataPath = folder.absoluteFilePath(files[0]);
	}
	else
	{
		m_dataPath = "";
	}
	emit dataRetrieved(getId(), getDataPath());
}