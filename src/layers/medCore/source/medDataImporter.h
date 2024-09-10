#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medCoreExport.h>

#include <medAbstractData.h>
#include <medAbstractDataReader.h>

/**
 * @class   medDataImporter
 * @author  Florent Leray
 * @date    13/10/2021
 *          
 * @brief   medDataImporter read data from file-system and converts them to medAbstractData.
 *          
 * @details This class provides three main function to convert a data from file-system to a
 *          medAbstractData usable by medInria.
 *          These functions are:
 *           \li convertSingleDataOnfly
 *           \li convertSingleData
 *           \li convertMultipData \n
 *           
 * 
 *          \b convertSingleDataOnfly use only static mechanism to provide pure imperative and simple conversion.\n
 *          \b convertSingleData is the classic convert for one data.\n
 *          Give the path of the data's main file or of the directory if the data is based on directory representation.\n
 *          The instance of medDataImporter will keep a pointer to the data, the reader instance, list of available readers id, etc\n
 *          \b convertMultipData \n
 */
class MEDCORE_EXPORT medDataImporter : public QObject
{
    Q_OBJECT

public:	
    medDataImporter(QObject *parent = nullptr);
    ~medDataImporter();

    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Conversion functions from path to medAbstractData
    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static medAbstractData * convertSingleDataOnfly(QString path); //One path on file or directory represents one data
    medAbstractData *        convertSingleData(QString path);      //One path on file or directory represents one data
    QList<medAbstractData*>  convertMultipData(QString path);      //One path on file or directory represents multiple data
    medAbstractData *        convertSingleData(QStringList paths); //Many paths on files represent one data
    QList<medAbstractData *> convertMultipData(QStringList paths); //Many paths on files represent multiple data

    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Control functions 
    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    medAbstractData * convertWithOtherReader(medAbstractData *&data, QString readerId = QString());
    bool              releaseReads(medAbstractData *data = nullptr);
    bool              forgotData(medAbstractData *data = nullptr);

    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Getters functions 
    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QList<medAbstractData*> getData();
    medAbstractDataReader*  getCurrentReaderInstance(QString volumeId = QString());
    medAbstractDataReader*  getCurrentReaderInstance(medAbstractData *data = nullptr);
    QString                 getCurrentReader(QString volumeId = QString());
    QString                 getCurrentReader(medAbstractData *data = nullptr);
    QStringList             getAvailableReader(QString volumeId = QString());
    QStringList             getAvailableReader(medAbstractData *data = nullptr);
    QStringList             getPaths(QString volumeId = QString());
    QStringList             getPaths(medAbstractData *data = nullptr);
    QString                 getVolumeId(medAbstractData *data);

    medAbstractDataReader* getReaderForFile(QList<medAbstractDataReader*> &readers, QString file, int &index);

    //void detectVolumes(QStringList paths, QMap<QString, QString> & volumePathsMap, QMap<QString, QString> & volumeNameMap);
    void detectVolumes(QStringList pathsIn, QString & rootDir, QMap<QString /*volumeId*/, QString /*index*/> & volumeIndexMap, QMap<QString /*volumeId*/, QPair<QString /*name*/, QString /*relPaths*/>> & volumeRelativeMap);

private:
    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Private Static functions to:             list files,   getReaders,   readFiles,         opt find volumes
    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static QStringList listFilesOfData(QString &path);
    static QList<medAbstractDataReader*> getSuitableReader(QStringList filename, QStringList *readersId = nullptr);
    static medAbstractData * readFiles(QList<medAbstractDataReader *> &readers, QStringList &fileList, medAbstractDataReader **usedReader = nullptr);
    void findVolumesInFiles(QStringList &fileList);
    void findVolumesInDirectory(QString &path);

    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Private tools functions 
    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void reset();
    inline bool getVolumeFromData(medAbstractData*, QString & volume);
    //template<typename T> T get(QString volumeId, const QMap<QString, T>& map);

    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Other 
    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static QString createVolumeId(medAbstractData* data); //TODO move into the medAbstractReader with virtual but not pure to have default impl
signals:


private:
    QMap <QString, QStringList>            m_pathsVolumesMap;
    QMap <QString, QString>                m_nameVolumesMap;
    QMap <QString, QStringList>            m_availablesReadersVolumesMap;
    QMap <QString, medAbstractData*>       m_meddataVolumesMap;
    QMap <QString, medAbstractDataReader*> m_currentReaderVolumesMap;
};

QString fileSysPathToIndex2(const QString &path, QStringList files = {});
QString computeRootPathOfListPath2(QStringList &fileList, QStringList &relativePathList);


