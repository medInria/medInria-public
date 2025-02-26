/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataImporter.h>

#include <medAbstractDataFactory.h>
#include "medDataExporter.h"

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor and destructor + reset function
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Conversion functions from path to medAbstractData
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private Static functions to:             list files,   getReaders,   readFiles,         opt find volumes
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Control functions 
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Getters functions 
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


medDataExporter::medDataExporter(QObject *parent)
{
}

medDataExporter::~medDataExporter()
{
}

bool medDataExporter::convertSingleDataOnfly(medAbstractData * data, QString &path)
{
    bool bRes = false;

    QList<medAbstractDataWriter*> dataWriters = getSuitableWriter(data, path);
    if (!dataWriters.isEmpty())
    {
        dataWriters[0]->setData(data);
        auto fileExtList = dataWriters[0]->supportedFileExtensions();
        if (!fileExtList.isEmpty())
        {
            path = path + fileExtList[0];
        }
        bRes = dataWriters[0]->write(path);
    }

    return bRes;
}

bool medDataExporter::convertSingleDataOnfly(medAbstractData * data, QString & path, QStringList fileExts)
{
    bool bRes = false;

    QList<medAbstractDataWriter*> dataWriters = getSuitableWriter(data, path);
    if (!dataWriters.isEmpty())
    {
        QMultiMap<QString, medAbstractDataWriter*> extfileWriterMap;
        for (auto fileExt : fileExts)
        {
            for (auto * pDataWriter : dataWriters)
            {
                if (pDataWriter->supportedFileExtensions().contains(fileExt))
                {
                    extfileWriterMap.insert(fileExt, pDataWriter);
                }
            }
        }

        bool bContinue = true;
        while (bContinue)
        {
            for (int i = 0; (i < fileExts.size() && bContinue); ++i)
            {
                if (extfileWriterMap.contains(fileExts[i]))
                {
                    auto pathTmp = path + fileExts[i];
                    auto writers = extfileWriterMap.values(fileExts[i]);
                    for (int i = 0; (i < writers.size() && bContinue); ++i)
                    {
                        writers[i]->setData(data);
                        bRes = writers[i]->write(pathTmp);
                        if (bRes)
                        {
                            bContinue = false;
                            path = pathTmp;
                        }
                    }
                }
            }
            bContinue = false;
        }
    }

    return bRes;
}

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private functions 
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QList<medAbstractDataWriter*> medDataExporter::getSuitableWriter(medAbstractData * medData, QString filePath, QStringList fileExts, QStringList *writersId)
{
    QList<medAbstractDataWriter*> dataWriterRes;

    QStringList writers = medAbstractDataFactory::instance()->writers();

    if (writers.size() != 0)
    {
        for (int i = 0; i < writers.size(); i++)
        {
            QString fileExt;
            auto *dataWriter = dynamic_cast<medAbstractDataWriter*>(medAbstractDataFactory::instance()->writer(writers[i])); 
            auto fileExtList = dataWriter->supportedFileExtensions();
            if (!fileExtList.isEmpty())
            {
            	fileExt = fileExtList[0];
            }

            if (dataWriter && dataWriter->handled().contains(medData->identifier()) && dataWriter->canWrite(filePath + fileExt))
            {
                dataWriterRes.push_back(dataWriter);
                if (writersId)
                {
                    *writersId << writers[i];
                }
            }
            else
            {
                delete(dataWriter);
            }
        }
    }

    return dataWriterRes;
}
