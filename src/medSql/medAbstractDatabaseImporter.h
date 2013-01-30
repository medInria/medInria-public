 /* medDatabaseImporter.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jan 19 13:41:28 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Oct  6 15:33:33 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 10
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef MEDABSTRACTDATABASEIMPORTER_H
#define MEDABSTRACTDATABASEIMPORTER_H

#include "medSqlExport.h"

#include <QtCore>
#include <QtSql>

#include <dtkCore/dtkSmartPointer.h>

#include <medJobItem.h>
#include <medDataIndex.h>

class medAbstractDatabaseImporterPrivate;
class dtkAbstractData;
class QFileInfo;
class dtkAbstractDataReader;
class dtkAbstractDataWriter;

/**
* @class medAbstractDatabaseImporter
* @brief Threaded importing/indexing of files or directories into medInria database.
* @medDatabaseImporter is in charge of importing (or indexing, any word will be used
* hereafter) items into medInria database.
* It is designed to run as a thread, to know how use it check the documentation
* of @medJobItem.
* Images can be imported, that means that are not only indexed but also copied in
* medInria database (and as a result they can end up being aggregated by volume)
* or they can be just indexed (by indicating so using the parameters in the constructor)
**/
class MEDSQL_EXPORT medAbstractDatabaseImporter : public medJobItem
{
    Q_OBJECT

public:
    medAbstractDatabaseImporter ( const QString& file, bool indexWithoutImporting );
    ~medAbstractDatabaseImporter ( void ); 

    /**
    * Runs the import process based on the input file
    * or directory given in the constructor
    **/
    void run ( void ) = 0;

signals:
    /**
     * This signal is emitted when the import/index process
     * detects an attempt of partial importing. That is when the user
     * tried to import, in 2 separate steps, images belonging
     * to the same volume.
     */
    void partialImportAttempted ( const QString& message );

    /**
     * This signal is emitted after a successful import/index.
     * @param addedIndex - the @medDataIndex that was successfully added
     */
    void addedIndex ( const medDataIndex& addedIndex);

public slots:
    void onCancel ( QObject* );

//private:
protected:    

    /**
    * Finds if parameter @seriesName is already being used in the database
    * if is not, it returns @seriesName unchanged
    * otherwise, it returns an unused new series name (created by adding a suffix)
    * @param seriesName - the series name
    * @return newSeriesName - a new, unused, series name
    **/
    virtual QString ensureUniqueSeriesName ( const QString seriesName ) = 0;

    /**
    * Populates the missing metadata in the @dtkAbstractData object.
    * If metadata is not present it's filled with default or empty values.
    * @param dtkData - the object whose missing metadata will be filled
    * @param seriesDescription - string used to fill SeriesDescription field if not present
    **/
    void populateMissingMetadata ( dtkAbstractData* dtkData, const QString seriesDescription );


    /**
    * Checks if the image which was used to create the dtkData object
    * passed as parameter already exists in the database
    * @param dtkData - a @dtkAbstractData object created from the original image
    * @param imageName - the name of the image we are looking for
    * @return true if already exists, false otherwise
    **/
    virtual bool checkIfExists ( dtkAbstractData* dtkdata, QString imageName ) = 0;


    /**
    * Tries to find a @dtkAbstractDataReader able to read input file/s.
    * @param filename - Input file/s we would like to find a reader for
    * @return a proper reader if found, NULL otherwise
    **/
    dtkSmartPointer<dtkAbstractDataReader> getSuitableReader ( QStringList filename );

    /**
    * Tries to find a @dtkAbstractDataWriter able to write input file/s.
    * @param filename - name of the file we want to write
    * @param dtkData - the @dtkAbstractData object we want to write
    * @return a proper writer if found, NULL otherwise
    **/
    dtkSmartPointer<dtkAbstractDataWriter> getSuitableWriter ( QString filename, dtkAbstractData* dtkData );

    /**
    * Walks through the whole directory tree and returns a list of every file found.
    * @param fileOrDirectory - File or directory to search
    * @return a list containing all files found
    **/
    QStringList getAllFilesToBeProcessed ( QString fileOrDirectory );

    /**
    * Tries to read the file/s indicated by filesPath.
    * Only the header is read is specified by readOnlyImageInformation parameter.
    * @param filesPath - path/s of the file/s we want to read
    * @param readOnlyImageInformation - if true only image header is read, otherwise the full image
    * @return a @dtkAbstractData containing the read data
    **/
    dtkSmartPointer<dtkAbstractData> tryReadImages ( const QStringList& filesPath,const bool readOnlyImageInformation );

    /**
    * Determines the filename where the dtkData object will be written (if importing).
    * @param dtkData - the @dtkAbstractData that will be written
    * @param volumeNumber - the volume number
    * @return a string with the new filename
    **/
    QString determineFutureImageFileName ( const dtkAbstractData* dtkData, int volumeNumber );
//     QString determineFutureImageFileName(const dtkAbstractData* dtkData, int volumeNumber);

    /**
    * Determines the extension (i.e. file format) which
    * will be used for writing the dtkData object (if importing).
    * @param dtkData - the @dtkAbstractData that will be written
    * @return a string with the desired extension if found, and empty string otherwise
    **/
    QString determineFutureImageExtensionByDataType ( const dtkAbstractData* dtkData );

    /**
    * Tries writing the dtkData object in filePath.
    * @param filePath - file path to use for writing
    * @param dtkData - @dtkAbstractData object to be written
    * @return true is writing was successful, false otherwise
    **/
    bool tryWriteImage ( QString filePath, dtkAbstractData* dtkData );

    /**
    * Adds some additional metadata (e.g. Size, FilePaths
    * and FileName) to the dtkData object.
    * @param dtkData - a @dtkAbstractData object to add metadata to
    * @param fileName - file name where the object will be written to
    * @param filePaths - if the file is aggregating more than one file, all of them will be listed here
    **/
    void addAdditionalMetaData ( dtkAbstractData* imData, QString aggregatedFileName, QStringList aggregatedFilesPaths );

    /**
    * Generates an Id intended to be unique for each volume
    * @param dtkData - @dtkAbstractData object whose id will be generate
    * @return the volume id of the dtkData object
    **/
    QString generateUniqueVolumeId ( const dtkAbstractData* dtkData );

    /**
    * Generates and saves the thumbnails for images in @dtkAbstractData.
    * Also stores as metada with key RefThumbnailPath the path of the image that
    * will be used as reference for patient, study and series.
    * @param dtkData - @dtkAbstractData object whose thumbnails will be generated and saved
    * @param pathToStoreThumbnails - path where the thumbnails will be stored
    * @return a list of the thumbnails paths
    **/
    QStringList generateThumbnails ( dtkAbstractData* dtkData, QString pathToStoreThumbnails );


    /**
     * Checks if the user is trying to perform a partial import
     * (that is, trying to import files belonging to the same volume
     * in 2 different steps).
     */
    bool isPartialImportAttempt ( dtkAbstractData* dtkData );
    
    
    medAbstractDatabaseImporterPrivate *d;


};

#endif // MEDDATABASEIMPORTER_H
