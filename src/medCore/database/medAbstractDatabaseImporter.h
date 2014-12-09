/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <QtCore>
#include <QtSql>

#include <medAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medAbstractJob.h>
#include <medDataIndex.h>

class medAbstractDatabaseImporterPrivate;
class medAbstractData;

/**
* @class medAbstractDatabaseImporter
* @brief Base class for database importers.
* @medAbstractDatabaseImporter is in charge of importing (or indexing, any word will be used
* hereafter) items into medInria databases.
* It is designed to run as a thread, to know how to use it, check the documentation
* of @medAbstractJob.
* It defines a set of usefuls method (populateMissingMetadata, getSuitableReader,...) and implements a default run() method.
* To implement your own database importer, implement  the pure virtual methods, and override the run() method if neccessary.
* For example, see @medDatabaseImporter and @medDatabaseNonPersistentReader
**/
class MEDCORE_EXPORT medAbstractDatabaseImporter : public medAbstractJob
{
    Q_OBJECT

public:
    medAbstractDatabaseImporter (const QString& file, const QUuid &uuid, bool indexWithoutImporting = false);
    medAbstractDatabaseImporter ( medAbstractData* medData, const QUuid& uuid);

    ~medAbstractDatabaseImporter ( void );


signals:
    /**
     * This signal is emitted after a successful import/index.
     * @param addedIndex - the @medDataIndex that was successfully added
     */
    void dataImported ( const medDataIndex& index );

    /**
     * This signal is emitted after a successful import/index.
     * @param addedIndex - the @medDataIndex that was successfully added
     * @param callerUuid - the caller Uuid. If the caller Uuid has not been specified,
     * the addedSignal(const medDataIndex& addedIndex) is emitted instead.
     */
    void dataImported ( const medDataIndex& index, const QUuid& uuid );

public slots:
    void onCancel ( QObject* );


protected:
    virtual void internalRun ( void ) ;

    QString file ( void );   
    bool isCancelled ( void );   
    bool indexWithoutImporting ( void );  
    QMap<int, QString> volumeIdToImageFile ( void );    
    QString callerUuid ( void );
    medDataIndex index(void) const;

    void populateMissingMetadata ( medAbstractData* medData, const QString seriesDescription );
    void addAdditionalMetaData ( medAbstractData* imData, QString aggregatedFileName, QStringList aggregatedFilesPaths );

    dtkSmartPointer<dtkAbstractDataReader> getSuitableReader ( QStringList filename );
    dtkSmartPointer<dtkAbstractDataWriter> getSuitableWriter ( QString filename, medAbstractData* medData );

    QStringList getAllFilesToBeProcessed ( QString fileOrDirectory );

    medAbstractData *tryReadImages ( const QStringList& filesPath,const bool readOnlyImageInformation );
    bool tryWriteImage ( QString filePath, medAbstractData* medData );

    QString determineFutureImageFileName ( const medAbstractData* medData, int volumeNumber );
    QString determineFutureImageExtensionByDataType ( const medAbstractData* medData );

    QString generateUniqueVolumeId ( const medAbstractData* medData );
    QStringList generateThumbnails ( medAbstractData* medData, QString pathToStoreThumbnails );

    void importData();
    void importFile();


    /**
    * Finds if parameter @seriesName is already being used in the database
    * if is not, it returns @seriesName unchanged
    * otherwise, it returns an unused new series name (created by adding a suffix)
    * @param seriesName - the series name
    * @return newSeriesName - a new, unused, series name
    **/
    virtual QString ensureUniqueSeriesName ( const QString seriesName ) = 0;

    /**
    * Checks if the image which was used to create the medData object
    * passed as parameter already exists in the database
    * @param medData - a @medAbstractData object created from the original image
    * @param imageName - the name of the image we are looking for
    * @return true if already exists, false otherwise
    **/
    virtual bool checkIfExists ( medAbstractData* medData, QString imageName ) = 0;

    /**
     * Retrieves patientID. Checks if patient is already in the database
     * if so, returns his Id, otherwise creates a new guid
     */
    virtual QString getPatientID(QString patientName, QString birthDate) = 0;

    /**
    * Populates database tables and generates thumbnails.
    * @param medData - a @medAbstractData object created from the original image
    * @param pathToStoreThumbnails - path where the thumbnails will be stored
    * @return medDataIndex the new medDataIndex associated with this imported series.
    **/
    virtual medDataIndex populateDatabaseAndGenerateThumbnails ( medAbstractData* medData, QString pathToStoreThumbnails ) = 0;




    medAbstractDatabaseImporterPrivate *d;


};


