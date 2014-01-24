/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseController.h"
#include "medDatabaseReader.h"
#include <medStorage.h>
#include <medAbstractDataImage.h>

#include <medMetaDataKeys.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <medAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>

class medDatabaseReaderPrivate
{
public:
    medDataIndex index;
};

medDatabaseReader::medDatabaseReader ( const medDataIndex& index ) : QObject(), d ( new medDatabaseReaderPrivate )
{
    d->index = index;
}

medDatabaseReader::~medDatabaseReader()
{
    delete d;

    d = NULL;
}

dtkSmartPointer<medAbstractData> medDatabaseReader::run()
{
    QVariant patientDbId = d->index.patientId();
    QVariant   studyDbId = d->index.studyId();
    QVariant  seriesDbId = d->index.seriesId();
    QVariant   imageDbId = d->index.imageId();

    QSqlQuery query ( ( * ( medDatabaseController::instance()->database() ) ) );

    QString patientName, birthdate, age, gender, patientId;
    QString studyName, studyUid, studyId;
    QString seriesName, seriesUid, orientation, seriesNumber, sequenceName,
            sliceThickness, rows, columns, refThumbPath, description, protocol,
            comments, status, acquisitiondate, importationdate, referee,
            institution, report, modality, seriesId;

    query.prepare ( "SELECT name, birthdate, gender, patientId FROM patient WHERE id = :id" );
    query.bindValue ( ":id", patientDbId );
    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    if ( query.first() )
    {
        patientName = query.value ( 0 ).toString();
        birthdate = query.value ( 1 ).toString();
        gender = query.value ( 2 ).toString();
        patientId = query.value ( 3 ).toString();
    }

    query.prepare ( "SELECT name, uid, studyId FROM study WHERE id = :id" );
    query.bindValue ( ":id", studyDbId );
    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    if ( query.first() )
    {
        studyName = query.value ( 0 ).toString();
        studyUid = query.value ( 1 ).toString();
        studyId = query.value ( 2 ).toString();
    }

    query.prepare ( "SELECT name, uid, orientation, seriesNumber, sequenceName, sliceThickness, rows, columns, \
                     description, protocol, comments, status, acquisitiondate, importationdate, referee,       \
                     institution, report, modality, seriesId \
                     FROM series WHERE id = :id" );
    
    query.bindValue ( ":id", seriesDbId );
    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    if ( query.first() )
    {
        seriesName = query.value ( 0 ).toString();
        seriesUid = query.value ( 1 ).toString();
        orientation = query.value ( 2 ).toString();
        seriesNumber = query.value ( 3 ).toString();
        sequenceName = query.value ( 4 ).toString();
        sliceThickness = query.value ( 5 ).toString();
        rows = query.value ( 6 ).toString();
        columns = query.value ( 7 ).toString();
        description = query.value ( 8 ).toString();
        protocol = query.value ( 9 ).toString();
        comments = query.value ( 10 ).toString();
        status = query.value ( 11 ).toString();
        acquisitiondate = query.value ( 12 ).toString();
        importationdate = query.value ( 13 ).toString();
        referee = query.value ( 14 ).toString();
        institution = query.value ( 15 ).toString();
        report = query.value ( 16 ).toString();
        modality = query.value ( 17 ).toString();
        seriesId = query.value ( 18 ).toString();
    }

    query.prepare ( "SELECT name, id, path, instance_path, isIndexed FROM image WHERE series = :series" );
    query.bindValue ( ":series", seriesDbId );
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

    dtkSmartPointer <medAbstractData> dtkdata =  this->readFile ( filenames );


    if ( ( !dtkdata.isNull() ) && dtkdata.data() )
    {

        QSqlQuery seriesQuery ( * ( medDatabaseController::instance()->database() ) );
        QVariant seriesThumbnail;

        seriesQuery.prepare ( "SELECT thumbnail FROM series WHERE id = :id" );
        seriesQuery.bindValue ( ":id", seriesDbId );
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

        medMetaDataKeys::PatientID.add ( dtkdata, patientId );
        medMetaDataKeys::PatientName.add ( dtkdata, patientName );
        medMetaDataKeys::BirthDate.add ( dtkdata, birthdate );
        medMetaDataKeys::Gender.add ( dtkdata, gender );
        medMetaDataKeys::StudyDescription.add ( dtkdata, studyName );
        medMetaDataKeys::StudyID.add ( dtkdata, studyId );
        medMetaDataKeys::StudyDicomID.add ( dtkdata, studyUid );
        medMetaDataKeys::SeriesDescription.add ( dtkdata, seriesName );
        medMetaDataKeys::SeriesID.add ( dtkdata, seriesId );
        medMetaDataKeys::SeriesDicomID.add ( dtkdata, seriesUid );
        medMetaDataKeys::Orientation.add ( dtkdata, orientation );
        medMetaDataKeys::Columns.add ( dtkdata, columns );
        medMetaDataKeys::Rows.add ( dtkdata, rows );
        medMetaDataKeys::AcquisitionDate.add ( dtkdata, acquisitiondate );
        medMetaDataKeys::Comments.add ( dtkdata, comments );
        medMetaDataKeys::Description.add ( dtkdata, description );
        medMetaDataKeys::ImportationDate.add ( dtkdata, importationdate );
        medMetaDataKeys::Modality.add ( dtkdata, modality );
        medMetaDataKeys::Protocol.add ( dtkdata, protocol );
        medMetaDataKeys::Referee.add ( dtkdata, referee );
        medMetaDataKeys::Institution.add ( dtkdata, institution );
        medMetaDataKeys::Report.add ( dtkdata, report );
        medMetaDataKeys::Status.add ( dtkdata, status );
        medMetaDataKeys::SequenceName.add ( dtkdata, sequenceName );
        medMetaDataKeys::SliceThickness.add ( dtkdata, sliceThickness );
        medMetaDataKeys::SeriesNumber.add(dtkdata, seriesNumber);

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
    QVariant patientDbId = d->index.patientId();
    QVariant   studyDbId = d->index.studyId();
    QVariant  seriesDbId = d->index.seriesId();
    QVariant   imageDbId = d->index.imageId();

    QSqlQuery query ( ( * ( medDatabaseController::instance()->database() ) ) );

    QString filename;

    query.prepare ( "SELECT path, instance_path, isIndexed FROM image WHERE series = :series" );
    query.bindValue ( ":series", seriesDbId );

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

dtkSmartPointer<medAbstractData> medDatabaseReader::readFile ( QString filename )
{
    QStringList filenames;
    filenames << filename;
    return this->readFile ( filenames );
}


dtkSmartPointer<medAbstractData> medDatabaseReader::readFile ( const QStringList filenames )
{
    dtkSmartPointer<medAbstractData> dtkdata;

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
            dtkdata = dynamic_cast<medAbstractData*>(dataReader->data());
            if ( dtkdata.refCount() != 2 )
                qWarning() << "(ReaderLoop) RefCount should be 2 here: " << dtkdata.refCount();
            break;
        }

    }
    return dtkdata;
}
