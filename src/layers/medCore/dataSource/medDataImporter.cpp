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

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor and destructor + reset function
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
medDataImporter::medDataImporter(QObject *parent)
{

}

medDataImporter::~medDataImporter()
 {
    reset();
 }

void medDataImporter::reset()
{

    m_pathsVolumesMap.clear();
    m_availablesReadersVolumesMap.clear();
    m_meddataVolumesMap.clear();
    for (auto *reader : m_currentReaderVolumesMap)
    {
        delete reader;
        reader = nullptr;
    }
    m_currentReaderVolumesMap.clear();
}


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Conversion functions from path to medAbstractData
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
* @brief  Convert a data from file or directory to a medAbstractData.
* @param  path to a folder or a file that contains only one data, without extra files.
* @return A pointer to the medAbstractData if conversion was successful, nullptr otherwise.
*/
medAbstractData * medDataImporter::convertSingleDataOnfly(QString path)
{
    medAbstractData *medDataRes = nullptr;

    QStringList fileList = listFilesOfData(path);
    auto readers = getSuitableReader(fileList);
    medDataRes = readFiles(readers, fileList);

    return medDataRes;
}

/**
* @brief  Convert a data from file or directory to a medAbstractData, and keep conversion context.
* @param  path to a folder or a file that contains only one data, without extra files.
* @return A pointer to the medAbstractData if conversion was successful, nullptr otherwise.
*/
medAbstractData * medDataImporter::convertSingleData(QString path)
{   
    reset();

    medAbstractData *medDataRes = nullptr;
    QString volume; //default volume is empty because here you used only one volume
    
    QStringList fileList = listFilesOfData(path);
    auto readers = getSuitableReader(fileList, &m_availablesReadersVolumesMap[volume]);
    medDataRes = readFiles(readers, fileList, &m_currentReaderVolumesMap[volume]);

    m_pathsVolumesMap[volume] = fileList;
    m_meddataVolumesMap[volume] = medDataRes;

    return medDataRes;
}

/**
* @brief  Convert many data from file or directory to a list of medAbstractData, and keep conversion context.
* @param  path to a folder or a file that contains data.
* @return A list of pointers to medAbstractData.
*/
QList<medAbstractData*>  medDataImporter::convertMultipData(QString path)
{
    reset();

    QList<medAbstractData*> listRes;

    findVolumesInDirectory(path);
    auto volumes = m_pathsVolumesMap.keys();
    for (auto &volume : volumes)
    {
        auto readers = getSuitableReader(m_pathsVolumesMap[volume], &m_availablesReadersVolumesMap[volume]);
        auto medData = readFiles(readers, m_pathsVolumesMap[volume], &m_currentReaderVolumesMap[volume]);

        m_meddataVolumesMap[volume] = medData;
        listRes.push_back(medData);
    }

    return listRes;
}


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private Static functions to:             list files,   getReaders,   readFiles,         opt find volumes
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* @brief  Obtain a list of all the files that are going to be processed
* @param  path to a folder or a file that contains data.
* @return A list of path.
* @details this flattens the tree structure (if the input is a directory)
*          and puts all the files in one single list.
*/
QStringList medDataImporter::listFilesOfData(QString &path)
{
    QStringList fileList;

    QDir dir(path);
    dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    if (dir.exists())
    {
        QDirIterator directory_walker(path, QDir::Files, QDirIterator::Subdirectories);
        while (directory_walker.hasNext())
        {
            QString aFilePath = directory_walker.next();
            QFileInfo fileInfo(aFilePath);
            if (fileInfo.size() != 0)
            {
                fileList << aFilePath;
            }
        }
    }
    else
    {
        QFileInfo fileInfo(path);
        if (fileInfo.size() != 0)
        {
            fileList << path;
        }
    }
    fileList.sort();

    return fileList;
}

