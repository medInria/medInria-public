/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QSqlError>

#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>

#include <medAbstractDataFactory.h>
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

#define FAILURE(msg) do {qDebug() <<  "medDatabaseReader::run:"<<(msg);emit failure(this);return NULL;} while(0)

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
    bool indexed;

    query.prepare ( "SELECT name, birthdate, gender, patientId FROM patient WHERE id = :id" );
    query.bindValue ( ":id", patientDbId );
    if ( ! query.exec())
        FAILURE(query.lastError());
    if ( ! query.first())
        FAILURE("No record in patient table for id:" + patientDbId.toString());

    patientName = query.value ( 0 ).toString();
    birthdate = query.value ( 1 ).toString();
    gender = query.value ( 2 ).toString();
    patientId = query.value ( 3 ).toString();

    query.prepare ( "SELECT name, uid, studyId FROM study WHERE id = :id" );
    query.bindValue ( ":id", studyDbId );
    if ( ! query.exec())
        FAILURE(query.lastError());
    if ( ! query.first())
        FAILURE("No record in study table for id:" + studyDbId.toString());

    studyName = query.value ( 0 ).toString();
    studyUid = query.value ( 1 ).toString();
    studyId = query.value ( 2 ).toString();

    query.prepare ( "SELECT name, uid, orientation, seriesNumber, sequenceName, sliceThickness, rows, columns, \
                     description, protocol, comments, status, acquisitiondate, importationdate, referee,       \
                     institution, report, modality, seriesId, path, thumbnail, isIndexed \
                     FROM series WHERE id = :id" );

    query.bindValue ( ":id", seriesDbId );

    if ( ! query.exec())
        FAILURE(query.lastError());
    if ( ! query.first())
        FAILURE("No record in series table for id:" + seriesDbId.toString());

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
    seriesPath = query.value ( 19 ).toString();
    thumbnailPath = query.value ( 20 ).toString();
    indexed = query.value ( 21 ).toBool();

    QStringList filePaths = seriesPath.split(';', QString::SkipEmptyParts);
    for(int i = 0 ; i < filePaths.size(); i++) {
        // Non-indexed file paths are relative to the DB directory
        QString fullPath = (indexed ? QString() : medStorage::dataLocation()) + filePaths.at(i);
        QFileInfo fullPathInfo(fullPath);
        if ( ! fullPathInfo.exists())
            FAILURE("No data found at path:" + fullPath);
        filePaths[i] = fullPath;
    }

    medAbstractData *medData = this->readFile(filePaths);

    if ( ! medData)
        FAILURE("Failed to read data at path:" + seriesPath);

    QString fullThumbnailPath(medStorage::dataLocation() + thumbnailPath);
    QFileInfo fullThumbnailPathInfo(fullThumbnailPath);
    if ( ! fullThumbnailPathInfo.exists())
        qWarning("No thumbnail found at path: %s", qPrintable(fullThumbnailPath));

    medMetaDataKeys::SeriesThumbnail.add (medData, fullThumbnailPath);

    medMetaDataKeys::PatientID.add ( medData, patientId );
    medMetaDataKeys::PatientName.add ( medData, patientName );
    medMetaDataKeys::BirthDate.add ( medData, birthdate );
    medMetaDataKeys::Gender.add ( medData, gender );
    medMetaDataKeys::StudyDescription.add ( medData, studyName );
    medMetaDataKeys::StudyID.add ( medData, studyId );
    medMetaDataKeys::StudyDicomID.add ( medData, studyUid );
    medMetaDataKeys::SeriesDescription.add ( medData, seriesName );
    medMetaDataKeys::SeriesID.add ( medData, seriesId );
    medMetaDataKeys::SeriesDicomID.add ( medData, seriesUid );
    medMetaDataKeys::Orientation.add ( medData, orientation );
    medMetaDataKeys::Columns.add ( medData, columns );
    medMetaDataKeys::Rows.add ( medData, rows );
    medMetaDataKeys::AcquisitionDate.add ( medData, acquisitiondate );
    medMetaDataKeys::Comments.add ( medData, comments );
    medMetaDataKeys::Description.add ( medData, description );
    medMetaDataKeys::ImportationDate.add ( medData, importationdate );
    medMetaDataKeys::Modality.add ( medData, modality );
    medMetaDataKeys::Protocol.add ( medData, protocol );
    medMetaDataKeys::Referee.add ( medData, referee );
    medMetaDataKeys::Institution.add ( medData, institution );
    medMetaDataKeys::Report.add ( medData, report );
    medMetaDataKeys::Status.add ( medData, status );
    medMetaDataKeys::SequenceName.add ( medData, sequenceName );
    medMetaDataKeys::SliceThickness.add ( medData, sliceThickness );
    medMetaDataKeys::SeriesNumber.add(medData, seriesNumber);

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
