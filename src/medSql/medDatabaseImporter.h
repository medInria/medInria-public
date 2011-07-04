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

#ifndef MEDDATABASEIMPORTER_H
#define MEDDATABASEIMPORTER_H

#include "medSqlExport.h"

#include <medCore/medJobItem.h>
#include <QtCore>
#include <QtSql>

class medDatabaseImporterPrivate;
class dtkAbstractData;
class QFileInfo;
class dtkAbstractDataReader;
class dtkAbstractDataWriter;

/**
* @class medDatabaseImporter
* @brief Threaded importing of files or directories into medinria database
* @medDatabaseImporter is in charge of importing items into medinria database
* it is designed to run as a thread, to know how to check the documentation
* of @medJobItem.
* Images are imported, that means that are not only indexed but also copied in
* medinria database, as a result they can end up being aggregated by volume.
**/
class MEDSQL_EXPORT medDatabaseImporter : public medJobItem
{
    Q_OBJECT

public:
    medDatabaseImporter(const QString& file, bool indexWithoutImporting);
    ~medDatabaseImporter(void);

    /**
    * Runs the import process based on the input file
    * or directory given in the constructor
    **/
    void run(void);

public slots:
    void onCancel(QObject*);

private:

    void import(void);

    void index(void);

    /**
    * Populates the missing metadata in the @dtkAbstractData object.
    * If metadata is not present it's filled with default or empty values.
    * @param dtkData - the object whose missing metadata will be filled
    * @param fileBaseName - the base name of the file used to generate the dtkData object
    *                       it's used to fill SeriesDescription field if not present
    **/
    void populateMissingMetadata(dtkAbstractData* dtkData, const QString fileBaseName);

    /**
    * Checks if the image which was used to create the dtkData object
    * passed as parameter already exists in the database
    * @param dtkData - a @dtkAbstractData object created from the original image
    * @param fileInfo - a @QFileInfo object created with the original image filename
    * @return true if already exists, false otherwise
    **/
    bool checkIfExists(dtkAbstractData* dtkData, const QFileInfo* fileInfo);

    /**
    * Populates database tables and generates thumbnails.
    * @param dtkData - a @dtkAbstractData object created from the original image
    * @param seriesInfo - a @QFileInfo object created with the aggregated image filename
    **/
    void populateDatabaseAndGenerateThumbnails(dtkAbstractData* dtkData, const QFileInfo* seriesInfo);

    /**
    * Tries to find a @dtkAbstractDataReader able to read input file/s.
    * @param filename - Input file/s we would like to find a reader for
    * @return a proper reader if found, NULL otherwise
    **/
    dtkAbstractDataReader* getSuitableReader(QStringList filename);

    /**
    * Tries to find a @dtkAbstractDataWriter able to write input file/s.
    * @param filename - name of the file we want to write
    * @param dtkData - the @dtkAbstractData object we want to write
    * @return a proper writer if found, NULL otherwise
    **/
    dtkAbstractDataWriter* getSuitableWriter(QString filename, dtkAbstractData* dtkData);

    /**
    * Walks through the whole directory tree and returns a list of every file found.
    * @param fileOrDirectory - File or directory to search
    * @return a list containing all files found
    **/
    QStringList getAllFilesToBeProcessed(QString fileOrDirectory);

    /**
    * Tries to read the file/s indicated by filesPath.
    * Only the header is read is specified by readOnlyImageInformation parameter.
    * @param filesPath - path/s of the file/s we want to read
    * @param readOnlyImageInformation - if true only image header is read, otherwise the full image
    * @return a @dtkAbstractData containing the read data
    **/
    dtkAbstractData* tryReadImages(QStringList filesPath, bool readOnlyImageInformation);

    /**
    * Determines the filename where the dtkData object will be written.
    * @param dtkData - the @dtkAbstractData that will be written
    * @param volumeNumber - the volume number
    * @return a string with the new filename
    **/
    QString determineFutureImageFileName(dtkAbstractData* dtkData, int volumeNumber);

    /**
    * Determines the extension (i.e. file format) which
    * will be used for writing the dtkData object.
    * @param dtkData - the @dtkAbstractData that will be written
    * @return a string with the desired extension if found, and empty string otherwise
    **/
    QString determineFutureImageExtensionByDataType(dtkAbstractData* dtkData);

    /**
    * Tries writing the dtkData object in filePath.
    * @param filePath - file path to use for writing
    * @param dtkData - @dtkAbstractData object to be written
    * @return true is writing was successful, false otherwise
    **/
    bool tryWriteImage(QString filePath, dtkAbstractData* dtkData);

    /**
    * Adds some additional metadata (e.g. Size, FilePaths
    * and FileName) to the dtkData object.
    * @param dtkData - a @dtkAbstractData object to add metadata to
    * @param fileName - file name where the object will be written to
    * @param filePaths - if the file is aggregating more than one file, all of them will be listed here
    **/
    void addAdditionalMetaData(dtkAbstractData* dtkData, QString fileName, QStringList filePaths);

    /**
    * Generates an Id intended to be unique for each volume
    * @param dtkData - @dtkAbstractData object whose id will be generate
    * @return the volume id of the dtkData object
    **/
    QString generateUniqueVolumeId(dtkAbstractData* dtkData);

    /**
        * Generates and saves the thumbnails for images in @dtkAbstractData.
        * Also stores as metada with key RefThumbnailPath the path of the image that
        * will be used as reference for patient, study and series.
        * @param dtkData - @dtkAbstractData object whose thumbnails will be generated and saved
        * @param aggregatedFileNameFileInfo - @QFileInfo of the file aggregating the images
        * @return a list of the thumbnails paths
        **/
    QStringList generateThumbnails(dtkAbstractData* dtkData, const QFileInfo* aggregatedFileNameFileInfo);

    int getOrCreatePatient(dtkAbstractData* dtkData, QSqlDatabase db);
    int getOrCreateStudy(dtkAbstractData* dtkData, QSqlDatabase db, int patientId);
    int getOrCreateSeries(dtkAbstractData* dtkData, QSqlDatabase db, int studyId);
    void createMissingImages(dtkAbstractData* dtkData, QSqlDatabase db, int seriesId, QStringList thumbPaths);

    medDatabaseImporterPrivate *d;

};

#endif // MEDDATABASEIMPORTER_H
