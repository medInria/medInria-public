/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medFirstStartUpdateFile.h"

#include "medFirstStartCommon.h"

#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>


class medFirstStartUpdateFilePrivate
{
public:
    medFirstStartUpdateFilePrivate() = default;
    ~medFirstStartUpdateFilePrivate() = default;


    QString fileName;

    QString updateDirPath;
    QString lastFilePath;
    QString filePath;
    QString originalFilePath;

    std::function<bool(QString, QString)> update;
    std::function<bool(QString, QString)> init;
};

medFirstStartUpdateFile::medFirstStartUpdateFile(QString filePath, QString featureName, QString originalFilePath,
                                                 std::function<bool(QString, QString)> update, std::function<bool(QString, QString)> init)
{
    d = new medFirstStartUpdateFilePrivate();
    QString baseUpdatedPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/" + QCoreApplication::organizationName() + "/" + QCoreApplication::applicationName() + "/resources/conf/";

    d->update = update;
    d->init   = init;

    d->fileName         = QFileInfo(filePath).fileName();
    d->updateDirPath    = baseUpdatedPath + featureName;
    d->lastFilePath     = d->updateDirPath + "/last/" + d->fileName;
    d->filePath         = filePath;
    d->originalFilePath = originalFilePath;
}

medFirstStartUpdateFile::~medFirstStartUpdateFile()
{
    delete d;
}




bool medFirstStartUpdateFile::installFile()
{
    bool bRes = prepareFile();

    if (currentFileOutdated())
    {
        if (d->init)
        {
            d->init(d->lastFilePath, d->filePath);
        }

        if (d->update)
        {
            d->update(d->lastFilePath, d->filePath);
        }
        bRes = medFirstStartCommon::copy(d->lastFilePath, d->filePath);
    }

    return bRes;
}

bool medFirstStartUpdateFile::prepareFile()
{
    bool bRes = true;

    QDir(d->updateDirPath).mkpath(".");

    if (!QFile::exists(d->lastFilePath))
    {
        bRes = medFirstStartCommon::copy(d->originalFilePath, d->lastFilePath);
    }

    return bRes;
}

bool medFirstStartUpdateFile::currentFileOutdated()
{
    bool bRes = false;

    QDir updateDir(d->updateDirPath);

    if (updateDir.exists())
    {
        if (QFile::exists(d->filePath))
        {
            if (!medFirstStartCommon::comparerFiles(d->lastFilePath, d->filePath))
            {
                auto subDirs = updateDir.entryList(QDir::NoDotAndDotDot  | QDir::AllDirs);
                for (auto subDir : subDirs)
                {
                    bRes |= medFirstStartCommon::comparerFiles(d->filePath, d->updateDirPath + '/' + subDir + '/' + d->fileName);
                }
            }
        }
        else
        {
            bRes = true;
        }
    }
    else
    {
        bRes = true;
    }

    return bRes;
}
