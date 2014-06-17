/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medSqlExport.h>

#include <QtCore/QObject>

#include <medDataIndex.h>
#include <medAbstractDatabaseImporter.h>

class medAbstractData;

/**
* @brief Reads files and adds them to the medNonPersistentDatabase.
*
* This class inherits from medJobItem and is meant to be run by the medJobManager.
*
*/
class MEDSQL_EXPORT medDatabaseNonPersistentImporter : public medAbstractDatabaseImporter
{
    Q_OBJECT

public:
    /**
    * @brief Constructor.
    *
    * @param file the file or directory to be read.
    * @param callerUuid The string representation of a unique identifier. The caller will react to link the final signal with this id to know whether it should react to it or not.
    */
    medDatabaseNonPersistentImporter(const QString& file, const QString& callerUuid = QString());
    medDatabaseNonPersistentImporter(medAbstractData* medData, const QString& callerUuid = QString());
    ~medDatabaseNonPersistentImporter();
       
     
public:
     /**
     * Retrieves patientID. Checks if patient is already in the database
     * if so, reurns is Id, otherwise creates a new guid
     */
    QString getPatientID(QString patientName, QString birthDate);
    
     /**
    * Populates database tables and generates thumbnails.
    * @param medData - a @medAbstractData object created from the original image
    * @param pathToStoreThumbnails - path where the thumbnails will be stored
    * @return medDataIndex the new medDataIndex associated with this imported series.
    **/
    medDataIndex populateDatabaseAndGenerateThumbnails ( medAbstractData* medData, QString pathToStoreThumbnails );

     /**
    * Finds if parameter @seriesName is already being used in the database
    * if is not, it returns @seriesName unchanged
    * otherwise, it returns an unused new series name (created by adding a suffix)
    * @param seriesName - the series name
    * @return newSeriesName - a new, unused, series name
    **/
    QString ensureUniqueSeriesName ( const QString seriesName );
    
    /**
    * Checks if the image which was used to create the medData object
    * passed as parameter already exists in the database
    * @param medData - a @medAbstractData object created from the original image
    * @param imageName - the name of the image we are looking for
    * @return true if already exists, false otherwise
    **/
    bool checkIfExists ( medAbstractData* medData, QString imageName );
    
    /**
     * Checks if the user is trying to perform a partial import
     * (that is, trying to import files belonging to the same volume
     * in 2 different steps).
     */
    bool isPartialImportAttempt ( medAbstractData* medData );


private:

};



