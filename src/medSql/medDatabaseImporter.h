/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifndef MEDDATABASEIMPORTER_H
#define MEDDATABASEIMPORTER_H

#include "medSqlExport.h"

#include <QtCore>
#include <QtSql>

#include <dtkCore/dtkSmartPointer.h>

#include <medAbstractDatabaseImporter.h>
#include <medDataIndex.h>

class medDatabaseImporterPrivate;
class dtkAbstractData;
class QFileInfo;
class dtkAbstractDataReader;
class dtkAbstractDataWriter;

/**
* @class medDatabaseImporter
* @brief Threaded importing/indexing of files or directories into medInria database.
* @medDatabaseImporter is in charge of importing (or indexing, any word will be used
* hereafter) items into medInria database.
* It is designed to run as a thread, to know how use it check the documentation
* of @medJobItem.
* Images can be imported, that means that are not only indexed but also copied in
* medInria database (and as a result they can end up being aggregated by volume)
* or they can be just indexed (by indicating so using the parameters in the constructor)
**/
class MEDSQL_EXPORT medDatabaseImporter : public medAbstractDatabaseImporter
{
    Q_OBJECT

public:
    medDatabaseImporter ( const QString& file, bool indexWithoutImporting = false, const QString& callerUuid = QString() );
    ~medDatabaseImporter ( void );


private:

    /**
    * Finds if parameter @seriesName is already being used in the database
    * if is not, it returns @seriesName unchanged
    * otherwise, it returns an unused new series name (created by adding a suffix)
    * @param seriesName - the series name
    * @return newSeriesName - a new, unused, series name
    **/
    QString ensureUniqueSeriesName ( const QString seriesName );

    /**
    * Checks if the image which was used to create the dtkData object
    * passed as parameter already exists in the database
    * @param dtkData - a @dtkAbstractData object created from the original image
    * @param imageName - the name of the image we are looking for
    * @return true if already exists, false otherwise
    **/
    bool checkIfExists ( dtkAbstractData* dtkdata, QString imageName );

    /**
    * Populates database tables and generates thumbnails.
    * @param dtkData - a @dtkAbstractData object created from the original image
    * @param pathToStoreThumbnails - path where the thumbnails will be stored
    * @return medDataIndex the new medDataIndex associated with this imported series.
    **/
    medDataIndex populateDatabaseAndGenerateThumbnails ( dtkAbstractData* dtkData, QString pathToStoreThumbnails );

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
     * Retrieves the patient id of the existent (or newly created)
     * patient record in the patient table.
     */
    int getOrCreatePatient ( const dtkAbstractData* dtkData, QSqlDatabase db );

    /**
     * Retrieves the study id of the existent (or newly created)
     * study record in the study table.
     */
    int getOrCreateStudy ( const dtkAbstractData* dtkData, QSqlDatabase db, int patientId );

    /**
     * Retrieves the series id of the existent (or newly created)
     * series record in the series table.
     */
    int getOrCreateSeries ( const dtkAbstractData* dtkData, QSqlDatabase db, int studyId );

    /**
     * Creates records in the image table for the files we are importing/indexing.
     */
    void createMissingImages ( dtkAbstractData* dtkData, QSqlDatabase db, int seriesId, QStringList thumbPaths );

    /**
     * Checks if the user is trying to perform a partial import
     * (that is, trying to import files belonging to the same volume
     * in 2 different steps).
     */
    bool isPartialImportAttempt ( dtkAbstractData* dtkData );
    
    /**
     * Retrieves patientID. Checks if patient is already in the database
     * if so, returns his Id, otherwise creates a new guid
     */
    QString getPatientID(QString patientName, QString birthDate);

};

#endif // MEDDATABASEIMPORTER_H
