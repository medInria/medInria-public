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

#include <QSqlError>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medAbstractImageData.h>
#include <medDatabaseController.h>
#include <medDatabaseRemover.h>
#include <medDataIndex.h>
#include <medDataManager.h>
#include <medStorage.h>

class medDatabaseRemoverPrivate
{
public:
    medDataIndex index;
    QSqlDatabase db;
    static const QString T_PATIENT;
    static const QString T_STUDY;
    static const QString T_SERIES;

    bool isCancelled;
};

const QString medDatabaseRemoverPrivate::T_PATIENT = "patient";
const QString medDatabaseRemoverPrivate::T_STUDY = "study";
const QString medDatabaseRemoverPrivate::T_SERIES = "series";

medDatabaseRemover::medDatabaseRemover ( const medDataIndex &index_ ) : medJobItemL(), d ( new medDatabaseRemoverPrivate )
{
    d->index = index_;
    d->db = medDatabaseController::instance()->database();
    d->isCancelled = false;
}

medDatabaseRemover::~medDatabaseRemover()
{
    delete d;
    d = nullptr;
}

void medDatabaseRemover::internalRun()
{
    QSqlDatabase db( d->db );
    QSqlQuery ptQuery ( db );

    // Is Patient
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

        // Is Study
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

            // Is Series
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

                // Remove Series
                this->removeSeries ( patientDbId, studyDbId, seriesDbId );

                emit progress (this, 50 );
            } // seQuery.next

            // After removal of series, test if study is empty, and remove it if it is
            if ( this->isStudyEmpty ( studyDbId ) )
            {
                this->removeStudy ( patientDbId, studyDbId );
            }

        } // stQuery.next

        // After removal of study, test if patient is empty, and remove it if it is
        if ( this->isPatientEmpty ( patientDbId ) )
        {
            this->removePatient ( patientDbId );
        }

    } // ptQuery.next
    emit progress (this, 100 );

    if ( d->isCancelled )
        emit failure ( this );
    else
        emit success ( this );

    return;
}

void medDatabaseRemover::removeSeries ( int patientDbId, int studyDbId, int seriesDbId )
{
    QSqlDatabase db(d->db);
    QSqlQuery query ( db );

    query.prepare ( "SELECT thumbnail, path, name  FROM " + d->T_SERIES + " WHERE id = :series " );
    query.bindValue ( ":series", seriesDbId );
    EXEC_QUERY ( query );

    if ( query.next() )
    {
        QString path = query.value (1).toString();

        // if path is empty then it was an indexed series
        if ( !path.isNull() && !path.isEmpty() )
        {
            this->removeDataFile(path);
        }
        removeThumbnailIfNeeded(query);
    }

    if( removeTableRow ( d->T_SERIES, seriesDbId ) )
        emit removed(medDataIndex(1, patientDbId, studyDbId, seriesDbId));
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

    if ( query.next() )
    {
        medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(d->index.dataSourceId());
        if (dbc->metaData(d->index,  medMetaDataKeys::StudyID.key()).toInt() == studyDbId)
        {
            removeThumbnailIfNeeded(query);
        }
    }

    if( removeTableRow ( d->T_STUDY, studyDbId ) )
        emit removed(medDataIndex(1, patientDbId, studyDbId, -1));
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

    QString patientId;

    query.prepare ( "SELECT thumbnail, patientId  FROM " + d->T_PATIENT + " WHERE id = :patient " );
    query.bindValue ( ":patient", patientDbId );
    EXEC_QUERY ( query );
    if ( query.next() )
    {
        removeThumbnailIfNeeded(query);
        patientId = query.value ( 1 ).toString();
    }
    if( removeTableRow ( d->T_PATIENT, patientDbId ) )
        emit removed(medDataIndex(1, patientDbId, -1, -1));
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

void medDatabaseRemover::removeThumbnailIfNeeded(QSqlQuery query)
{
    QString thumbnail = query.value(0).toString();

    this->removeFile ( thumbnail );

    // we want to remove the directory if empty
    QFileInfo seriesFi ( medStorage::dataLocation() + thumbnail );
    if ( seriesFi.dir().exists() )
    {
        bool res = seriesFi.dir().rmdir ( seriesFi.absolutePath() ); // only removes if empty

        // the series's directory has been deleted, let's check if the patient directory is empty
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

void medDatabaseRemover::onCancel ( QObject* )
{
    d->isCancelled = true;
}

//! Remove a data image file. Includes special cases for some file types.
void medDatabaseRemover::removeDataFile(const QString & filename)
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
