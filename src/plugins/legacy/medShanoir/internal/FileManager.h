#pragma once
#include <QFile>
#include <QByteArray>
#include <QString>

class FileManager
{
private:

	/**
	 * @brief create folders for a file if they does not exist
	 * @param filename a filename with a relative or absolute path from a file that aims to be created
	 * @return true if folders needed to be created for that file
	 */
	static bool folderCreator(QString filename);

public:

    /**
	 * @param fileData the content of the file to save 
	 * @param filename the name of the file to save. It can contain a part of the path preceding it (absolute or relative)
	 * @return the absolute path of the save. Empty string if the save failed. 
	*/
    static QString saveFileData(const QByteArray& fileData, const QString& filename);

    /**
	 * @param zipPath the file path of the zip file
	 * @return the output folder where the files have been extracted. Empty string if something went wrong. 
	*/
    static QString extractZipFile(QString zipPath);
};
