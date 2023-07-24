#include "FileManager.h"

#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QProcess>

bool FileManager::folderCreator(QString filename)
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

QString FileManager::saveFileData(const QByteArray& fileData, const QString& filename)
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


 QString FileManager::extractZipFile(QString zipPath)
{
	QFileInfo fileInfo(zipPath);
	QString outputfolder = fileInfo.absoluteDir().absolutePath() +  QDir::separator() + fileInfo.baseName();
	folderCreator(zipPath);
	
    // Extract files from a zip file using the system unzipper (for linux windows and mac)
    // check the OS and use the appropriate command
    QString command;
    #ifdef Q_OS_WIN
        // using expand-Archive command
        command = "powershell.exe -command \"Expand-Archive -Path "+zipPath+" -DestinationPath "+outputfolder+"\"";
    #elif defined(Q_OS_MACOS)
        // using unzip command
        command = "unzip "+zipPath+" -d "+outputfolder;
    #elif defined(Q_OS_LINUX)
        // using unzip command
        command = "unzip "+zipPath+" -d "+outputfolder;
    #endif
	QProcess process;
	process.start(command);
	process.waitForFinished(-1); 

	int exitCode = process.exitCode();
	if (exitCode == 0)
	{
        qDebug().noquote().nospace()<<"Files have been unzipped at"<<QDir(outputfolder).absolutePath()<<" directory";
		QFile::remove(fileInfo.absoluteFilePath());
	}
    else
    {
        outputfolder="";
    }
	return outputfolder;
}