/**
* @brief  Obtain a list of readers available for given files.
* @param  filename is the list of path for the data to convert.
* @param  readersId if not null will contain all available readers id for the data to convert.
* @return A list of potential reader.
*/
QList<medAbstractDataReader*> medDataImporter::getSuitableReader(QStringList filename, QStringList *readersId)
{
    QList<medAbstractDataReader*> dataReadersRes;

    QStringList readers = medAbstractDataFactory::instance()->readers();

    if (readers.size() != 0)
    {
        for (int i = 0; i < readers.size(); i++)
        {
            auto *dataReader = dynamic_cast<medAbstractDataReader*>(medAbstractDataFactory::instance()->reader(readers[i])); //TODO remove cast when we remove dtk
            if (dataReader && dataReader->canRead(filename)) //TODO remove check nullptr when remove the dynamic_cast above
            {
                dataReadersRes.push_back(dataReader);
                if (readersId)
                {
                    *readersId << readers[i];
                }
            }
            else
            {
                delete(dataReader);
            }
        }
    }

    return dataReadersRes;
}

/**
* @brief  Create medAbstractData from Files.
* @param  readers provides readers available for this type of data.
* @param  fileList is the list of files that constitute the input data.
* @param  usedReader if not null will contain the instance of the reader used.
* @return A medAbstractData if the reading is a success, a nullptr in otherwise.
*/
medAbstractData * medDataImporter::readFiles(QList<medAbstractDataReader *> &readers, QStringList &fileList, medAbstractDataReader **usedReader)
{
    medAbstractData * medDataRes = nullptr;

    if (readers.size() > 0)
    {
        int i = 0;
        while (medDataRes == nullptr && i < readers.size())
        {
            bool bReaded = readers[i]->read(fileList);
            if (bReaded)
            {
                medDataRes = dynamic_cast<medAbstractData*>(readers[i]->data());
                if (usedReader)
                {
                    *usedReader = readers[i];
                }
                else
                {
                    delete(readers[i]);
                    readers[i] = nullptr;
                }
            }
            else
            {
                delete(readers[i]);
                readers[i] = nullptr;
            }
            ++i;
        }

        for (i; i < readers.size(); ++i)
        {
            delete(readers[i]);
            readers[i] = nullptr;
        }
    }

    return medDataRes;
}

/**
 * @fn  void medDataImporter::findVolumesInDirectory(QString &path)
 * @brief   Searches volumes/data in directory or a file
 * @param [in]  path    Full pathname of the file.
 */
void medDataImporter::findVolumesInDirectory(QString &path)
{
    QStringList readersID = medAbstractDataFactory::instance()->readers();
    QStringList fileList = listFilesOfData(path);
    QList<medAbstractDataReader*>  readers;
    for (auto reader : readersID)
    {
        readers.push_back(static_cast<medAbstractDataReader*>(medAbstractDataFactory::instance()->reader(reader)));
    }

    for (auto file : fileList)
    {
        for (int i=0; i<readers.size(); ++i)
        {
            if (readers[i]->canRead(file) && readers[i]->readInformation(file))
            {
                QString volume = createVolumeId(dynamic_cast<medAbstractData*>(readers[i]->data()));
                m_pathsVolumesMap[volume] << file;
                m_availablesReadersVolumesMap[volume] << readersID[i];
            }
        }
    }

    for (auto reader : readers)
    {
        delete reader;
    }
}

/**
 * @fn  medAbstractData * medDataImporter::convertWithOtherReader(medAbstractData *&data, QString readerId)
 * @brief   Convert an existing data with another reader
 * @param [in,out]  data        the input data to convert again with a different reader.
 * @param [in]      readerId    Identifier of the another reader. If it is empty the next reader of the current one will be used. 
 * @returns nullptr if it fails, else a pointer on the new medAbstractData converted by another reader.
 */
medAbstractData * medDataImporter::convertWithOtherReader(medAbstractData *&data, QString readerId)
{
    medAbstractData *dataRes = nullptr;

    QString volumeId;
    if (getVolumeFromData(data, volumeId))
    {
        if (readerId.isEmpty())
        {

            QString readerIdOld = m_currentReaderVolumesMap[volumeId]->identifier();
            int oldReaderIndex = m_availablesReadersVolumesMap[volumeId].indexOf(readerIdOld);
            int newReaderIndex = (oldReaderIndex +1) % m_availablesReadersVolumesMap[volumeId].size();
            if (oldReaderIndex != newReaderIndex)
            {
                readerId = m_availablesReadersVolumesMap[volumeId][newReaderIndex];
            }
        }

        if (!readerId.isEmpty() && (m_availablesReadersVolumesMap[volumeId].indexOf(readerId) > -1))
        {
            auto *dataReader = dynamic_cast<medAbstractDataReader*>(medAbstractDataFactory::instance()->reader(readerId));
            if(dataReader->read(m_pathsVolumesMap[volumeId]))
            {
                releaseReads(data);
                data = dynamic_cast<medAbstractData*>(dataReader->data());
                m_currentReaderVolumesMap[volumeId] = dataReader;
                m_meddataVolumesMap[volumeId] = data;
                dataRes = data;
            }            
        }
    }


    return dataRes;
}

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Control functions 
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @fn  bool medDataImporter::releaseReads(medAbstractData * data)
 * @brief   Deallocate the reads used to convert the data.
 * @param [in]  data pointer to the data converted by this instance.
 * @returns True if it succeeds, false if it fails.
 */
