#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtCore>
#include <QtSql/QtSql>

#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractData.h>
#include <medCoreLegacyExport.h>
#include <medDataIndex.h>
#include <medJobItemL.h>

class medAbstractDatabaseImporterPrivate;
class medAbstractData;

/**
* @class medAbstractDatabaseImporter
* @brief Base class for database importers.
* @medAbstractDatabaseImporter is in charge of importing (or indexing, any word will be used
* hereafter) items into the databases.
* It is designed to run as a thread, to know how to use it, check the documentation
* of @medJobItemL.
* It defines a set of usefuls method (populateMissingMetadata, getSuitableReader,...) and implements a default run() method.
* To implement your own database importer, implement  the pure virtual methods, and override the run() method if neccessary.
* For example, see @medDatabaseImporter and @medDatabaseNonPersistentReader
**/
class MEDCORELEGACY_EXPORT medAbstractDatabaseImporter : public medJobItemL
{
    Q_OBJECT

public:
    medAbstractDatabaseImporter(const QString &file, const QUuid &uuid, bool indexWithoutImporting = false);
    medAbstractDatabaseImporter(medAbstractData *medData, const QUuid &uuid, bool indexWithoutImporting = false);
    medAbstractDatabaseImporter(const QHash<QString, QHash<QString, QVariant> > &pData,
                                const QHash<QString, QHash<QString, QVariant> > &sData,
                                const QUuid &uuid);
    ~medAbstractDatabaseImporter() override;

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

    bool isDicomName(const QString & fileName);
    QString generateUniqueVolumeId (const medAbstractData* medData , bool isDicom=false);
    QString generateThumbnail(medAbstractData* medData, QString pathToStoreThumbnail );

    void importData();
    void importFile();
    void fetchDataFromPACS();
    /**
    * Finds if parameter @seriesName is already used in a given @studyId in the database.
    * If it is not, it returns @seriesName unchanged
    * otherwise, it returns an unused new series name (created by adding a suffix)
    * @param seriesName - the series name
    * @param studyId - database study id. If empty, the entire database is searched
    * @return newSeriesName - a new, unused, series name
    **/
    virtual QString ensureUniqueSeriesName(const QString seriesName, const QString studyId = QString("") ) = 0;

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
    virtual void setNumberOfFilesInDirectory(int num) = 0;

    virtual void createDBEntryForMetadataAttachedFile(medAbstractData *medData, int seriesDbId) {};
    medAbstractDatabaseImporterPrivate *d;
};
