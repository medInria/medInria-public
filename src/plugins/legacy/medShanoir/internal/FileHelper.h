#ifndef FILE_HELPER
#define FILE_HELPER

#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <JsonHelper.h>

inline bool folderCreator(QString filename)
{
    QString outputfolder = QFileInfo(filename).absoluteDir().absolutePath();
    // create the path if it does not exist
    QDir dir(outputfolder);
    if (!dir.exists())
    {
        dir.mkpath(".");
        return true;
    }
    return false;
}

inline QString saveFileData(const QByteArray &fileData, const QString &filename)
{
    QFileInfo fileInfo = QFileInfo(filename);
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
 * @param headers
 * @param payload
 * @param deletionDelay time (ms) when the decompressed folder will be deleted. -1 it should not be deleted
 * @return a QVariant that is a string of the filepath if everything went well. Otherwise it returns a int that is an error code
 * The possible error codes are :
 * +0 :no filename found in the headers
 * +1 :there is not any niftii file in the decompressed file
 */
inline QVariant decompressNiftiiFromRequest(QString prefix, QJsonObject headers, QByteArray payload, int deletionDelay = 3000)
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
				qDebug()  << extractionPath << "WILL BE DELETED IN "<<deletionDelay<<" ms";
                // delete the folder containing the niftii from the filesystem in a reasonnable time
                QTimer::singleShot(deletionDelay, [extractionPath]()
                { 
					if (QDir(extractionPath).removeRecursively())
					{
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

#endif