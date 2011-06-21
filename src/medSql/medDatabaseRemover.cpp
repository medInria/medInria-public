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


#include "medDatabaseController.h"
#include "medDatabaseRemover.h"
#include <medCore/medStorage.h>
#include <medCore/medDataIndex.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>

#define EXEC_QUERY(q) execQuery(q, __FILE__ , __LINE__ )

inline bool execQuery( QSqlQuery & query, const char *file, int line ) {
    if ( ! query.exec() ) {
         qDebug() << file << "(" << line << ") :" << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
         return false;
    }
    return true;
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
};

const QString medDatabaseRemoverPrivate::T_PATIENT = "patient";
const QString medDatabaseRemoverPrivate::T_STUDY = "study";
const QString medDatabaseRemoverPrivate::T_SERIES = "series";
const QString medDatabaseRemoverPrivate::T_IMAGE = "image";

medDatabaseRemover::medDatabaseRemover(const medDataIndex &index_) : QObject(), d(new medDatabaseRemoverPrivate)
{
    d->index = index_;
    d->db = medDatabaseController::instance()->database();
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
                    this->removeImage(imageId);
                }
                if (this->isSeriesEmpty( seriesId ) )
                    this->removeSeries( seriesId );
            } // seQuery.next
            if (this->isStudyEmpty( studyId ) )
                this->removeStudy( studyId );
        } // stQuery.next
        if (this->isPatientEmpty( patientId ) )
            this->removePatient( patientId );
    } // ptQuery.next

    emit removed(index);
    emit progressed(100);
    emit success(this);

    return;
}

void medDatabaseRemover::removeImage( int imageId )
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

void medDatabaseRemover::removeSeries( int seriesId )
{
    QSqlDatabase & db( *d->db );
    QSqlQuery query(db);

    query.prepare("SELECT thumbnail, path  FROM " + d->T_SERIES + " WHERE id = :seriesId ");
    query.bindValue(":seriesId", seriesId);
    EXEC_QUERY(query);
    if ( query.next()) {
        QString thumbnail = query.value(0).toString();
        this->removeFile( thumbnail );
        QString path = query.value(1).toString();
        this->removeFile( path );
    }
    removeTableRow( d->T_SERIES, seriesId );
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

void medDatabaseRemover::removeStudy( int studyId )
{
    QSqlDatabase & db( *d->db );
    QSqlQuery query(db);

    query.prepare("SELECT thumbnail FROM " + d->T_STUDY + " WHERE id = :studyId ");
    query.bindValue(":studyId", studyId);
    EXEC_QUERY(query);
    if ( query.next()) {
        QString thumbnail = query.value(0).toString();
        this->removeFile( thumbnail );
    }
    removeTableRow( d->T_STUDY, studyId );
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

    query.prepare("SELECT thumbnail  FROM " + d->T_PATIENT + " WHERE id = :patientId ");
    query.bindValue(":patientId", patientId);
    EXEC_QUERY(query);
    if ( query.next()) {
        QString thumbnail = query.value(0).toString();
        this->removeFile( thumbnail );
    }
    removeTableRow( d->T_PATIENT, patientId );
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

