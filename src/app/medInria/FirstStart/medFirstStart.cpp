/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medFirstStart.h>

#include <medFirstStartDownloader.h>
#include <medFirstStartJSonIO.h>


#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>

#include <QMap>
#include <QPair>


class medFirstStartPrivate : public QObject
{
public:
    medFirstStartPrivate();
    ~medFirstStartPrivate() = default;



    QMap <QString, QPair <QString, QUrl> > registerdMap;
    QMap <QString, QPair <QString, QUrl> > updateMap;
    QMap <QString, QPair <QString, QUrl> > updatedMap;

    QString baseUpdatedPath;
    QString confiPath;

    medFirstStartDownloader downloader;
    medFirstStartJSonIO     confReadWriter;

};

medFirstStartPrivate::medFirstStartPrivate() : QObject(), confReadWriter("")
{

}

medFirstStart::medFirstStart()
{
    d = new medFirstStartPrivate();
    d->baseUpdatedPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/" + QCoreApplication::organizationName() + "/" + QCoreApplication::applicationName() + "/resources/conf/";
    d->confiPath = "updatedBaseConfigs.json";
}

medFirstStart::~medFirstStart()
{
    delete d;
}

void medFirstStart::pushPathToCheck(QString pathToCheck, QString pathRessourceBase, QUrl urlDL)
{
    d->registerdMap[pathToCheck] = QPair<QString, QUrl>(pathRessourceBase, urlDL);
}

void medFirstStart::checkAndUpdate()
{
    for (QString const & aPath : d->registerdMap.keys())
    {
        if (!QFileInfo::exists(aPath))
        {
            if (!copy(d->registerdMap[aPath].first, aPath))
            {
                d->updateMap[aPath] = d->registerdMap[aPath];
            }
        }
    }

    askNewVersion();

    for (QString const & aPath : d->updatedMap.keys())
    {
        copy(d->updatedMap[aPath].first, aPath);
    }
}

void medFirstStart::askNewVersion()
{
    bool bRes = true;

    bRes = readUpdatedFile();
    for (auto const & key : d->updateMap.keys())
    {
        auto & url = d->updateMap[key].second;
        if (!url.isEmpty())
        {
            //d->updateMap[key] = ;
        }
    }
    //d->downloader.setDownload();
}





bool medFirstStart::copy(QString const & pathSource, QString const & pathDest)
{
    bool bRes = true;

    QFileInfo fiSource(pathSource);
    if (fiSource.isFile())
    {
        QDir destDir = QFileInfo(pathDest).dir();
        destDir.mkpath(destDir.absolutePath());
        bRes = bRes && QFile::copy(pathSource, pathDest);
    }
    else if (fiSource.isDir())
    {
        QDirIterator it(pathSource, QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        QDir dir(pathSource);
        const int srcPathLength = dir.canonicalPath().length();

        while (it.hasNext())
        {
            it.next();

            const auto fi = it.fileInfo();
            const QString relPath = fi.canonicalPath().mid(srcPathLength);
            const QString absDstPath = pathDest + relPath;

            if (fi.isFile())
            {
                bRes = bRes && QFile::copy(fi.canonicalPath(), absDstPath);
            }
            else if (fi.isDir())
            {
                bRes = bRes && dir.mkpath(absDstPath);
            } 
        }
    }
    else
    {
        bRes = false;
    }

    return bRes;
}

bool medFirstStart::readUpdatedFile()
{
    bool bRes = d->confReadWriter.readJsonUpdatedFile();
    if (bRes)
    {
        auto updatedMap = d->confReadWriter.getUpdatedMap();
        for (auto const & key : updatedMap.keys())
        {
            d->registerdMap[key] = updatedMap[key];
        }
    }

    return bRes;
}

bool medFirstStart::writeUpdatedFile()
{
    return false;
}
