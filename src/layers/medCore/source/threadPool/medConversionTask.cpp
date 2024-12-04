/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include "medConversionTask.h"

#include <medDataExporter.h>
#include <medDataImporter.h>

#include <medDataHub.h>
#include <medDataManager.h>
#include <medNewLogger.h>

medConversionTask::~medConversionTask()
{
}

void medConversionTask::run()
{
    if (m_bImport)
    {
        medAbstractData * pDataRes = medDataImporter::convertSingleDataOnfly(m_path);
        if (pDataRes)
        {
            QString index = fileSysPathToIndex(m_path);

            pDataRes->setDataIndex(index);

            m_IndexToData[index] = pDataRes;
            m_IndexToData[index].data();

            medDataHub::instance(nullptr)->getVirtualRepresentation()->addData(m_path, "", pDataRes);

            if(!m_uuid.isNull()) medDataManager::instance().medDataHubRelay(index, m_uuid); //TODO Remove when compatibility with medDataManager will be deprecated
            emit medDataHub::instance(nullptr)->dataLoaded(m_path);
        }
        else
        {
            medNotif::createNotif(notifLevel::warning, QString("Converting file ") + m_path, " failed");
        }
    }
    else if (m_bExport)
    {
        if (medDataExporter::convertSingleDataOnfly(m_data, m_path))
        {
            //update m_data
            emit medDataHub::instance(nullptr)->dataExported(m_data, m_path);
        }
        else
        {
            medNotif::createNotif(notifLevel::warning, QString("Can't save data to ") + m_path, " failed");
        }
    }
    else
    {
        mDebug << "Try to run a converting import/export thread without defined task";
    }
}

bool medConversionTask::importTask(medAbstractData & data, QString path, QUuid uuid)
{
    return false;
}

bool medConversionTask::exportTask(medAbstractData & data, QString path, QUuid uuid)
{
    return false;
}
