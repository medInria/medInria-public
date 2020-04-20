/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractDataReader.h>
#include <dtkCoreSupport/dtkAbstractDataWriter.h>
#include <dtkCoreSupport/dtkGlobal.h>
#include <dtkLog/dtkLog.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medAbstractImageData.h>
#include <medDatabaseImporter.h>
#include <medDatabaseController.h>
#include <medMetaDataKeys.h>
#include <medStorage.h>

//-----------------------------------------------------------------------------------------------------------

medDatabaseImporter::medDatabaseImporter ( const QString& file, const QUuid& uuid, bool indexWithoutImporting) :
    medAbstractDatabaseImporter(file, uuid, indexWithoutImporting)
{

}

//-----------------------------------------------------------------------------------------------------------

medDatabaseImporter::medDatabaseImporter ( medAbstractData* medData, const QUuid& uuid ) :
    medAbstractDatabaseImporter(medData, uuid)
{

}

//-----------------------------------------------------------------------------------------------------------

medDatabaseImporter::~medDatabaseImporter ( void )
{

}

//-----------------------------------------------------------------------------------------------------------

/**
 * Retrieves patientID. Checks if patient is already in the database
 * if so, returns his Id, otherwise creates a new guid
 */
QString medDatabaseImporter::getPatientID(QString patientName, QString birthDate)
{
    QString patientID = "";
    //Let's see if the patient is already in the db
    QSqlQuery query ( medDatabaseController::instance()->database() );

    query.prepare ( "SELECT patientId FROM patient WHERE name = :name AND birthdate = :birthdate" );
    query.bindValue ( ":name", patientName );
    query.bindValue ( ":birthdate", birthDate );

    if ( !query.exec() )
    {
        dtkDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if ( query.first() )
        patientID = query.value ( 0 ).toString();
    else
        patientID = QUuid::createUuid().toString().replace("{","").replace("}","");

    return patientID;
}

//-----------------------------------------------------------------------------------------------------------
/**
* Populates database tables and generates thumbnails.
* @param medData - a @medAbstractData object created from the original image
* @param pathToStoreThumbnails - path where the thumbnails will be stored
* @return medDataIndex the new medDataIndex associated with this imported series.
**/
medDataIndex medDatabaseImporter::populateDatabaseAndGenerateThumbnails ( medAbstractData* medData, QString pathToStoreThumbnail )
{
    QSqlDatabase db = medDatabaseController::instance()->database();

    generateThumbnail ( medData, pathToStoreThumbnail );

    int patientDbId = getOrCreatePatient ( medData, db );

    int studyDbId = getOrCreateStudy ( medData, db, patientDbId );

    int seriesDbId = getOrCreateSeries ( medData, db, studyDbId );

    medDataIndex index = medDataIndex ( medDatabaseController::instance()->dataSourceId(), patientDbId, studyDbId, seriesDbId );
    return index;
}

//-----------------------------------------------------------------------------------------------------------
/**
 * Retrieves the patient id of the existent (or newly created)
 * patient record in the patient table.
 */
int medDatabaseImporter::getOrCreatePatient ( const medAbstractData* medData, QSqlDatabase db )
{
    int patientDbId = -1;

    QSqlQuery query ( db );

    QString patientName = medMetaDataKeys::PatientName.getFirstValue(medData).simplified();
    QString birthDate = medMetaDataKeys::BirthDate.getFirstValue(medData);
    QString patientId = medMetaDataKeys::PatientID.getFirstValue(medData);

    query.prepare ( "SELECT id FROM patient WHERE name = :name AND birthdate = :birthdate" );
    query.bindValue ( ":name", patientName );
    query.bindValue ( ":birthdate", birthDate );

    if ( !query.exec() )
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if ( query.first() )
    {
        patientDbId = query.value ( 0 ).toInt();
    }
    else
    {
        QString birthdate      = medMetaDataKeys::BirthDate.getFirstValue(medData);
        QString gender         = medMetaDataKeys::Gender.getFirstValue(medData);

        query.prepare ( "INSERT INTO patient (name, thumbnail, birthdate, gender, patientId) VALUES (:name, :thumbnail, :birthdate, :gender, :patientId)" );
        query.bindValue ( ":name", patientName );
        query.bindValue ( ":thumbnail", QString("") );
        query.bindValue ( ":birthdate", birthdate );
        query.bindValue ( ":gender",    gender );
        query.bindValue ( ":patientId", patientId);
        query.exec();

        patientDbId = query.lastInsertId().toInt();
    }

    return patientDbId;
}

//-----------------------------------------------------------------------------------------------------------
/**
 * Retrieves the study id of the existent (or newly created)
 * study record in the study table.
 */
int medDatabaseImporter::getOrCreateStudy ( const medAbstractData* medData, QSqlDatabase db, int patientDbId )
{
    int studyDbId = -1;

    QSqlQuery query ( db );

    QString studyName   = medMetaDataKeys::StudyDescription.getFirstValue(medData).simplified();
    QString studyUid    = medMetaDataKeys::StudyInstanceUID.getFirstValue(medData);
    QString studyId    = medMetaDataKeys::StudyID.getFirstValue(medData);

    QString seriesName   = medMetaDataKeys::SeriesDescription.getFirstValue(medData).simplified();

    if( studyName=="EmptyStudy" && seriesName=="EmptySeries" )
        return studyDbId;

    query.prepare ( "SELECT id FROM study WHERE patient = :patient AND name = :studyName AND uid = :studyUid" );
    query.bindValue ( ":patient", patientDbId );
    query.bindValue ( ":studyName", studyName );
    query.bindValue ( ":studyUid", studyUid );

    if ( !query.exec() )
    {
        dtkDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if ( query.first() )
    {
        studyDbId = query.value ( 0 ).toInt();
    }
    else
    {
        QString refThumbPath = medMetaDataKeys::ThumbnailPath.getFirstValue(medData);

        query.prepare ( "INSERT INTO study (patient, name, uid, thumbnail, studyId) "
                        "VALUES (:patient, :studyName, :studyUid, :thumbnail, :studyId)" );
        query.bindValue ( ":patient", patientDbId );
        query.bindValue ( ":studyName", studyName );
        query.bindValue ( ":studyUid", studyUid );
        query.bindValue ( ":thumbnail", refThumbPath );
        query.bindValue ( ":studyId", studyId);

        query.exec();

        studyDbId = query.lastInsertId().toInt();
    }

    return studyDbId;
}

//-----------------------------------------------------------------------------------------------------------
/**
 * Retrieves the series id of the existent (or newly created)
 * series record in the series table.
 */
int medDatabaseImporter::getOrCreateSeries ( const medAbstractData* medData, QSqlDatabase db, int studyDbId )
{
    int seriesDbId = -1;

    QSqlQuery query ( db );

    QString seriesName     = medMetaDataKeys::SeriesDescription.getFirstValue(medData).simplified();
    QString seriesUid      = medMetaDataKeys::SeriesInstanceUID.getFirstValue(medData);
    QString seriesId       = medMetaDataKeys::SeriesID.getFirstValue(medData);

    QString orientation    = medMetaDataKeys::Orientation.getFirstValue(medData);
    QString seriesNumber   = medMetaDataKeys::SeriesNumber.getFirstValue(medData);
    QString sequenceName   = medMetaDataKeys::SequenceName.getFirstValue(medData);
    QString sliceThickness = medMetaDataKeys::SliceThickness.getFirstValue(medData);
    QString rows           = medMetaDataKeys::Rows.getFirstValue(medData);
    QString columns        = medMetaDataKeys::Columns.getFirstValue(medData);

    if( seriesName=="EmptySeries" )
        return seriesDbId;

    query.prepare ( "SELECT * FROM series WHERE study = :study AND name = :seriesName AND uid = :seriesUid AND orientation = :orientation AND seriesNumber = :seriesNumber AND sequenceName = :sequenceName AND sliceThickness = :sliceThickness AND rows = :rows AND columns = :columns" );
    query.bindValue ( ":study", studyDbId );
    query.bindValue ( ":seriesName", seriesName );
    query.bindValue ( ":seriesUid", seriesUid );
    query.bindValue ( ":orientation", orientation );
    query.bindValue ( ":seriesNumber", seriesNumber );
    query.bindValue ( ":sequenceName", sequenceName );
    query.bindValue ( ":sliceThickness", sliceThickness );
    query.bindValue ( ":rows", rows );
    query.bindValue ( ":columns", columns );

    if( seriesName=="EmptySeries" )
        return seriesDbId;

    if ( !query.exec() )
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if ( query.first() )
    {
        seriesDbId = query.value ( 0 ).toInt();
    }
    else
    {
        // if we are creating a new series while indexing then we need to empty
        // the column 'path', as there won't be a file aggregating the images

        QString seriesPath = "";
        if ( !indexWithoutImporting() )
        {
            QStringList fileNames  = medData->metaDataValues( "FileName" );
            seriesPath = fileNames.count()>0 ? fileNames[0] : "" ;
        }
        int size               = medMetaDataKeys::Size.getFirstValue(medData).toInt();
        QString refThumbPath   = medMetaDataKeys::ThumbnailPath.getFirstValue(medData);
        QString age            = medMetaDataKeys::Age.getFirstValue(medData);
        QString description    = medMetaDataKeys::Description.getFirstValue(medData);
        QString modality       = medMetaDataKeys::Modality.getFirstValue(medData);
        QString protocol       = medMetaDataKeys::Protocol.getFirstValue(medData);
        QString comments       = medMetaDataKeys::Comments.getFirstValue(medData);
        QString status         = medMetaDataKeys::Status.getFirstValue(medData);
        QString acqdate        = medMetaDataKeys::AcquisitionDate.getFirstValue(medData);
        QString importdate     = medMetaDataKeys::ImportationDate.getFirstValue(medData);
        QString referee        = medMetaDataKeys::Referee.getFirstValue(medData);
        QString performer      = medMetaDataKeys::Performer.getFirstValue(medData);
        QString institution    = medMetaDataKeys::Institution.getFirstValue(medData);
        QString report         = medMetaDataKeys::Report.getFirstValue(medData);
        QString origin          = medMetaDataKeys::Origin.getFirstValue(medData);
        QString flipAngle       = medMetaDataKeys::FlipAngle.getFirstValue(medData);
        QString echoTime        = medMetaDataKeys::EchoTime.getFirstValue(medData);
        QString repetitionTime  = medMetaDataKeys::RepetitionTime.getFirstValue(medData);
        QString acquisitionTime = medMetaDataKeys::AcquisitionTime.getFirstValue(medData);

        query.prepare ( "INSERT INTO series (study, seriesId, size, name, path, uid, "
                        "orientation, seriesNumber, sequenceName, sliceThickness, rows, columns, "
                        "thumbnail, age, description, modality, protocol, comments, "
                        "status, acquisitiondate, importationdate, referee, performer, institution, report, "
                        "origin, flipAngle, echoTime, repetitionTime, acquisitionTime) "
                        "VALUES (:study, :seriesId, :size, :seriesName, :seriesPath, :seriesUid, "
                        ":orientation, :seriesNumber, :sequenceName, :sliceThickness, :rows, :columns, "
                        ":thumbnail, :age, :description, :modality, :protocol, :comments, "
                        ":status, :acquisitiondate, :importationdate, :referee, :performer, :institution, :report, "
                        ":origin, :flipAngle, :echoTime, :repetitionTime, :acquisitionTime)" );

        query.bindValue ( ":study",          studyDbId );
        query.bindValue ( ":seriesId",       seriesId );
        query.bindValue ( ":size",           size );
        query.bindValue ( ":seriesName",     seriesName );
        query.bindValue ( ":seriesPath",     seriesPath );
        query.bindValue ( ":seriesUid",      seriesUid );
        query.bindValue ( ":orientation",    orientation );
        query.bindValue ( ":seriesNumber",   seriesNumber );
        query.bindValue ( ":sequenceName",   sequenceName );
        query.bindValue ( ":sliceThickness", sliceThickness );
        query.bindValue ( ":rows",           rows );
        query.bindValue ( ":columns",        columns );
        query.bindValue ( ":thumbnail",      refThumbPath );
        query.bindValue ( ":age",            age );
        query.bindValue ( ":description",    description );
        query.bindValue ( ":modality",       modality );
        query.bindValue ( ":protocol",       protocol );
        query.bindValue ( ":comments",       comments );
        query.bindValue ( ":status",         status );
        query.bindValue ( ":acquisitiondate",acqdate );
        query.bindValue ( ":importationdate",importdate );
        query.bindValue ( ":referee",        referee );
        query.bindValue ( ":performer",      performer );
        query.bindValue ( ":institution",    institution );
        query.bindValue ( ":report",         report );
        query.bindValue ( ":origin",           origin );
        query.bindValue ( ":flipAngle",        flipAngle );
        query.bindValue ( ":echoTime",         echoTime );
        query.bindValue ( ":repetitionTime",   repetitionTime );
        query.bindValue ( ":acquisitionTime",  acquisitionTime );

        if ( !query.exec() )
        {
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        }

        seriesDbId = query.lastInsertId().toInt();
    }

    return seriesDbId;
}

//-----------------------------------------------------------------------------------------------------------
/**
* Finds if parameter @seriesName is already being used in the database
* if is not, it returns @seriesName unchanged
* otherwise, it returns an unused new series name (created by adding a suffix)
* @param seriesName - the series name
* @return newSeriesName - a new, unused, series name
**/
QString medDatabaseImporter::ensureUniqueSeriesName ( const QString seriesName )
{
    QSqlDatabase db = medDatabaseController::instance()->database();

    QSqlQuery query ( db );
    query.prepare ( "SELECT name FROM series WHERE name LIKE '" + seriesName + "%'" );

    if ( !query.exec() )
    {
        dtkDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    QStringList seriesNames;
    while (query.next())
    {
        QString sname = query.value(0).toString();
        seriesNames << sname;
    }

    QString originalSeriesName = seriesName;
    QString newSeriesName = seriesName;

    int suffix = 0;
    while (seriesNames.contains(newSeriesName))
    {
        // it exist
        suffix++;
        newSeriesName = originalSeriesName + "_" + QString::number(suffix);
    }

    return newSeriesName;
}
