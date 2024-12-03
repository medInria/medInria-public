/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medFirstStartClient.h>

#include "medFirstStartDownloader.h"
#include "medFirstStartCommon.h"

#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

class medFirstStartClientPrivate : public QObject
{
public:
    medFirstStartClientPrivate(QNetworkAccessManager *qnam) : dl(qnam){};
    ~medFirstStartClientPrivate() = default;
    
    QString uri;
    QString updateDirPath;
    QString lastDirPath;
    QString lastFilePath;
    QString tmpDirPath;
    QString tmpFilePath;

    medFirstStartDownloader dl;
};

medFirstStartClient::medFirstStartClient(QString featureName, QString fileName, QString uri, QNetworkAccessManager *qnam)
{
    d = new medFirstStartClientPrivate(qnam);

    QString baseUpdatedPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/" + QCoreApplication::organizationName() + "/" + QCoreApplication::applicationName() + "/resources/conf/";

    d->uri = uri;
    d->updateDirPath = baseUpdatedPath + featureName;
    d->lastDirPath   = d->updateDirPath + "/last/";
    d->lastFilePath  = d->lastDirPath + fileName;
    d->tmpDirPath    = d->updateDirPath + "/tmp/";
    d->tmpFilePath   = d->tmpDirPath + fileName;
}

medFirstStartClient::~medFirstStartClient()
{
    delete d;
}


bool medFirstStartClient::updateReferenceFile()
{
    bool bRes = true;

    if (!d->uri.isEmpty())
    {
        QDir(d->updateDirPath).mkpath(".");
        QDir(d->tmpDirPath).mkpath(".");
        if (d->dl.download(d->uri, d->tmpFilePath))
        {
            if (!medFirstStartCommon::compareFiles(d->lastFilePath, d->tmpFilePath))
            {
                if (QFile::exists(d->lastDirPath))
                {
                    QDir(d->updateDirPath).rename("last", getNextArchiveFolderName());
                }
                QDir(d->tmpDirPath).rename(d->tmpDirPath, d->lastDirPath);
            }
        }
        else
        {
            bRes = false;
        }
    }

    return bRes;
}

QString medFirstStartClient::getNextArchiveFolderName()
{
    int iMax = 0;
    QDir updateDir(d->updateDirPath);
    auto subDirs = updateDir.entryList(QDir::NoDotAndDotDot  & QDir::Dirs);
    for (auto subDir : subDirs)
    {
        bool ok;
        int i = subDir.toInt(&ok);
        if (ok)
        {
            iMax = std::max(iMax, i);
        }
    }

    return QString::number(iMax + 1);
}
