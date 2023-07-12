#pragma once
#include <QFile>
#include <QByteArray>
#include <QString>

class FileManager
{
public:

    /**
	 * @param fileData the content of the file to save 
	 * @param filename the name of the file to save. It can contain a part of the path preceding it (absolute or relative)
	 * @return the absolute path of the save. Empty string if the save failed. 
	*/
    static QString saveFileData(const QByteArray& fileData, const QString& filename);

    /**
	 * @warning requires the command 7zip to be installed 
	 * @param zipPath the file path of the zip file
	 * @param extension an extension filename (jpg, png, nii, dcm, etc)
	 * @return the output folder where the files have been extracted. Empty string if something went wrong. 
	*/
    static QString extractZipFile(QString zipPath, QString extension);
};
