/* medDatabaseReader.cpp ---
 *
 * Author: Julien Wintz
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

/* Change log:
 *
 */

#include "medDatabaseController.h"
#include "medDatabaseReader.h"
#include <medStorage.h>
#include <medAbstractDataImage.h>

#include <medCore/medMetaDataKeys.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>

class medDatabaseReaderPrivate
{
public:
    medDataIndex index;
};

medDatabaseReader::medDatabaseReader ( const medDataIndex& index ) : QObject(), d ( new medDatabaseReaderPrivate )
{
    d->index = index;
}

medDatabaseReader::~medDatabaseReader ( void )
{
    delete d;

    d = NULL;
}

dtkSmartPointer<dtkAbstractData> medDatabaseReader::run ( void )
{
    QVariant patientId = d->index.patientId();
    QVariant   studyId = d->index.studyId();
    QVariant  seriesId = d->index.seriesId();
    QVariant   imageId = d->index.imageId();

    QSqlQuery query ( ( * ( medDatabaseController::instance()->database() ) ) );

    QString patientName;
    QString birthdate;
    QString studyName;
    QString studyUid;
    QString seriesName;
    QString seriesUid;

    query.prepare ( "SELECT name, birthdate FROM patient WHERE id = :id" );
    query.bindValue ( ":id", patientId );
    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    if ( query.first() )
    {
        patientName = query.value ( 0 ).toString();
        birthdate = query.value ( 1 ).toString();
    }

    query.prepare ( "SELECT name, uid FROM study WHERE id = :id" );
    query.bindValue ( ":id", studyId );
    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    if ( query.first() )
    {
        studyName = query.value ( 0 ).toString();
        studyUid = query.value ( 1 ).toString();
    }

    query.prepare ( "SELECT name, uid FROM series WHERE id = :id" );
    query.bindValue ( ":id", seriesId );
    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    if ( query.first() )
    {
        seriesName = query.value ( 0 ).toString();
        seriesUid = query.value ( 1 ).toString();
    }

    query.prepare ( "SELECT name, id, path, instance_path, isIndexed FROM image WHERE series = :series" );
    query.bindValue ( ":series", seriesId );
    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    // now we might have both indexed and imported images in the same series
    // so we will get the full path for indexed images and, for the imported
    // (and possibly aggregated) images we will get the path of the
    // aggregated file (which is in instance_path)

    QStringList filenames;
    while ( query.next() )
    {
        bool isIndexed = query.value ( 4 ).toBool();

        if ( isIndexed )
        {
            QString filename = query.value ( 2 ).toString();

            // if the file is indexed the chanced that is not there anymore are higher
            // so we check for existence and return null if they are not there anymore
            QFileInfo fileinfo ( filename );

            if ( !fileinfo.exists() )
            {
                emit failure ( this );
                return NULL;
            }

            filenames << filename;
        }
        else
        {
            filenames << medStorage::dataLocation() + query.value ( 3 ).toString();
        }
    }
    // we remove all the duplicate entries, as imported files
    // might have introduced duplicates
    filenames.removeDuplicates();

    dtkSmartPointer <dtkAbstractData> dtkdata =  this->readFile ( filenames );


    if ( ( !dtkdata.isNull() ) && dtkdata.data() )
    {

        QSqlQuery seriesQuery ( * ( medDatabaseController::instance()->database() ) );
        QVariant seriesThumbnail;

        seriesQuery.prepare ( "SELECT thumbnail FROM series WHERE id = :seriesId" );
        seriesQuery.bindValue ( ":seriesId", seriesId );
        if ( !seriesQuery.exec() )
            qDebug() << DTK_COLOR_FG_RED << seriesQuery.lastError() << DTK_NO_COLOR;

        if ( seriesQuery.first() )
        {
            seriesThumbnail = seriesQuery.value ( 0 );

            QString thumbPath = medStorage::dataLocation() + seriesThumbnail.toString();
            medMetaDataKeys::SeriesThumbnail.add ( dtkdata, thumbPath );

        }
        else
        {
            qWarning() << "Thumbnailpath not found";
        }

        medMetaDataKeys::PatientName.add ( dtkdata, patientName );
        medMetaDataKeys::BirthDate.add ( dtkdata, birthdate );
        medMetaDataKeys::StudyDescription.add ( dtkdata, studyName );
        medMetaDataKeys::SeriesDescription.add ( dtkdata, seriesName );
        medMetaDataKeys::PatientID.add ( dtkdata, patientId.toString() );
        medMetaDataKeys::StudyID.add ( dtkdata, studyUid );
        medMetaDataKeys::SeriesID.add ( dtkdata, seriesUid );
        //medMetaDataKeys::ImageID.add(data, imageId.toString());

        emit success ( this );
    }
    else
    {
        emit failure ( this );
    }
    if ( dtkdata.refCount() != 1 )
        qWarning() << "(Run:Exit) RefCount should be 1 here: " << dtkdata.refCount();
    return dtkdata;

}

qint64 medDatabaseReader::getDataSize()
{
    QString filename = getFilePath();
    QFileInfo info ( filename );
    return info.size();
}

QString medDatabaseReader::getFilePath()
{
    QVariant patientId = d->index.patientId();
    QVariant   studyId = d->index.studyId();
    QVariant  seriesId = d->index.seriesId();
    QVariant   imageId = d->index.imageId();

    QSqlQuery query ( ( * ( medDatabaseController::instance()->database() ) ) );

    QString filename;

    query.prepare ( "SELECT path, instance_path, isIndexed FROM image WHERE series = :series" );
    query.bindValue ( ":series", seriesId );

    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    // indexed files have an empty string in 'instance_path' column
    // and imported files have the relative path of the (aggregated) file

    while ( query.next() )
    {
        bool isIndexed = query.value ( 2 ).toBool();

        if ( isIndexed )
        {
            filename = query.value ( 0 ).toString();
        }
        else
        {
            filename = medStorage::dataLocation() + query.value ( 2 ).toString();
        }
    }

    return filename;
}

dtkSmartPointer<dtkAbstractData> medDatabaseReader::readFile ( QString filename )
{
    QStringList filenames;
    filenames << filename;
    return this->readFile ( filenames );
}


dtkSmartPointer<dtkAbstractData> medDatabaseReader::readFile ( const QStringList filenames )
{
    dtkSmartPointer<dtkAbstractData> dtkdata;

    QList<QString> readers = dtkAbstractDataFactory::instance()->readers();

    for ( int i = 0; i < readers.size(); i++ )
    {

        dtkSmartPointer<dtkAbstractDataReader> dataReader;
        dataReader = dtkAbstractDataFactory::instance()->readerSmartPointer ( readers[i] );

        connect ( dataReader, SIGNAL ( progressed ( int ) ), this, SIGNAL ( progressed ( int ) ) );
        if ( dataReader->canRead ( filenames ) )
        {
            dataReader->read ( filenames );
            dataReader->enableDeferredDeletion ( false );
            dtkdata = dataReader->data();
            if ( dtkdata.refCount() != 2 )
                qWarning() << "(ReaderLoop) RefCount should be 2 here: " << dtkdata.refCount();
            break;
        }

    }
    return dtkdata;
}
