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
#include <medDataManager.h>
#include <medAbstractData.h>

medDataLoadThread::medDataLoadThread(medDataIndex const & index, medViewContainer * parent) : m_index(index), m_parent(parent)
{
    connect(this, SIGNAL(dataReady(medAbstractData*)), m_parent, SLOT(addData(medAbstractData *)) );
}

medDataLoadThread::~medDataLoadThread()
{
}

void medDataLoadThread::process()
{
    internalProcess(3);
    emit finished();
    deleteLater();
}

void medDataLoadThread::internalProcess(int deep)
{
    if (deep < 0)
    {
        //TODO ERROR
        //TODO LOG
        //TODO Notif
    }
    else
    {
        if (m_index.sourceId() == "fs")
        {
            QString path = indexToFileSysPath(m_index.asString());
            QFileInfo fi(path);
            if (fi.exists())
            {
                if (fi.isFile())
                {
                    m_pAbsDataList << medDataManager::instance()->retrieveData(m_index);
                    if (m_pAbsDataList[0])
                    {
                        emit dataReady(m_pAbsDataList[0]);
                    }
                }
                else if (fi.isDir())
                {
                    //TODO Handle subdir
                    m_pAbsDataList << medDataManager::instance()->retrieveDataList(m_index);
                    for (auto absData : m_pAbsDataList)
                    {
                        if (absData)
                        {
                            emit dataReady(absData);
                        }
                    }
                }
                else
                {
                    //todo faire une notif d'erreur de non prise en charge
                }
            }
        }
        else
        {
            int type = medDataHub::instance()->getDataType(m_index);
            if (type == DATATYPE_ROLE_DATASET || type == DATATYPE_ROLE_BOTH)
            {
                m_pAbsDataList << medDataManager::instance()->retrieveData(m_index);
                if (m_pAbsDataList.last())
                {
                    emit dataReady(m_pAbsDataList.last());
                }
            }
            else if (type == DATATYPE_ROLE_FOLDER)
            {
                auto clidrenList = medDataManager::instance()->getSubData(m_index);
                for (auto & child : clidrenList)
                {
                    internalProcess(deep - 1);
                }
            }
            else
            {
                //todo faire une notif d'erreur de non prise en charge
            }
        }
    }
}
