/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseRemover.h>

#include <QSqlError>

#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <medAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>

#include <medDatabaseController.h>
#include <medStorage.h>
#include <medDataIndex.h>
#include <medAbstractImageData.h>

#define EXEC_QUERY(q) execQuery(q, __FILE__ , __LINE__ )
namespace
{
    inline bool execQuery ( QSqlQuery & query, const char *file, int line )
    {
        if ( ! query.exec() )
        {
            qDebug() << file << "(" << line << ") :" << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
            return false;
        }
        return true;
    }
}

class medDatabaseRemoverPrivate
{
public:
    medDataIndex index;
    QSqlDatabase db;
    static const QString T_PATIENT;
    static const QString T_STUDY;
    static const QString T_SERIES;
    static const QString T_IMAGE;

    bool isCancelled;
};

const QString medDatabaseRemoverPrivate::T_PATIENT = "patient";
const QString medDatabaseRemoverPrivate::T_STUDY = "study";
const QString medDatabaseRemoverPrivate::T_SERIES = "series";
const QString medDatabaseRemoverPrivate::T_IMAGE = "image";

medDatabaseRemover::medDatabaseRemover ( const medDataIndex &index_ ) : medAbstractJob("Remove data"), d ( new medDatabaseRemoverPrivate )
{
    d->index = index_;
    d->db = medDatabaseController::instance()->database();
    d->isCancelled = false;
}

medDatabaseRemover::~medDatabaseRemover()
{
    delete d;

    d = NULL;
}

void medDatabaseRemover::internalRun()
{

    QSqlDatabase db( d->db );
    QSqlQuery ptQuery ( db );

    const medDataIndex index = d->index;
    if ( index.isValidForPatient() )
    {
        ptQuery.prepare ( "SELECT id FROM " + d->T_PATIENT + " WHERE id = :id" );
        ptQuery.bindValue ( ":id", index.patientId() );
    }
    else
    {
        ptQuery.prepare ( "SELECT id FROM " + d->T_PATIENT );
    }

    EXEC_QUERY ( ptQuery );
    while ( ptQuery.next() )
    {
        if ( d->isCancelled )
            break;

        int patientDbId = ptQuery.value ( 0 ).toInt();
        QSqlQuery stQuery ( db );

        if ( index.isValidForStudy() )
        {
            stQuery.prepare ( "SELECT id FROM " + d->T_STUDY + " WHERE id = :id AND patient = :patient" );
            stQuery.bindValue ( ":id", index.studyId() );
        }
        else
        {
            stQuery.prepare ( "SELECT id FROM " + d->T_STUDY + " WHERE patient = :patient" );
        }
        stQuery.bindValue ( ":patient", patientDbId );

        EXEC_QUERY ( stQuery );
        while ( stQuery.next() )
        {
            if ( d->isCancelled )
                break;

            int studyDbId = stQuery.value ( 0 ).toInt();
            QSqlQuery seQuery ( db );

            if ( index.isValidForSeries() )
            {
                seQuery.prepare ( "SELECT id FROM " + d->T_SERIES + " WHERE id = :id AND study = :study" );
                seQuery.bindValue ( ":id", index.seriesId() );
            }
            else
            {
                seQuery.prepare ( "SELECT id FROM " + d->T_SERIES + " WHERE study = :study" );
            }
            seQuery.bindValue ( ":study", studyDbId );

            EXEC_QUERY ( seQuery );
            while ( seQuery.next() )
            {
                if ( d->isCancelled )
                    break;

                int seriesDbId = seQuery.value ( 0 ).toInt();
                QSqlQuery imQuery ( db );

                if ( index.isValidForImage() )
                {
                    imQuery.prepare ( "SELECT id FROM " + d->T_IMAGE + " WHERE id = :id AND series = :seriesId" );
                    imQuery.bindValue ( ":id", index.imageId() );
                }
                else
                {
                    imQuery.prepare ( "SELECT id FROM " + d->T_IMAGE + " WHERE series = :series" );
                }
                imQuery.bindValue ( ":series", seriesDbId );

                EXEC_QUERY ( imQuery );

                imQuery.last();
                double nbImage = imQuery.at();
                imQuery.first();

                do
                {
                    int imageId = imQuery.value ( 0 ).toInt();
                    this->removeImage ( patientDbId, studyDbId, seriesDbId, imageId );
                    emit progressed (imQuery.at() / nbImage * 100 );
                }
                while ( imQuery.next() );
                if ( this->isSeriesEmpty ( seriesDbId ) )
                    this->removeSeries ( patientDbId, studyDbId, seriesDbId );

            } // seQuery.next
            if ( this->isStudyEmpty ( studyDbId ) )
                this->removeStudy ( patientDbId, studyDbId );

        } // stQuery.next
        if ( this->isPatientEmpty ( patientDbId ) )
            this->removePatient ( patientDbId );

    } // ptQuery.next

    if ( d->isCancelled )
        emit failure ();
    else
        emit success();

    return;
}

void medDatabaseRemover::removeImage ( int patientDbId, int studyDbId, int seriesDbId, int imageId )
{
    QSqlDatabase db(d->db);
    QSqlQuery query ( db );

    query.prepare ( "SELECT thumbnail FROM " + d->T_IMAGE + " WHERE id = :imageId " );
    query.bindValue ( ":id", imageId );
    EXEC_QUERY ( query );
    if ( query.next() )
    {
        QString thumbnail = query.value ( 0 ).toString();
        this->removeFile ( thumbnail );
    }
    removeTableRow ( d->T_IMAGE, imageId );
}

