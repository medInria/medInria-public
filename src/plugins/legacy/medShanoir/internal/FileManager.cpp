#include "FileManager.h"

#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QProcess>

 QString FileManager::saveFileData(const QByteArray& fileData, const QString& filename)
 {
    QFile outputFile(filename);
    QString absoluteFilePath = ""; 
    if (outputFile.open(QIODevice::WriteOnly))
    {
        outputFile.write(fileData);
        outputFile.close();

        QFileInfo fileInfo(outputFile);
        absoluteFilePath = fileInfo.absoluteFilePath();
    }
    return absoluteFilePath;
 }

 QString FileManager::extractZipFile(QString zipPath, QString extension)
{
	QFileInfo fileInfo(zipPath);
	QString filepath = fileInfo.fileName();
	QString outputfolder = fileInfo.baseName();
	QDir().mkdir(outputfolder);
	QString command = "7z e " + filepath + " -o" + outputfolder + " -ir!*." + extension + " -y";

	QProcess process;
	process.start(command);
	process.waitForFinished(-1); 

	int exitCode = process.exitCode();
	if (exitCode == 0)
	{
        qDebug().noquote().nospace()<<extension<<" files saved in "<<QDir(outputfolder).absolutePath()<<" directory";
		QFile::remove(fileInfo.absoluteFilePath());
	}
    else
    {
        outputfolder="";
    }
	return outputfolder;
}