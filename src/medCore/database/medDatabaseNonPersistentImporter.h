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

#include <QtCore/QObject>

#include <medDataIndex.h>
#include <medAbstractDatabaseImporter.h>

class medAbstractData;

/**
* @brief Reads files and adds them to the medNonPersistentDatabase.
*
* This class inherits from medAbstractJob and is meant to be run by the medJobManager.
*
*/
class MEDCORE_EXPORT medDatabaseNonPersistentImporter : public medAbstractDatabaseImporter
{
    Q_OBJECT

public:
    /**
    * @brief Constructor.
    *
    * @param file the file or directory to be read.
    * @param callerUuid The string representation of a unique identifier. The caller will react to link the final signal with this id to know whether it should react to it or not.
    */
    medDatabaseNonPersistentImporter(const QString& file, const QUuid &uuid);
    medDatabaseNonPersistentImporter(medAbstractData* medData, const QUuid& uuid);
    ~medDatabaseNonPersistentImporter();

public:
    QString getPatientID(QString patientName, QString birthDate);
    medDataIndex populateDatabaseAndGenerateThumbnails ( medAbstractData* medData, QString pathToStoreThumbnails );
    QString ensureUniqueSeriesName ( const QString seriesName );
    bool checkIfExists ( medAbstractData* medData, QString imageName );

private:

};