bool medDatabaseRemover::isSeriesEmpty ( int seriesDbId )
{
    QSqlDatabase db(d->db);
    QSqlQuery query ( db );

    query.prepare ( "SELECT id FROM " + d->T_IMAGE + " WHERE series = :series " );
    query.bindValue ( ":series", seriesDbId );
    EXEC_QUERY ( query );
    return !query.next();
}

void medDatabaseRemover::removeSeries ( int patientDbId, int studyDbId, int seriesDbId )
{
    QSqlDatabase db(d->db);
    QSqlQuery query ( db );

    query.prepare ( "SELECT thumbnail, path, name  FROM " + d->T_SERIES + " WHERE id = :series " );
    query.bindValue ( ":series", seriesDbId );
    EXEC_QUERY ( query );

    QString thumbnail;
    if ( query.next() )
    {
        thumbnail = query.value ( 0 ).toString();
        this->removeFile ( thumbnail );
        QString path = query.value ( 1 ).toString();

        // if path is empty then it was an indexed series
        if ( !path.isNull() && !path.isEmpty() )
            this->removeDataFile ( medDataIndex::makeSeriesIndex ( d->index.dataSourceId(), patientDbId, studyDbId, seriesDbId ) , path );
    }

    if( removeTableRow ( d->T_SERIES, seriesDbId ) )
        emit removed(medDataIndex(1, patientDbId, studyDbId, seriesDbId, -1));

    // we want to remove the directory if empty
    QFileInfo seriesFi ( medStorage::dataLocation() + thumbnail );
    if ( seriesFi.dir().exists() )
    {
        bool res = seriesFi.dir().rmdir ( seriesFi.absolutePath() ); // only removes if empty

        // the serie's directory has been deleted, let's check if the patient directory is empty
        // this can happen after moving series
        if(res)
        {
            QDir parentDir = seriesFi.dir();
            res = parentDir.cdUp();

            if ( res && parentDir.exists() )
            {
                res = seriesFi.dir().rmdir ( parentDir.absolutePath() ); // only removes if empty
            }
        }
    }
}

bool medDatabaseRemover::isStudyEmpty ( int studyDbId )
{
    QSqlDatabase db(d->db);
    QSqlQuery query ( db );

    query.prepare ( "SELECT id FROM " + d->T_SERIES + " WHERE study = :study " );
    query.bindValue ( ":study", studyDbId );
    EXEC_QUERY ( query );
    return !query.next();
}

void medDatabaseRemover::removeStudy ( int patientDbId, int studyDbId )
{
    QSqlDatabase db(d->db);
    QSqlQuery query ( db );

    query.prepare ( "SELECT thumbnail, name, uid FROM " + d->T_STUDY + " WHERE id = :id " );
    query.bindValue ( ":id", studyDbId );
    EXEC_QUERY ( query );
    QString thumbnail;
    if ( query.next() )
    {
        thumbnail = query.value ( 0 ).toString();
        this->removeFile ( thumbnail );
    }
    if( removeTableRow ( d->T_STUDY, studyDbId ) )
        emit removed(medDataIndex(1, patientDbId, studyDbId, -1, -1));
}

bool medDatabaseRemover::isPatientEmpty ( int patientDbId )
{
    QSqlDatabase db(d->db);
    QSqlQuery query ( db );

    query.prepare ( "SELECT id FROM " + d->T_STUDY + " WHERE patient = :patient " );
    query.bindValue ( ":patient", patientDbId );
    EXEC_QUERY ( query );
    return !query.next();
}

void medDatabaseRemover::removePatient ( int patientDbId )
{
    QSqlDatabase db(d->db);
    QSqlQuery query ( db );

    QString patientName;
    QString patientBirthdate;
    QString patientId;

    query.prepare ( "SELECT thumbnail, patientId  FROM " + d->T_PATIENT + " WHERE id = :patient " );
    query.bindValue ( ":patient", patientDbId );
    EXEC_QUERY ( query );
    if ( query.next() )
    {
        QString thumbnail = query.value ( 0 ).toString();
        this->removeFile ( thumbnail );
        patientId = query.value ( 1 ).toString();
    }
    if( removeTableRow ( d->T_PATIENT, patientDbId ) )
        emit removed(medDataIndex(1, patientDbId, -1, -1, -1));

    medDatabaseController * dbi = medDatabaseController::instance();
    QDir patientDir ( medStorage::dataLocation() + "/" + dbi->stringForPath ( patientId ) );
    
    if ( patientDir.exists() )
        patientDir.rmdir ( patientDir.path() ); // only removes if empty
}

bool medDatabaseRemover::removeTableRow ( const QString &table, int id )
{
    QSqlDatabase db(d->db);
    QSqlQuery query ( db );
    query.prepare ( "DELETE FROM " + table + " WHERE id = :id" );
    query.bindValue ( ":id", id );
    EXEC_QUERY ( query );

    return (query.numRowsAffected()==1);
}

//! Remove a single file
void medDatabaseRemover::removeFile ( const QString & filename )
{
    QFile file ( medStorage::dataLocation() + filename );
    file.remove();
}

void medDatabaseRemover::onCancel ( QObject* )
{
    d->isCancelled = true;
}

//! Remove a data image file. Includes special cases for some file types.
void medDatabaseRemover::removeDataFile ( const medDataIndex &index, const QString & filename )
{
    QFileInfo fi ( filename );
    const QString suffix = fi.suffix();
    const QString mhd ( "mhd" );
    const QString mha ( "mha" );

    if ( suffix == mhd )
    {
        QString mhaFile ( filename );
        mhaFile.chop ( mhd.length() );
        mhaFile += mha;
        this->removeFile ( mhaFile );
    }
    else if ( suffix == mha )
    {
        QString mhdFile ( filename );
        mhdFile.chop ( mha.length() );
        mhdFile += mhd;
        this->removeFile ( mhdFile );
    }

    this->removeFile ( filename );
}
