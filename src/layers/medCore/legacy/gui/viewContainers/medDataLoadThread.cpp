/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include <medDataLoadThread.h>

#include <medDataHub.h>
#include <medDataImporter.h>
#include <medDataManager.h>
#include <medAbstractData.h>


medDataLoadThread::medDataLoadThread(QList<medDataIndex> const & index, QList<QUrl> const & urls, medViewContainer *parent) : m_indexList(index), m_urlList(urls), m_parent(parent)
{
    connect(this, SIGNAL(dataReady(medAbstractData*)), m_parent, SLOT(addData(medAbstractData *)));
}

void medDataLoadThread::process()
{
    QStringList paths;
    for (auto & url : m_urlList)
    {
        QDirIterator it(url.toLocalFile(), QDir::Files, QDirIterator::Subdirectories);
        while(it.hasNext())
        {
            paths << it.next();
        }
    }
    QStringList files;
    QString path     = computeRootPathOfListPath(paths, files);
    m_indexList << fileSysPathToIndex(path, files);

    //medDataImporter importer;
    //importer.etectVolumes(paths, m_rootDir,  m_volumePathsMap, m_volumeRelMap);
    ////detectVolume(paths, m_volumePathsMap);
    //for (auto indexTmp : m_volumePathsMap.values())
    //{
    //    m_indexList << indexTmp;
    //}
    for (medDataIndex index : m_indexList)
    {
        internalProcess(index, 3);
    }
    emit finished();
    deleteLater();
}

void medDataLoadThread::internalProcess(medDataIndex &index, int deep)
{
    if (deep < 0)
    {
        //TODO ERROR
        //TODO LOG
        //TODO Notif
    }
    else
    {
        int type = medDataHub::instance()->getDataType(index);
        if (type == DATATYPE_ROLE_DATASET || type == DATATYPE_ROLE_BOTH)
        {
            m_pAbsDataList << medDataManager::instance()->retrieveDataList(index);
            for (auto absData : m_pAbsDataList)
            {
                if (absData)
                {
                    emit dataReady(absData);
                }
            }
        }
        else if (type == DATATYPE_ROLE_FOLDER)
        {
            auto clidrenList = medDataManager::instance()->getSubData(index);
            for (auto & child : clidrenList)
            {
                internalProcess(child, deep - 1);
            }
        }
        else
        {
            //todo faire une notif d'erreur de non prise en charge
        }
    }
}
