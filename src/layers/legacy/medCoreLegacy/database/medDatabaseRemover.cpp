/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QSqlError>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medAbstractImageData.h>
#include <medDatabaseRemover.h>
#include <medDataIndex.h>
#include <medDataManager.h>
#include <medStorage.h>

class medDatabaseRemoverPrivate
{
public:
    medDataIndex index;
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
    d->isCancelled = false;
}

medDatabaseRemover::~medDatabaseRemover()
{
    delete d;
    d = nullptr;
}

void medDatabaseRemover::internalRun()
{
    QSqlDatabase dbConnection = medDataManager::instance().controller()->getThreadSpecificConnection();
    QSqlQuery ptQuery(dbConnection);

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

    QMutexLocker mutexLocker(&medDataManager::instance().controller()->getDatabaseMutex());
    medDataManager::instance().controller()->execQuery(ptQuery);
    while ( ptQuery.next() )
    {
        if ( d->isCancelled )
            break;

        int patientDbId = ptQuery.value ( 0 ).toInt();
        QSqlQuery stQuery(dbConnection);

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

        medDataManager::instance().controller()->execQuery(stQuery);
        while ( stQuery.next() )
        {
            if ( d->isCancelled )
                break;

            int studyDbId = stQuery.value ( 0 ).toInt();
            QSqlQuery seQuery(dbConnection);

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

            medDataManager::instance().controller()->execQuery(seQuery);

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

    ptQuery.finish();
    mutexLocker.unlock();

    emit progress (this, 100 );

    if ( d->isCancelled )
        emit failure ( this );
    else
        emit success ( this );

    return;
}

void medDatabaseRemover::removeSeries ( int patientDbId, int studyDbId, int seriesDbId )
{
    QSqlDatabase dbConnection = medDataManager::instance().controller()->getThreadSpecificConnection();
    QMutexLocker mutexLocker(&medDataManager::instance().controller()->getDatabaseMutex());
    QSqlQuery query(dbConnection);
    query.exec("SELECT COUNT(*) as cpt FROM pragma_table_info('series') WHERE name='json_meta_path'");
    bool jsonColExist = false;
    if (query.next())
    {
        jsonColExist = query.value("cpt").toInt() != 0;
    }

    if (jsonColExist)
    {
        query.prepare(
                "SELECT thumbnail as thumbnail, path as path, name as name, json_meta_path as json_meta_path  FROM " +
                d->T_SERIES + " WHERE id = :series ");
    }
    else
    {
        query.prepare(
                "SELECT thumbnail as thumbnail, path as path, name as name FROM " +
                d->T_SERIES + " WHERE id = :series ");
    }
    query.bindValue ( ":series", seriesDbId );
    medDataManager::instance().controller()->execQuery(query);

    if ( query.next() )
    {
        QString path = query.value ("path").toString();

        // if path is empty then it was an indexed series
        if ( !path.isNull() && !path.isEmpty() )
        {
            this->removeDataFile(path);
        }
        if (jsonColExist)
        {
            QString json_meta_file = query.value("json_meta_path").toString();
            if (!json_meta_file.isEmpty())
                this->removeFile(json_meta_file);
        }
        removeThumbnailIfNeeded(query);
    }

    query.finish();
    mutexLocker.unlock();

    if( removeTableRow ( d->T_SERIES, seriesDbId ) )
        emit removed(medDataIndex(1, patientDbId, studyDbId, seriesDbId));
}

bool medDatabaseRemover::isStudyEmpty ( int studyDbId )
{
    QSqlDatabase dbConnection = medDataManager::instance().controller()->getThreadSpecificConnection();
    QSqlQuery query(dbConnection);

    query.prepare ( "SELECT id FROM " + d->T_SERIES + " WHERE study = :study " );
    query.bindValue ( ":study", studyDbId );
    QMutexLocker mutexLocker(&medDataManager::instance().controller()->getDatabaseMutex());
    medDataManager::instance().controller()->execQuery(query);
    return !query.next();
}

void medDatabaseRemover::removeStudy ( int patientDbId, int studyDbId )
{
    QSqlDatabase dbConnection = medDataManager::instance().controller()->getThreadSpecificConnection();
    QSqlQuery query(dbConnection);

    query.prepare ( "SELECT thumbnail, name, uid FROM " + d->T_STUDY + " WHERE id = :id " );
    query.bindValue ( ":id", studyDbId );

    QMutexLocker mutexLocker(&medDataManager::instance().controller()->getDatabaseMutex());
    medDataManager::instance().controller()->execQuery(query);

    if ( query.next() )
    {
        medAbstractDbController * dbc = medDataManager::instance().controllerForDataSource(d->index.dataSourceId());
        if (dbc->metaData(d->index,  medMetaDataKeys::StudyID.key()).toInt() == studyDbId)
        {
            removeThumbnailIfNeeded(query);
        }
    }

    query.finish();
    mutexLocker.unlock();

    if( removeTableRow ( d->T_STUDY, studyDbId ) )
        emit removed(medDataIndex(1, patientDbId, studyDbId, -1));
}

bool medDatabaseRemover::isPatientEmpty ( int patientDbId )
{
    QSqlDatabase dbConnection = medDataManager::instance().controller()->getThreadSpecificConnection();
    QSqlQuery query(dbConnection);

    query.prepare ( "SELECT id FROM " + d->T_STUDY + " WHERE patient = :patient " );
    query.bindValue ( ":patient", patientDbId );
    QMutexLocker mutexLocker(&medDataManager::instance().controller()->getDatabaseMutex());
    medDataManager::instance().controller()->execQuery(query);
    return !query.next();
}

void medDatabaseRemover::removePatient ( int patientDbId )
{
    QSqlDatabase dbConnection = medDataManager::instance().controller()->getThreadSpecificConnection();
    QSqlQuery query(dbConnection);

    QString patientId;

    query.prepare ( "SELECT thumbnail, patientId  FROM " + d->T_PATIENT + " WHERE id = :patient " );
    query.bindValue ( ":patient", patientDbId );
    QMutexLocker mutexLocker(&medDataManager::instance().controller()->getDatabaseMutex());
    medDataManager::instance().controller()->execQuery(query);
    if ( query.next() )
    {
        removeThumbnailIfNeeded(query);
        patientId = query.value ( 1 ).toString();
    }

    query.finish();
    mutexLocker.unlock();

    if( removeTableRow ( d->T_PATIENT, patientDbId ) )
        emit removed(medDataIndex(1, patientDbId, -1, -1));
}

bool medDatabaseRemover::removeTableRow ( const QString &table, int id )
{
    QSqlDatabase dbConnection = medDataManager::instance().controller()->getThreadSpecificConnection();
    QSqlQuery query(dbConnection);
    query.prepare ( "DELETE FROM " + table + " WHERE id = :id" );
    query.bindValue ( ":id", id );
    QMutexLocker mutexLocker(&medDataManager::instance().controller()->getDatabaseMutex());
    medDataManager::instance().controller()->execQuery(query);

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
    QString thumbnail = query.value("thumbnail").toString();

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
