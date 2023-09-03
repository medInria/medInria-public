#ifndef FILE_HELPER
#define FILE_HELPER

#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <JsonHelper.h>

/**
 * @brief this file contains helper functions to deal with files and folders
 * (decompressing, deleting, etc.)
*/

/**
 * @param filepath the path of a file to create
 * @return true if a folder was created for this path to exist, false otherwise
*/
inline bool folderCreator(QString filepath)
{
    QString outputfolder = QFileInfo(filepath).absoluteDir().absolutePath();
    // create the path if it does not exist
    QDir dir(outputfolder);
    if (!dir.exists())
    {
        dir.mkpath(".");
        return true;
    }
    return false;
}

/**
 * @param fileData the data of a file to save
 * @param filepath the path (including the name) of the file to save. it can be relative or absolute, and may not exist yet
 * @return the absolute path of the file if it was created, an empty string otherwise
*/
inline QString saveFileData(const QByteArray &fileData, const QString &filepath)
{
    QFileInfo fileInfo = QFileInfo(filepath);
    QString absoluteFilePath = fileInfo.absoluteFilePath();
    QFile file(absoluteFilePath);
    // create the path if it does not exist
    bool creation = folderCreator(absoluteFilePath);
    // check if it is possible to write the file
    if (file.open(QIODevice::WriteOnly))
    {
        // write the file
        file.write(fileData);
        file.close();
    }
    else
    {
        if (creation)
        {
            // delete the containing folder if it was created for the occasion
            QDir().rmdir(absoluteFilePath);
        }
        absoluteFilePath = "";
    }
    return absoluteFilePath;
}


/**
 * @param zipPath the path of a zip file to extract
 * @return the path of the folder where the files were extracted if everything went well, an empty string otherwise
 * @note the zip file is deleted if the extraction was successful
 * @note the function uses the system unzipper (for linux windows and mac)
*/
inline QString extractZipFile(QString zipPath)
{
    QFileInfo fileInfo(zipPath);
    QString outputfolder = fileInfo.absoluteDir().absolutePath() + QDir::separator() + fileInfo.baseName();
    folderCreator(zipPath);

    // Extract files from a zip file using the system unzipper (for linux windows and mac)
    // check the OS and use the appropriate command
    QString command;
#ifdef Q_OS_WIN
    // using expand-Archive command
    command = "powershell.exe -command \"Expand-Archive -Path " + zipPath + " -DestinationPath " + outputfolder + "\"";
#elif defined(Q_OS_MACOS)
    // using unzip command
    command = "unzip " + zipPath + " -d " + outputfolder;
#elif defined(Q_OS_LINUX)
    // using unzip command
    command = "unzip " + zipPath + " -d " + outputfolder;
#endif
    QProcess process;
    process.start(command);
    process.waitForFinished(-1);

    int exitCode = process.exitCode();
    if (exitCode == 0)
    {
        qDebug().noquote().nospace() << "Files have been unzipped at" << QDir(outputfolder).absolutePath() << " directory";
        QFile::remove(fileInfo.absoluteFilePath());
    }
    else
    {
        outputfolder = "";
    }
    return outputfolder;
}

/**
 * @param prefix the prefix of the path where to decompress the file 
 * @param headers the headers of a request containing a filename
 * @param payload the data of a request containing a zip file
 * @param deletionDelay time (ms) when the decompressed folder will be deleted. -1 it should not be deleted
 * @return a QVariant that is a string of the filepath if everything went well. Otherwise it returns a int that is an error code
 * The possible error codes are :
 * +0 :no filename found in the headers
 * +1 :there is not any niftii file in the decompressed file
 */
inline QVariant decompressNiftiiFromRequest(QString prefix, QJsonObject headers, QByteArray payload, QStringList &pendingDeletions, int deletionDelay = 3000)
{
    QString fileName;
    if (verifyJsonKeys(headers, {"Content-Disposition"}))
    {
        fileName = headers.value("Content-Disposition").toString().split("filename=")[1].split(";").first();
    }

    if (!fileName.isEmpty())
    {
		QString filePath = prefix + fileName; 
        QString zipPath = saveFileData(payload, filePath);
        QString extractionPath = extractZipFile(zipPath);
        QDir folder(extractionPath);

        QStringList filters;
        filters << "*.nii" << "*.nii.gz";
        QStringList files = folder.entryList(filters, QDir::Files | QDir::NoDotAndDotDot);
        if (files.size() > 0)
        {
            if (deletionDelay != -1)
            {
                pendingDeletions.append(extractionPath); 
				qDebug()  << extractionPath << "WILL BE DELETED IN "<<deletionDelay<<" ms";
                // delete the folder containing the niftii from the filesystem in a reasonnable time
                QTimer::singleShot(deletionDelay, [extractionPath, &pendingDeletions]()
                { 
					if (QDir(extractionPath).removeRecursively())
					{
				   	pendingDeletions.removeAll(extractionPath);  
						qDebug() << "DELETION OF" << extractionPath << "SUCCESSFUL";
					}
					else
					{
						qDebug() << "DELETION OF" << extractionPath << "FAILED";
					}
				});
            }

            QString pathRes = folder.absoluteFilePath(files[0]);
            return pathRes;
        }
        else
        { // there is not any niftii file in the decompressed file
            return 1;
        }
    }
    else
    { // no filename found in the headers
        return 0;
    }
}

/**
 * @param folders a list of folders to delete
 * @return true if all the folders were successfully deleted, false otherwise 
*/
inline bool deleteAllFolders(QStringList &folders)
{
    QStringList successfulDeletions;

    for (const QString &path : folders)
    {
        if (QDir(path).removeRecursively())
        {
            qDebug() << "Deleted folder:" << path;
            successfulDeletions.append(path);
        }
        else
        {
            qDebug() << "Failed to delete folder:" << path;
        }
    }

    bool deletionSuccess = (successfulDeletions == folders);

    // Remove successfully deleted paths from the main folders list
    for (const QString &successfulPath : successfulDeletions)
    {
        folders.removeAll(successfulPath);
    }

    return deletionSuccess;
}

#endif