bool medDataImporter::releaseReads(medAbstractData * data)
{
    bool bRes = false;

    QString volumeId;
    bRes = getVolumeFromData(data, volumeId);
    if (bRes)
    {
        bRes = m_currentReaderVolumesMap.contains(volumeId);
        if (bRes)
        {
            delete m_currentReaderVolumesMap[volumeId];
            m_currentReaderVolumesMap[volumeId] = nullptr;
        }
    }

    return bRes;
}

/**
 * @fn  bool medDataImporter::forgotData(medAbstractData * data)
 * @brief   Remove all links between the data and its converter.
 * @param [in]  data pointer to the data converted by this instance.
 * @returns True if it succeeds, false if it fails (data was not obtain by this instance).
 */
bool medDataImporter::forgotData(medAbstractData * data)
{
    bool bRes = false;

    QString volumeId;
    bRes = getVolumeFromData(data, volumeId);
    if (bRes)
    {
        m_pathsVolumesMap.remove(volumeId);
        m_availablesReadersVolumesMap.remove(volumeId);
        m_meddataVolumesMap.remove(volumeId);
        delete(m_currentReaderVolumesMap.take(volumeId));
    }

    return bRes;
}


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Getters functions 
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @fn  QList<medAbstractData*> medDataImporter::getData()
 * @brief   Provide a list of medAbstractData converted by this instance.
 * @returns list of medAbstractData successfully converted by this instance.
 */
QList<medAbstractData*> medDataImporter::getData()
{
    QList<medAbstractData*> medDataList;

    for (auto *medData : m_meddataVolumesMap)
    {
        medDataList << medData;
    }

    return medDataList;
}

/**
 * @fn  medAbstractDataReader * medDataImporter::getCurrentReaderInstance(QString volumeId)
 * @brief   Gets current reader instance.
 * @param   volumeId    Identifier for the volume.
 * @returns Null if it fails, else the current reader instance.
 */
medAbstractDataReader * medDataImporter::getCurrentReaderInstance(QString volumeId)
{
    medAbstractDataReader *readerRes = nullptr;

    if (volumeId.isEmpty() && !m_currentReaderVolumesMap.isEmpty())
    {
        readerRes = m_currentReaderVolumesMap.first();
    }
    else if (m_currentReaderVolumesMap.contains(volumeId))
    {
        readerRes = m_currentReaderVolumesMap[volumeId];
    }

    return readerRes;
}

/**
 * @fn  medAbstractDataReader * medDataImporter::getCurrentReaderInstance(medAbstractData * data)
 * @brief   Gets current reader instance.
 * @param   data pointer to the data converted by this instance.
 * @returns Null if it fails, else the current reader instance.
 */
medAbstractDataReader * medDataImporter::getCurrentReaderInstance(medAbstractData * data)
{
    medAbstractDataReader *readerRes = nullptr;
    QString volumeId;
    if (getVolumeFromData(data, volumeId))
    {
        readerRes = getCurrentReaderInstance(volumeId);
    }

    return readerRes;
}

/**
 * @fn  QString medDataImporter::getCurrentReader(QString volumeId)
 * @brief   Gets current reader for a volumeId
 * @param   volumeId    Identifier for the volume. If empty get first volume.
 * @returns The current reader for this volumeId or empty if it fails
 */
