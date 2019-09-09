/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtSql/QSqlError>

#include <medDatabaseController.h>
#include <medDatabaseReader.h>
#include <medStorage.h>
#include <medAbstractImageData.h>

#include <medMetaDataKeys.h>
#include <medAbstractDataFactory.h>
#include <dtkCoreSupport/dtkAbstractDataReader.h>
#include <dtkCoreSupport/dtkAbstractDataWriter.h>
#include <medAbstractData.h>
#include <dtkCoreSupport/dtkGlobal.h>
#include <dtkLog>

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

medAbstractData* medDatabaseReader::run()
{
    QVariant patientDbId = d->index.patientId();
    QVariant   studyDbId = d->index.studyId();
    QVariant  seriesDbId = d->index.seriesId();

    QSqlQuery query(medDatabaseController::instance()->database());

    QString patientName, birthdate, age, gender, patientId;
    QString studyName, studyUid, studyId;
    QString seriesName, seriesUid, orientation, seriesNumber, sequenceName,
            sliceThickness, rows, columns, refThumbPath, description, protocol,
            comments, status, acquisitiondate, importationdate, referee,
            institution, report, modality, seriesId;
    QString origin, flipAngle, echoTime, repetitionTime, acquisitionTime;

    query.prepare ( "SELECT name, birthdate, gender, patientId FROM patient WHERE id = :id" );
    query.bindValue ( ":id", patientDbId );
    if ( !query.exec() )
    {
        dtkDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

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
    {
        dtkDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if ( query.first() )
    {
        studyName = query.value ( 0 ).toString();
        studyUid = query.value ( 1 ).toString();
        studyId = query.value ( 2 ).toString();
    }

    query.prepare ( "SELECT name, uid, orientation, seriesNumber, sequenceName, sliceThickness, rows, columns, \
                     description, protocol, comments, status, acquisitiondate, importationdate, referee,       \
                     institution, report, modality, seriesId, \
                     origin, flipAngle, echoTime, repetitionTime, acquisitionTime \
                     FROM series WHERE id = :id" );

    query.bindValue ( ":id", seriesDbId );
    if ( !query.exec() )
    {
        dtkDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

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

        // MR
        origin          = query.value ( 19 ).toString();
        flipAngle       = query.value ( 20 ).toString();
        echoTime        = query.value ( 21 ).toString();
        repetitionTime  = query.value ( 22 ).toString();
        acquisitionTime = query.value ( 23 ).toString();
    }

    query.prepare ( "SELECT name, id, path, instance_path, isIndexed FROM image WHERE series = :series" );
    query.bindValue ( ":series", seriesDbId );
    if ( !query.exec() )
    {
        dtkDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

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

    medAbstractData *medData =  this->readFile(filenames);


    if (medData)
    {

        QSqlQuery seriesQuery (medDatabaseController::instance()->database());
        QVariant seriesThumbnail;

        seriesQuery.prepare ( "SELECT thumbnail FROM series WHERE id = :id" );
        seriesQuery.bindValue ( ":id", seriesDbId );
        if (!seriesQuery.exec())
        {
            dtkDebug() << DTK_COLOR_FG_RED << seriesQuery.lastError() << DTK_NO_COLOR;
        }

        if(seriesQuery.first())
        {
            seriesThumbnail = seriesQuery.value ( 0 );

            QString thumbPath = medStorage::dataLocation() + seriesThumbnail.toString();
            medMetaDataKeys::SeriesThumbnail.set (medData, thumbPath);
        }
        else
        {
            dtkWarn() << "Thumbnailpath not found";
        }

        medMetaDataKeys::PatientID.set ( medData, patientId );
        medMetaDataKeys::PatientName.set ( medData, patientName );
        medMetaDataKeys::BirthDate.set ( medData, birthdate );
        medMetaDataKeys::Gender.set ( medData, gender );
        medMetaDataKeys::StudyDescription.set ( medData, studyName );
        medMetaDataKeys::StudyID.set ( medData, studyId );
        medMetaDataKeys::StudyDicomID.set ( medData, studyUid );
        medMetaDataKeys::SeriesDescription.set ( medData, seriesName );
        medMetaDataKeys::SeriesID.set ( medData, seriesId );
        medMetaDataKeys::SeriesDicomID.set ( medData, seriesUid );
        medMetaDataKeys::Orientation.set ( medData, orientation );
        medMetaDataKeys::Columns.set ( medData, columns );
        medMetaDataKeys::Rows.set ( medData, rows );
        medMetaDataKeys::AcquisitionDate.set ( medData, acquisitiondate );
        medMetaDataKeys::Comments.set ( medData, comments );
        medMetaDataKeys::Description.set ( medData, description );
        medMetaDataKeys::ImportationDate.set ( medData, importationdate );
        medMetaDataKeys::Modality.set ( medData, modality );
        medMetaDataKeys::Protocol.set ( medData, protocol );
        medMetaDataKeys::Referee.set ( medData, referee );
        medMetaDataKeys::Institution.set ( medData, institution );
        medMetaDataKeys::Report.set ( medData, report );
        medMetaDataKeys::Status.set ( medData, status );
        medMetaDataKeys::SequenceName.set ( medData, sequenceName );
        medMetaDataKeys::SliceThickness.set ( medData, sliceThickness );
        medMetaDataKeys::SeriesNumber.set(medData, seriesNumber);

        // MR
        medMetaDataKeys::Origin.set(medData, origin);
        medMetaDataKeys::FlipAngle.set(medData, flipAngle);
        medMetaDataKeys::EchoTime.set(medData, echoTime);
        medMetaDataKeys::RepetitionTime.set(medData, repetitionTime);
        medMetaDataKeys::AcquisitionTime.set(medData, acquisitionTime);

        emit success ( this );
    }
    else
    {
        emit failure ( this );
    }

    return medData;
}

qint64 medDatabaseReader::getDataSize()
{
    QString filename = getFilePath();
    QFileInfo info ( filename );
    return info.size();
}

QString medDatabaseReader::getFilePath()
{
    QVariant  seriesDbId = d->index.seriesId();

    QSqlQuery query (medDatabaseController::instance()->database());

    QString filename;

    query.prepare ( "SELECT path, instance_path, isIndexed FROM image WHERE series = :series" );
    query.bindValue ( ":series", seriesDbId );

    if ( !query.exec() )
    {
        dtkDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

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

medAbstractData *medDatabaseReader::readFile( QString filename )
{
    QStringList filenames;
    filenames << filename;
    return this->readFile ( filenames );
}


medAbstractData *medDatabaseReader::readFile( const QStringList filenames )
{
    medAbstractData *medData = NULL;

    QList<QString> readers = medAbstractDataFactory::instance()->readers();

    for ( int i = 0; i < readers.size(); i++ )
    {

        dtkSmartPointer<dtkAbstractDataReader> dataReader;
        dataReader = medAbstractDataFactory::instance()->readerSmartPointer ( readers[i] );

        connect ( dataReader, SIGNAL ( progressed ( int ) ), this, SIGNAL ( progressed ( int ) ) );
        if ( dataReader->canRead ( filenames ) )
        {
            dataReader->read ( filenames );
            dataReader->enableDeferredDeletion ( false );
            medData = dynamic_cast<medAbstractData*>(dataReader->data());

            break;
        }

    }
    return medData;
}
