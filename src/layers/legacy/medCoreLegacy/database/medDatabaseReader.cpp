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
#include <dtkLog>

#include <QtSql/QSqlError>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medAbstractImageData.h>
#include <medDatabaseController.h>
#include <medDatabaseReader.h>
#include <medMetaDataKeys.h>
#include <medStorage.h>

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

#define FAILURE(msg) do {qDebug() <<  "medDatabaseReader::run: "<<(msg);emit failure(this);return NULL;} while(0)

medAbstractData* medDatabaseReader::run()
{
    QVariant patientDbId = d->index.patientId();
    QVariant   studyDbId = d->index.studyId();
    QVariant  seriesDbId = d->index.seriesId();

    QSqlQuery query(medDatabaseController::instance()->database());

    QString patientName, birthdate, gender, patientId;
    QString studyName, studyUid, studyId;
    QString seriesName, seriesPath, seriesUid, orientation, seriesNumber, sequenceName,
            sliceThickness, rows, columns, thumbnailPath, description, protocol,
            comments, status, acquisitiondate, importationdate, referee,
            institution, report, modality, seriesId;
    QString origin, flipAngle, echoTime, repetitionTime, acquisitionTime;
    bool indexed;

    query.prepare ( "SELECT name, birthdate, gender, patientId FROM patient WHERE id = :id" );
    query.bindValue ( ":id", patientDbId );
    if ( !query.exec() )
    {
        FAILURE(query.lastError());
    }

    if (! query.first() )
    {
        FAILURE("No record in patient table for id:" + patientDbId.toString());
    }

    patientName = query.value ( 0 ).toString();
    birthdate = query.value ( 1 ).toString();
    gender = query.value ( 2 ).toString();
    patientId = query.value ( 3 ).toString();

    query.prepare ( "SELECT name, uid, studyId FROM study WHERE id = :id" );
    query.bindValue ( ":id", studyDbId );
    if ( !query.exec() )
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if (! query.first() )
    {
        FAILURE("No record in study table for id:" + studyDbId.toString());
    }

    studyName = query.value ( 0 ).toString();
    studyUid = query.value ( 1 ).toString();
    studyId = query.value ( 2 ).toString();

    query.prepare ( "SELECT name, uid, orientation, seriesNumber, sequenceName, sliceThickness, rows, columns, "
                    "description, protocol, comments, status, acquisitiondate, importationdate, referee, "
                    "institution, report, modality, seriesId, "
                    "origin, flipAngle, echoTime, repetitionTime, acquisitionTime, "
                    "path, thumbnail, isIndexed "
                    "FROM series WHERE id = :id" );
    query.bindValue ( ":id", seriesDbId );

    if ( !query.exec() )
    {
        FAILURE(query.lastError());
    }

    if ( ! query.first())
    {
        FAILURE("No record in series table for id:" + seriesDbId.toString());
    }

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

    seriesPath = query.value ( 24 ).toString();
    thumbnailPath = query.value ( 25 ).toString();
    indexed = query.value ( 26 ).toBool();

    QStringList filePaths = seriesPath.split(';', QString::SkipEmptyParts);
    for(int i = 0 ; i < filePaths.size(); i++)
    {
        // Non-indexed file paths are relative to the DB directory
        QString fullPath = (indexed ? QString() : medStorage::dataLocation()) + filePaths.at(i);
        QFileInfo fullPathInfo(fullPath);
        if ( ! fullPathInfo.exists())
        {
            FAILURE("No data found at path:" + fullPath);
        }
        filePaths[i] = fullPath;
    }

    medAbstractData *medData = this->readFile(filePaths);

    if (!medData)
    {
        FAILURE("Failed to read data at path:" + seriesPath);
    }

    QString fullThumbnailPath(medStorage::dataLocation() + thumbnailPath);
    QFileInfo fullThumbnailPathInfo(fullThumbnailPath);
    if (!fullThumbnailPathInfo.exists())
    {
        qWarning("No thumbnail found at path: %s", qPrintable(fullThumbnailPath));
    }
    medMetaDataKeys::SeriesThumbnail.add (medData, fullThumbnailPath);

    medMetaDataKeys::PatientID.set ( medData, patientId );
    medMetaDataKeys::PatientName.set ( medData, patientName );
    medMetaDataKeys::BirthDate.set ( medData, birthdate );
    medMetaDataKeys::Gender.set ( medData, gender );
    medMetaDataKeys::StudyDescription.set ( medData, studyName );
    medMetaDataKeys::StudyID.set ( medData, studyId );
    medMetaDataKeys::StudyInstanceUID.set ( medData, studyUid );
    medMetaDataKeys::SeriesDescription.set ( medData, seriesName );
    medMetaDataKeys::SeriesID.set ( medData, seriesId );
    medMetaDataKeys::SeriesInstanceUID.set ( medData, seriesUid );
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

    return medData;
}

medAbstractData *medDatabaseReader::readFile( const QStringList& filenames )
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