QString medDataImporter::getCurrentReader(QString volumeId) //Will failed behavior if reader instance is deallocated 
{
    QString readerIdRes;

    if (volumeId.isEmpty() && !m_currentReaderVolumesMap.isEmpty())
    {
        readerIdRes = m_currentReaderVolumesMap.first()->identifier();
    }
    else if (m_currentReaderVolumesMap.contains(volumeId))
    {
        readerIdRes = m_currentReaderVolumesMap[volumeId]->identifier();
    }

    return readerIdRes;
}

/**
 * @fn  QString medDataImporter::getCurrentReader(medAbstractData * data)
 * @brief   Gets current reader for a medAbstractData
 * @param   data pointer to the data converted by this instance.
 * @returns The current reader for this volumeId or empty if it fails
 */
QString medDataImporter::getCurrentReader(medAbstractData * data)
{
    QString readerRes;
    QString volumeId;
    if (getVolumeFromData(data, volumeId))
    {
        readerRes = getCurrentReader(volumeId);
    }

    return readerRes;
}

/**
 * @fn  QStringList medDataImporter::getAvailableReader(QString volumeId)
 * @brief   Gets available readers for a volumeId
 * @param   volumeId Identifier for the volume. If empty get first volume.
 * @returns The list readerId for this volumeId or empty if it fails
 */
QStringList medDataImporter::getAvailableReader(QString volumeId)
{
    QStringList readersRes;

    if (volumeId.isEmpty() && !m_availablesReadersVolumesMap.isEmpty())
    {
        readersRes = m_availablesReadersVolumesMap.first();
    }
    else if (m_availablesReadersVolumesMap.contains(volumeId))
    {
        readersRes = m_availablesReadersVolumesMap[volumeId];
    }

    return readersRes;
}

/**
 * @fn  QStringList medDataImporter::getAvailableReader(medAbstractData * data)
 * @brief   Gets available readers for a medAbstractData.
 * @param   data pointer to the data converted by this instance.
 * @returns The list readerId for this volumeId or empty if it fails
 */
QStringList medDataImporter::getAvailableReader(medAbstractData * data)
{
    QStringList availableReaderRes;
    QString volumeId;
    if (getVolumeFromData(data, volumeId))
    {
        availableReaderRes = getAvailableReader(volumeId);
    }

    return availableReaderRes;
}

/**
 * @fn  QStringList medDataImporter::getPaths(QString volumeId)
 * @brief   Gets paths for a volumeId.
 * @param   volumeId Identifier for the volume. If empty get first volume.
 * @returns Paths of the data.
 */
QStringList medDataImporter::getPaths(QString volumeId)
{
    QStringList readersRes;

    if (volumeId.isEmpty() && !m_pathsVolumesMap.isEmpty())
    {
        readersRes = m_pathsVolumesMap.first();
    }
    else if (m_availablesReadersVolumesMap.contains(volumeId))
    {
        readersRes = m_pathsVolumesMap[volumeId];
    }

    return readersRes;
}

/**
 * @fn  QStringList medDataImporter::getPaths(medAbstractData * data)
 * @brief   Gets paths for a medAbstractData.
 * @param   data pointer to the data converted by this instance.
 * @returns Paths of the data.
 */
QStringList medDataImporter::getPaths(medAbstractData * data)
{
    QStringList pathsRes;
    QString volumeId;
    if (getVolumeFromData(data, volumeId))
    {
        pathsRes = getPaths(volumeId);
    }

    return pathsRes;
}


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private functions 
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @fn  QString medDataImporter::createVolumeId(medAbstractData * data)
 * @brief   Creates volume identifier
 * @todo Implement it and in 2nd time move into readers
 * @param [in]  data to determine the volumeId.
 * @returns The volume identifier.
 */
QString medDataImporter::createVolumeId(medAbstractData * data)
{
    return QString(); //TODO
}

/**
 * @fn  inline bool medDataImporter::getVolumeFromData(medAbstractData *data, QString & volume)
 * @brief   Helper to get volume from data
 * @param [in]  data the data.
 * @param [out]  volume  The volume.
 * @returns True if it succeeds, false if it fails.
 */
inline bool medDataImporter::getVolumeFromData(medAbstractData *data, QString & volume)
{
    volume = m_meddataVolumesMap.key(data);
    return m_meddataVolumesMap.value(volume) && m_meddataVolumesMap.value(volume) == data;
}