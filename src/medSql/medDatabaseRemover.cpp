/* medDatabaseRemover.cpp ---
 *
 * Author: John Stark
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jun 29 15:27:20 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 29 15:46:20 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 19
 */

/* Commentary:
 *
 */

#include "medDatabaseRemover.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>

#include <medDatabaseController.h>
#include <medStorage.h>
#include <medDataIndex.h>
#include <medAbstractDataImage.h>

#define EXEC_QUERY(q) execQuery(q, __FILE__ , __LINE__ )
namespace {
    inline bool execQuery( QSqlQuery & query, const char *file, int line )
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
    QSqlDatabase * db;
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

medDatabaseRemover::medDatabaseRemover(const medDataIndex &index_) : medJobItem(), d(new medDatabaseRemoverPrivate)
{
    d->index = index_;
    d->db = medDatabaseController::instance()->database();
    d->isCancelled = false;
}

medDatabaseRemover::~medDatabaseRemover(void)
{
    delete d;

    d = NULL;
}

void medDatabaseRemover::run(void)
{

    QSqlDatabase & db( *d->db );
    QSqlQuery ptQuery(db);

    const medDataIndex index = d->index;
    if ( index.isValidForPatient() ) {
        ptQuery.prepare("SELECT id FROM " + d->T_PATIENT + " WHERE id = :id");
        ptQuery.bindValue(":id", index.patientId());
    } else {
        ptQuery.prepare("SELECT id FROM " + d->T_PATIENT);
    }

    EXEC_QUERY(ptQuery);
    while( ptQuery.next() ) {
        if (d->isCancelled)
            break;

        int patientId = ptQuery.value(0).toInt();
        QSqlQuery stQuery(db);

        if ( index.isValidForStudy() ) {
            stQuery.prepare("SELECT id FROM " + d->T_STUDY + " WHERE id = :id AND patient = :patientId");
            stQuery.bindValue(":id", index.studyId());
        } else {
            stQuery.prepare("SELECT id FROM " + d->T_STUDY + " WHERE patient = :patientId");
        }
        stQuery.bindValue(":patientId", patientId);

        EXEC_QUERY(stQuery);
        while( stQuery.next() ) {
            if (d->isCancelled)
                break;

            int studyId = stQuery.value(0).toInt();
            QSqlQuery seQuery(db);

            if ( index.isValidForSeries() ) {
                seQuery.prepare("SELECT id FROM " + d->T_SERIES + " WHERE id = :id AND study = :studyId");
                seQuery.bindValue(":id", index.seriesId());
            } else {
                seQuery.prepare("SELECT id FROM " + d->T_SERIES + " WHERE study = :studyId");
            }
            seQuery.bindValue(":studyId", studyId);

            EXEC_QUERY(seQuery);
            while( seQuery.next() ) {
                if (d->isCancelled)
                    break;

                int seriesId = seQuery.value(0).toInt();
                QSqlQuery imQuery(db);

                if ( index.isValidForImage() ) {
                    imQuery.prepare("SELECT id FROM " + d->T_IMAGE + " WHERE id = :id AND series = :seriesId");
                    imQuery.bindValue(":id", index.imageId());
                } else {
                    imQuery.prepare("SELECT id FROM " + d->T_IMAGE + " WHERE series = :seriesId");
                }
                imQuery.bindValue(":seriesId", seriesId);

                EXEC_QUERY(imQuery);
                while( imQuery.next() ) {
                    int imageId = imQuery.value(0).toInt();
                    this->removeImage(patientId, studyId, seriesId, imageId);
                }
                if (this->isSeriesEmpty( seriesId ) )
                    this->removeSeries( patientId, studyId, seriesId );

            } // seQuery.next
            if (this->isStudyEmpty( studyId ) )
                this->removeStudy( patientId, studyId );

        } // stQuery.next
        if (this->isPatientEmpty( patientId ) )
            this->removePatient( patientId );

    } // ptQuery.next

    emit removed(index);
    emit progressed(this, 100);
    emit progress(100);
    if ( d->isCancelled )
        emit failure(this);
    else
        emit success(this);

    return;
}

void medDatabaseRemover::removeImage(int patientId, int studyId, int seriesId, int imageId )
{
    QSqlDatabase & db( *d->db );
    QSqlQuery query(db);

    query.prepare("SELECT thumbnail FROM " + d->T_IMAGE + " WHERE id = :imageId ");
    query.bindValue(":id", imageId);
    EXEC_QUERY(query);
    if ( query.next()) {
        QString thumbnail = query.value(0).toString();
        this->removeFile( thumbnail );
    }
    removeTableRow( d->T_IMAGE, imageId );
}

bool medDatabaseRemover::isSeriesEmpty( int seriesId )
{
    QSqlDatabase & db( *d->db );
    QSqlQuery query(db);

    query.prepare("SELECT id FROM " + d->T_IMAGE + " WHERE series = :seriesId ");
    query.bindValue(":seriesId", seriesId);
    EXEC_QUERY(query);
    return !query.next();
}

void medDatabaseRemover::removeSeries( int patientId, int studyId, int seriesId )
{
    QSqlDatabase & db( *d->db );
    QSqlQuery query(db);

    query.prepare("SELECT thumbnail, path, name  FROM " + d->T_SERIES + " WHERE id = :seriesId ");
    query.bindValue(":seriesId", seriesId);
    EXEC_QUERY(query);
    QString seriesName;
    QString thumbnail;
    if ( query.next()) {
        thumbnail = query.value(0).toString();
        this->removeFile( thumbnail );
        QString path = query.value(1).toString();

        // if path is empty then it was an indexed series
        if(!path.isNull() && !path.isEmpty())
            this->removeDataFile( medDataIndex::makeSeriesIndex(d->index.dataSourceId(), patientId, studyId, seriesId) , path );

        seriesName = query.value(2).toString();
    }
    removeTableRow( d->T_SERIES, seriesId );

    // we want to remove the directory if empty
    QFileInfo seriesFi(medStorage::dataLocation() + thumbnail);
    if (seriesFi.dir().exists())
        seriesFi.dir().rmdir(seriesFi.absolutePath()); // only removes if empty
}

bool medDatabaseRemover::isStudyEmpty( int studyId )
{
    QSqlDatabase & db( *d->db );
    QSqlQuery query(db);

    query.prepare("SELECT id FROM " + d->T_SERIES + " WHERE study = :studyId ");
    query.bindValue(":studyId", studyId);
    EXEC_QUERY(query);
    return !query.next();
}

void medDatabaseRemover::removeStudy( int patientId, int studyId )
{
    QSqlDatabase & db( *d->db );
    QSqlQuery query(db);

    QString studyName;

    query.prepare("SELECT thumbnail, name FROM " + d->T_STUDY + " WHERE id = :studyId ");
    query.bindValue(":studyId", studyId);
    EXEC_QUERY(query);
    if ( query.next()) {
        QString thumbnail = query.value(0).toString();
        this->removeFile( thumbnail );
        studyName = query.value(1).toString();
    }
    removeTableRow( d->T_STUDY, studyId );

    query.prepare("SELECT name  FROM " + d->T_PATIENT + " WHERE id = :patientId ");
    query.bindValue(":patientId", patientId);
    EXEC_QUERY(query);
    QString patientName;
    if ( query.next() ) 
        patientName = query.value(0).toString();

    medDatabaseControllerImpl* dbi = medDatabaseController::instance();
    QDir studyDir(medStorage::dataLocation() + "/" + dbi->stringForPath(patientName) + "/" + dbi->stringForPath(studyName));
    if (studyDir.exists())
        studyDir.rmdir(studyDir.path()); // only removes if empty
}

bool medDatabaseRemover::isPatientEmpty( int patientId )
{
    QSqlDatabase & db( *d->db );
    QSqlQuery query(db);

    query.prepare("SELECT id FROM " + d->T_STUDY + " WHERE patient = :patientId ");
    query.bindValue(":patientId", patientId);
    EXEC_QUERY(query);
    return !query.next();
}

void medDatabaseRemover::removePatient( int patientId )
{
    QSqlDatabase & db( *d->db );
    QSqlQuery query(db);

    QString patientName;
    query.prepare("SELECT thumbnail,name  FROM " + d->T_PATIENT + " WHERE id = :patientId ");
    query.bindValue(":patientId", patientId);
    EXEC_QUERY(query);
    if ( query.next()) {
        QString thumbnail = query.value(0).toString();
        this->removeFile( thumbnail );
        patientName = query.value(1).toString();
    }
    removeTableRow( d->T_PATIENT, patientId );

    medDatabaseControllerImpl * dbi = medDatabaseController::instance();
    QDir patientDir(medStorage::dataLocation() + "/" + dbi->stringForPath(patientName));
    if (patientDir.exists())
        patientDir.rmdir(patientDir.path()); // only removes if empty
}

void medDatabaseRemover::removeTableRow( const QString &table, int id )
{
    QSqlDatabase & db( *d->db );
    QSqlQuery query(db);
    query.prepare("DELETE FROM " + table + " WHERE id = :id");
    query.bindValue(":id", id);
    EXEC_QUERY(query);
}

void medDatabaseRemover::removeFile( const QString & filename )
{
    QFile file( medStorage::dataLocation() + filename );
    file.remove();
}

void medDatabaseRemover::onCancel(QObject*)
{
    d->isCancelled = true;
}

void medDatabaseRemover::removeDataFile( const medDataIndex &index, const QString & filename )
{
    QFileInfo fi(filename);
    const QString suffix = fi.suffix();
    const QString mhd("mhd");
    const QString mha("mha");

    if ( suffix == mhd ) {
        QString mhaFile (filename);
        mhaFile.chop(mhd.length());
        mhaFile += mha;
        this->removeFile(mhaFile);
    } else if ( suffix == mha ) {
        QString mhdFile (filename);
        mhdFile.chop(mha.length());
        mhdFile += mhd;
        this->removeFile(mhdFile);
    }

    this->removeFile(filename);
}
