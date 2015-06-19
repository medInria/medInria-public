/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medStorage.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

#include <QtGui/QDesktopServices>
#include <medSettingsManager.h>

QString medStorage::m_dataLocation = NULL;

medStorage::medStorage(void)
{
}

medStorage::~medStorage(void)
{
}

bool medStorage::mkpath(const QString& dirPath)
{
    QDir dir; return(dir.mkpath(dirPath));
}

bool medStorage::rmpath(const QString& dirPath)
{
    QDir dir; return(dir.rmpath(dirPath));
}

QString medStorage::dataLocation(void)
{
    QString vDbLoc;

    // first check if someone set the data location, then try to pull the actual database
    if (m_dataLocation != NULL)
    {
        return m_dataLocation;
    }
    else
    {
        vDbLoc = medSettingsManager::instance()->value("database", "actual_database_location").toString();

        // if the location is still not set we return the default paths
        if ( vDbLoc.isEmpty() )
        {
#ifdef Q_OS_MAC
            vDbLoc = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
                .remove(QCoreApplication::applicationName())
                .append(QCoreApplication::applicationName());
#else
            vDbLoc = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
#endif
        }
        setDataLocation(vDbLoc);
    }
    return vDbLoc;
}

QString medStorage::configLocation(void)
{
#ifdef Q_OS_MAC
    return(QDir::homePath() + "/Library/Preferences/" + "com" + "." + QCoreApplication::organizationName() + "." + QCoreApplication::applicationName() + "." + "plist");
#else
    return(dataLocation());
#endif
}

void medStorage::setDataLocation( QString newLocation)
{
    // return without writing if the location ist the same
    if(m_dataLocation != NULL)
        if(m_dataLocation.compare(newLocation) == 0)
            return;

    m_dataLocation = newLocation;

    medSettingsManager::instance()->setValue("database", "actual_database_location", newLocation);
}


void medStorage::recurseAddDir(QDir d, QStringList & list)
{

    QStringList qsl = d.entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);

    foreach (QString file, qsl) {

        QFileInfo finfo(QString("%1/%2").arg(d.path()).arg(file));

        if (finfo.isSymLink())
            return;

        if (finfo.isDir()) {

            QString dirname = finfo.fileName();
            QDir sd(finfo.filePath());

            recurseAddDir(sd, list);

        } else
            list << QDir::toNativeSeparators(finfo.filePath());
    }
}

bool medStorage::createDestination(QStringList sourceList, QStringList& destList, QString sourceDir, QString destDir)
{
    bool res = true;

    if (!QDir(destDir).entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files).isEmpty()) {
        qWarning() << "Directory not empty: " << destDir;
        return false;
    }

    int trimCount = sourceDir.length();
    foreach (QString sourceFile, sourceList) {
        sourceFile.remove(0,trimCount);

        QString destination = destDir + sourceFile;

        // check if this is a directory
        QFileInfo completeFile (destination);
        QDir fileInfo(completeFile.path());

        if (!fileInfo.exists() && !medStorage::mkpath (fileInfo.path())) {
            qWarning() << "Cannot create directory: " << fileInfo.path();
            res = false;
        }

        destList.append(destination);
    }

    return res;
}

bool medStorage::copyFiles(QStringList sourceList, QStringList destList)
{
    if (destList.count() != sourceList.count())
        return false;

    // just copy not using a dialog
    for (int i = 0; i < sourceList.count(); i++) {
        // coping
        if (!QFile::copy(sourceList.at(i), destList.at(i))) {
            qWarning() << "[Failure] copying file: " << sourceList.at(i) << " to " << destList.at(i);
            return false;
        }
    }

    return true;
}

bool medStorage::removeDir(QString dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

