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

#include <QSqlDatabase>

#include <medAbstractDatabaseImporter.h>
#include <medDataIndex.h>

class medAbstractData;

/**
* @class medDatabaseImporter
* @brief Threaded importing/indexing of files or directories into medInria database.
* @medDatabaseImporter is in charge of importing (or indexing, any word will be used
* hereafter) items into medInria database.
* It is designed to run as a thread, to know how use it check the documentation
* of @medAbstractJob.
* Images can be imported, that means that are not only indexed but also copied in
* medInria database (and as a result they can end up being aggregated by volume)
* or they can be just indexed (by indicating so using the parameters in the constructor)
**/
class MEDCORE_EXPORT medDatabaseImporter : public medAbstractDatabaseImporter
{
    Q_OBJECT

public:
    medDatabaseImporter ( const QString& file, const QUuid& uuid, bool indexWithoutImporting = false);
    medDatabaseImporter ( medAbstractData* medData, const QUuid& callerUuid );
    ~medDatabaseImporter ( void );


private:

    QString ensureUniqueSeriesName ( const QString seriesName );

    bool checkIfExists ( medAbstractData* medData, QString imageName );

    medDataIndex populateDatabaseAndGenerateThumbnails ( medAbstractData* medData, QString pathToStoreThumbnails );

    int getOrCreatePatient ( const medAbstractData* medData, QSqlDatabase db );
    int getOrCreateStudy ( const medAbstractData* medData, QSqlDatabase db, int patientId );
    int getOrCreateSeries ( const medAbstractData* medData, QSqlDatabase db, int studyId );

    void createMissingImages ( medAbstractData* medData, QSqlDatabase db, int seriesId, QStringList thumbPaths );

    QString getPatientID(QString patientName, QString birthDate);

};


