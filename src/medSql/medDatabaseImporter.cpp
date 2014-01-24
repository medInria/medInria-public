/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseImporter.h"

#include <medAbstractDataImage.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <medAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>
#include <medDatabaseController.h>
#include <medMetaDataKeys.h>
#include <medStorage.h>



//-----------------------------------------------------------------------------------------------------------

medDatabaseImporter::medDatabaseImporter ( const QString& file, bool indexWithoutImporting, const QString& callerUuid ) : medAbstractDatabaseImporter(file,indexWithoutImporting, callerUuid)
{

}

//-----------------------------------------------------------------------------------------------------------

medDatabaseImporter::medDatabaseImporter ( medAbstractData* dtkData, const QString& callerUuid ) : medAbstractDatabaseImporter(dtkData,false, callerUuid)
{

}

//-----------------------------------------------------------------------------------------------------------

medDatabaseImporter::~medDatabaseImporter ( void )
{

}

//-----------------------------------------------------------------------------------------------------------


QString medDatabaseImporter::getPatientID(QString patientName, QString birthDate)
{
    QString patientID = "";
    //Let's see if the patient is already in the db
    QSqlDatabase db = * ( medDatabaseController::instance()->database() );
    QSqlQuery query ( db );

    query.prepare ( "SELECT patientId FROM patient WHERE name = :name AND birthdate = :birthdate" );
    query.bindValue ( ":name", patientName );
    query.bindValue ( ":birthdate", birthDate );

    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if ( query.first() )
        patientID = query.value ( 0 ).toString();
    else
        patientID = QUuid::createUuid().toString().replace("{","").replace("}","");

    return patientID;
}

//-----------------------------------------------------------------------------------------------------------

