#ifndef DATARETRIEVER_H
#define DATARETRIEVER_H
#include <QRunnable>
#include <QObject>
#include <QJsonObject>
#include <QString>
#include <Network.h>
#include <Authenticater.h>

class DataRetriever : public QObject, public QRunnable
{
	Q_OBJECT

protected:
	int m_id;
    Authenticater & m_auth;
	QString m_storagePath; // where to store the data
	QString m_dataPath; // where the data has been stored

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

public:
    DataRetriever(int id, Authenticater & auth, QString storagePath , QObject *parent = nullptr) ;

    virtual void run()=0;

	QString getDataPath();

	int getId();

signals:
	void startedRetrieving(int /*id*/);

	void dataRetrieved(int /*id*/, QString /*datapath*/);
	void failedRetrieving(int /*id*/);
};
#endif // DATARETRIEVER_H