bool medDatabaseImporter::isPartialImportAttempt ( medAbstractData* dtkData )
{
    // here we check if the series we try to import is already in the database

    // if the image didn't provide basic information like patient, study or series
    // we cannot tell whether we are importing the same file twice or
    // if we want to import now a file with a path that was previously imported before
    // see http://pm-med.inria.fr/issues/292 for more details
    QString containsBasicInfo = medMetaDataKeys::ContainsBasicInfo.getFirstValue(dtkData);
    if (containsBasicInfo.compare("true", Qt::CaseInsensitive) != 0)
        return false;

    QSqlDatabase db = * ( medDatabaseController::instance()->database() );
    QSqlQuery query ( db );

    QString patientName = medMetaDataKeys::PatientName.getFirstValue(dtkData).simplified();

    query.prepare ( "SELECT id FROM patient WHERE name = :name" );
    query.bindValue ( ":name", patientName );

    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if ( query.first() )
    {
        int patientDbId = query.value ( 0 ).toInt();

        query.clear();

        QString studyName = medMetaDataKeys::StudyDescription.getFirstValue(dtkData).simplified();
        QString studyUid = medMetaDataKeys::StudyDicomID.getFirstValue(dtkData);

        query.prepare ( "SELECT id FROM study WHERE patient = :patient AND name = :studyName AND uid = :studyUid" );
        query.bindValue ( ":patient", patientDbId );
        query.bindValue ( ":studyName", studyName );
        query.bindValue ( ":studyUid", studyUid );

        if ( !query.exec() )
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

        if ( query.first() )
        {
            int studyDbId = query.value ( 0 ).toInt();

            query.clear();

            QString seriesName = medMetaDataKeys::SeriesDescription.getFirstValue(dtkData).simplified();
            QString seriesUid = medMetaDataKeys::SeriesDicomID.getFirstValue(dtkData);
            QString orientation = medMetaDataKeys::Orientation.getFirstValue(dtkData);
            QString seriesNumber = medMetaDataKeys::SeriesNumber.getFirstValue(dtkData);
            QString sequenceName = medMetaDataKeys::SequenceName.getFirstValue(dtkData);
            QString sliceThickness = medMetaDataKeys::SliceThickness.getFirstValue(dtkData);
            QString rows = medMetaDataKeys::Rows.getFirstValue(dtkData);
            QString columns = medMetaDataKeys::Columns.getFirstValue(dtkData);

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

            if ( !query.exec() )
                qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

            if ( query.first() )
            {
                QStringList filePaths = dtkData->metaDataValues ( medMetaDataKeys::FilePaths.key() );
                *(partialAttemptsInfo()) << ( QStringList() << patientName << studyName << seriesName << filePaths[0] );
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------------------------------------


bool medDatabaseImporter::checkIfExists ( medAbstractData* dtkdata, QString imageName )
{
    bool imageExists = false;

    QSqlQuery query ( * ( medDatabaseController::instance()->database() ) );

    // first we query patient table
    QString patientName = medMetaDataKeys::PatientName.getFirstValue(dtkdata);
    QString birthDate = medMetaDataKeys::BirthDate.getFirstValue(dtkdata);

    query.prepare ( "SELECT id FROM patient WHERE name = :name AND birthDate = :birthdate");
    query.bindValue ( ":name", patientName );
    query.bindValue ( ":birthdate", birthDate );

    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if ( query.first() )
    {
        QVariant patientDbId = query.value ( 0 );
        // if patient already exists we now verify the study

        QString studyName = medMetaDataKeys::StudyDescription.getFirstValue(dtkdata);
        QString studyUid  = medMetaDataKeys::StudyDicomID.getFirstValue(dtkdata);

        query.prepare ( "SELECT id FROM study WHERE patient = :patient AND name = :name AND uid = :studyID" );
        query.bindValue ( ":patient", patientDbId );
        query.bindValue ( ":name", studyName );
        query.bindValue ( ":studyUid", studyUid );

        if ( !query.exec() )
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

        if ( query.first() )
        {
            QVariant studyDbId = query.value ( 0 );
            // both patient and study exists, let's check series
            QString seriesName  = medMetaDataKeys::SeriesDescription.getFirstValue(dtkdata);
            QString seriesUid = medMetaDataKeys::SeriesDicomID.getFirstValue(dtkdata);
            QString orientation = medMetaDataKeys::Orientation.getFirstValue(dtkdata); // orientation sometimes differ by a few digits, thus this is not reliable
            QString seriesNumber = medMetaDataKeys::SeriesNumber.getFirstValue(dtkdata);
            QString sequenceName = medMetaDataKeys::SequenceName.getFirstValue(dtkdata);
            QString sliceThickness = medMetaDataKeys::SliceThickness.getFirstValue(dtkdata);
            QString rows = medMetaDataKeys::Rows.getFirstValue(dtkdata);
            QString columns = medMetaDataKeys::Columns.getFirstValue(dtkdata);

            query.prepare ( "SELECT id FROM series WHERE study = :study AND name = :name AND uid = :seriesUid AND orientation = :orientation AND seriesNumber = :seriesNumber AND sequenceName = :sequenceName AND sliceThickness = :sliceThickness AND rows = :rows AND columns = :columns" );
            query.bindValue ( ":study", studyDbId );
            query.bindValue ( ":name", seriesName );
            query.bindValue ( ":seriesUid", seriesUid );
            query.bindValue ( ":orientation", orientation );
            query.bindValue ( ":seriesNumber", seriesNumber );
            query.bindValue ( ":sequenceName", sequenceName );
            query.bindValue ( ":sliceThickness", sliceThickness );
            query.bindValue ( ":rows", rows );
            query.bindValue ( ":columns", columns );

            if ( !query.exec() )
                qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

            if ( query.first() )
            {
                QVariant seriesDbId = query.value ( 0 );

                // finally we check the image table

                query.prepare ( "SELECT id FROM image WHERE series = :series AND name = :name" );
                query.bindValue ( ":series", seriesDbId );
                query.bindValue ( ":name", imageName );

                if ( !query.exec() )
                    qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

                if ( query.first() )
                {
                    imageExists = true;
                }
            }
        }
    }
    return imageExists;
}

//-----------------------------------------------------------------------------------------------------------

medDataIndex medDatabaseImporter::populateDatabaseAndGenerateThumbnails ( medAbstractData* dtkData, QString pathToStoreThumbnails )
{
    QSqlDatabase db = * ( medDatabaseController::instance()->database() );

    QStringList thumbPaths = generateThumbnails ( dtkData, pathToStoreThumbnails );

    int patientDbId = getOrCreatePatient ( dtkData, db );

    int studyDbId = getOrCreateStudy ( dtkData, db, patientDbId );

    int seriesDbId = getOrCreateSeries ( dtkData, db, studyDbId );

    createMissingImages ( dtkData, db, seriesDbId, thumbPaths );

    medDataIndex index = medDataIndex ( medDatabaseController::instance()->dataSourceId(), patientDbId, studyDbId, seriesDbId, -1 );
    return index;
}

//-----------------------------------------------------------------------------------------------------------

QStringList medDatabaseImporter::generateThumbnails ( medAbstractData* dtkData, QString pathToStoreThumbnails )
{
    QList<QImage> &thumbnails = dtkData->thumbnails();

    QStringList thumbPaths;

    if ( !medStorage::mkpath ( medStorage::dataLocation() + pathToStoreThumbnails ) )
        qDebug() << "Cannot create directory: " << pathToStoreThumbnails;

    for ( int i=0; i < thumbnails.count(); i++ )
    {
        QString thumb_name = pathToStoreThumbnails + QString().setNum ( i ) + ".png";
        thumbnails[i].save ( medStorage::dataLocation() + thumb_name, "PNG" );
        thumbPaths << thumb_name;
    }

    QImage refThumbnail = dtkData->thumbnail(); // representative thumbnail for PATIENT/STUDY/SERIES
    QString refThumbPath = pathToStoreThumbnails + "ref.png";
    refThumbnail.save ( medStorage::dataLocation() + refThumbPath, "PNG" );

    dtkData->addMetaData ( medMetaDataKeys::ThumbnailPath.key(), refThumbPath );

    return thumbPaths;
}

//-----------------------------------------------------------------------------------------------------------

int medDatabaseImporter::getOrCreatePatient ( const medAbstractData* dtkData, QSqlDatabase db )
{
    int patientDbId = -1;

    QSqlQuery query ( db );

    QString patientName = medMetaDataKeys::PatientName.getFirstValue(dtkData).simplified();
    QString birthDate = medMetaDataKeys::BirthDate.getFirstValue(dtkData);
    QString patientId = medMetaDataKeys::PatientID.getFirstValue(dtkData);

    query.prepare ( "SELECT id FROM patient WHERE name = :name AND birthdate = :birthdate" );
    query.bindValue ( ":name", patientName );
    query.bindValue ( ":birthdate", birthDate );

    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if ( query.first() )
    {
        patientDbId = query.value ( 0 ).toInt();
    }
    else
    {
        QString refThumbPath   = medMetaDataKeys::ThumbnailPath.getFirstValue(dtkData);
        QString birthdate      = medMetaDataKeys::BirthDate.getFirstValue(dtkData);
        QString gender         = medMetaDataKeys::Gender.getFirstValue(dtkData);

        query.prepare ( "INSERT INTO patient (name, thumbnail, birthdate, gender, patientId) VALUES (:name, :thumbnail, :birthdate, :gender, :patientId)" );
        query.bindValue ( ":name", patientName );
        
        // actually, in the database preview, thumbnails are retrieved from the series and not from this field
        // when this field is set, it can causes problems when moving studies or series and deleting a patient
        //query.bindValue ( ":thumbnail", refThumbPath );        
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

int medDatabaseImporter::getOrCreateStudy ( const medAbstractData* dtkData, QSqlDatabase db, int patientDbId )
{
    int studyDbId = -1;

    QSqlQuery query ( db );

    QString studyName   = medMetaDataKeys::StudyDescription.getFirstValue(dtkData).simplified();
    QString studyUid    = medMetaDataKeys::StudyDicomID.getFirstValue(dtkData);
    QString studyId    = medMetaDataKeys::StudyID.getFirstValue(dtkData);
    
    QString serieName   = medMetaDataKeys::SeriesDescription.getFirstValue(dtkData).simplified();
    
    if( studyName=="EmptyStudy" && serieName=="EmptySerie" )
        return studyDbId; 

    query.prepare ( "SELECT id FROM study WHERE patient = :patient AND name = :studyName AND uid = :studyUid" );
    query.bindValue ( ":patient", patientDbId );
    query.bindValue ( ":studyName", studyName );
    query.bindValue ( ":studyUid", studyUid );

    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if ( query.first() )
    {
        studyDbId = query.value ( 0 ).toInt();
    }
    else
    {
        QString refThumbPath = medMetaDataKeys::ThumbnailPath.getFirstValue(dtkData);

        query.prepare ( "INSERT INTO study (patient, name, uid, thumbnail, studyId) VALUES (:patient, :studyName, :studyUid, :thumbnail, :studyId)" );
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

int medDatabaseImporter::getOrCreateSeries ( const medAbstractData* dtkData, QSqlDatabase db, int studyDbId )
{
    int seriesDbId = -1;

    QSqlQuery query ( db );

    QString seriesName     = medMetaDataKeys::SeriesDescription.getFirstValue(dtkData).simplified();
    QString seriesUid      = medMetaDataKeys::SeriesDicomID.getFirstValue(dtkData);
    QString seriesId       = medMetaDataKeys::SeriesID.getFirstValue(dtkData);

    QString orientation    = medMetaDataKeys::Orientation.getFirstValue(dtkData);
    QString seriesNumber   = medMetaDataKeys::SeriesNumber.getFirstValue(dtkData);
    QString sequenceName   = medMetaDataKeys::SequenceName.getFirstValue(dtkData);
    QString sliceThickness = medMetaDataKeys::SliceThickness.getFirstValue(dtkData);
    QString rows           = medMetaDataKeys::Rows.getFirstValue(dtkData);
    QString columns        = medMetaDataKeys::Columns.getFirstValue(dtkData);
    
    if( seriesName=="EmptySerie" )
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
    
    if( seriesName=="EmptySerie" )
        return seriesDbId; 

    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

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
            QStringList fileNames  = dtkData->metaDataValues( "FileName" );
            seriesPath = fileNames.count()>0 ? fileNames[0] : "" ;
        }
        int size               = medMetaDataKeys::Size.getFirstValue(dtkData).toInt();
        QString refThumbPath   = medMetaDataKeys::ThumbnailPath.getFirstValue(dtkData);
        QString age            = medMetaDataKeys::Age.getFirstValue(dtkData);
        QString description    = medMetaDataKeys::Description.getFirstValue(dtkData);
        QString modality       = medMetaDataKeys::Modality.getFirstValue(dtkData);
        QString protocol       = medMetaDataKeys::Protocol.getFirstValue(dtkData);
        QString comments       = medMetaDataKeys::Comments.getFirstValue(dtkData);
        QString status         = medMetaDataKeys::Status.getFirstValue(dtkData);
        QString acqdate        = medMetaDataKeys::AcquisitionDate.getFirstValue(dtkData);
        QString importdate     = medMetaDataKeys::ImportationDate.getFirstValue(dtkData);
        QString referee        = medMetaDataKeys::Referee.getFirstValue(dtkData);
        QString performer      = medMetaDataKeys::Performer.getFirstValue(dtkData);
        QString institution    = medMetaDataKeys::Institution.getFirstValue(dtkData);
        QString report         = medMetaDataKeys::Report.getFirstValue(dtkData);

        query.prepare ( "INSERT INTO series (study, seriesId, size, name, path, uid, orientation, seriesNumber, sequenceName, sliceThickness, rows, columns, thumbnail, age, description, modality, protocol, comments, status, acquisitiondate, importationdate, referee, performer, institution, report) \
                                     VALUES (:study, :seriesId, :size, :seriesName, :seriesPath, :seriesUid, :orientation, :seriesNumber, :sequenceName, :sliceThickness, :rows, :columns, :refThumbPath, :age, :description, :modality, :protocol, :comments, :status, :acquisitiondate, :importationdate, :referee, :performer, :institution, :report)" );
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

        if ( !query.exec() )
          qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

        seriesDbId = query.lastInsertId().toInt();
    }

    return seriesDbId;
}

//-----------------------------------------------------------------------------------------------------------

void medDatabaseImporter::createMissingImages ( medAbstractData* dtkData, QSqlDatabase db, int seriesDbId, QStringList thumbPaths )
{
    QSqlQuery query ( db );

    QStringList filePaths  = dtkData->metaDataValues ( medMetaDataKeys::FilePaths.key() );

    if ( filePaths.count() == 1 && thumbPaths.count() > 1 ) // special case to 1 image and multiple thumbnails
    {
        QString seriesName  = medMetaDataKeys::SeriesDescription.getFirstValue(dtkData);

        QFileInfo fileInfo ( filePaths[0] );
        for ( int i = 0; i < thumbPaths.count(); i++ )
        {
            query.prepare ( "SELECT id FROM image WHERE series = :series AND name = :name" );
            query.bindValue ( ":series", seriesDbId );
            query.bindValue ( ":name", fileInfo.fileName() + QString().setNum ( i ) );

            if ( !query.exec() )
                qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

            if ( query.first() )
            {
                ; //qDebug() << "Image" << file << "already in database";
            }
            else
            {
                QString name_str = seriesName + "." + fileInfo.completeSuffix() + QString().setNum ( i );

                query.prepare ( "INSERT INTO image (series, name, path, instance_path, thumbnail, isIndexed) VALUES (:series, :name, :path, :instance_path, :thumbnail, :isIndexed)" );
                query.bindValue ( ":series", seriesDbId );
                query.bindValue ( ":name", name_str);
                query.bindValue ( ":path", fileInfo.filePath() );
                query.bindValue ( ":thumbnail", thumbPaths[i] );
                query.bindValue ( ":isIndexed", indexWithoutImporting() );

                // if we are indexing we want to leave the 'instance_path' column blank
                // as we will use the full path in 'path' column to load them
                QString relativeFilePath = dtkData->metaDataValues ( "FileName" ) [0];
                query.bindValue ( ":instance_path", indexWithoutImporting() ? "" : relativeFilePath );

                if ( !query.exec() )
                    qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
            }
        }
    }
    else
    {
        for ( int i = 0; i < filePaths.count(); i++ )
        {
            QFileInfo fileInfo ( filePaths[i] );

            query.prepare ( "SELECT id FROM image WHERE series = :series AND name = :name" );
            query.bindValue ( ":series", seriesDbId );
            query.bindValue ( ":name", fileInfo.fileName() );

            if ( !query.exec() )
                qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

            if ( query.first() )
            {
                ; //qDebug() << "Image" << file << "already in database";
            }
            else
            {
                query.prepare ( "INSERT INTO image (series, name, path, instance_path, thumbnail, isIndexed) VALUES (:series, :name, :path, :instance_path, :thumbnail, :isIndexed)" );
                query.bindValue ( ":series", seriesDbId );
                query.bindValue ( ":name", fileInfo.fileName() );
                query.bindValue ( ":path", fileInfo.filePath() );
                query.bindValue ( ":isIndexed", indexWithoutImporting() );

                // if we are indexing we want to leave the 'instance_path' column blank
                // as we will use the full path in 'path' column to load them
                QStringList fileNames  = dtkData->metaDataValues ( "FileName" );
                QString relativeFilePath = fileNames.count()>0 ? fileNames[0] : "" ;
                query.bindValue ( ":instance_path", indexWithoutImporting() ? "" : relativeFilePath );

                if ( i < thumbPaths.count() )
                    query.bindValue ( ":thumbnail", thumbPaths[i] );
                else
                    query.bindValue ( ":thumbnail", "" );

                if ( !query.exec() )
                    qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------------------

QString medDatabaseImporter::ensureUniqueSeriesName ( const QString seriesName )
{
    QSqlDatabase db = * ( medDatabaseController::instance()->database() );

    QSqlQuery query ( db );
    query.prepare ( "SELECT name FROM series WHERE name LIKE '" + seriesName + "%'" );

    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

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
