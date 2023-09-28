/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractDataReader.h>

#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medAbstractImageData.h>
#include <medDatabaseReader.h>
#include <medDataIndex.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <medStorage.h>

class medDatabaseReaderPrivate
{
public:
    medDataIndex index;
    medDatabaseReader::ReadMode readMode;
};

medDatabaseReader::medDatabaseReader ( const medDataIndex& index ) : QObject(), d ( new medDatabaseReaderPrivate )
{
    d->index = index;
    d->readMode = READ_ALL;
}

medDatabaseReader::~medDatabaseReader()
{
    delete d;

    d = nullptr;
}

void medDatabaseReader::setReadMode(ReadMode readMode)
{
    d->readMode = readMode;
}

medDatabaseReader::ReadMode medDatabaseReader::getReadMode() const
{
    return d->readMode;
}

#define FAILURE(msg) do {qDebug() <<  "medDatabaseReader::run: "<<(msg);emit failure(this);return nullptr;} while(0)

medAbstractData* medDatabaseReader::run()
{
    QVariant patientDbId = d->index.patientId();
    QVariant   studyDbId = d->index.studyId();
    QVariant  seriesDbId = d->index.seriesId();

    QSqlDatabase dbConnection = medDataManager::instance().controller()->getThreadSpecificConnection();
    QSqlQuery query(dbConnection);

    QString patientName, birthdate, gender, patientId;
    QString studyName, studyUid, studyId, studyTime, studyDate;
    QString seriesName, seriesPath, seriesUid, orientation, seriesNumber, sequenceName,
            sliceThickness, rows, columns, thumbnailPath, age, description, protocol,
            comments, status, acquisitiondate, importationdate, referee,
            institution, report, modality, seriesId, seriesTime, seriesDate, kvp;
    QString origin, flipAngle, echoTime, repetitionTime, acquisitionTime;
    bool indexed;

    query.prepare ( "SELECT name, birthdate, gender, patientId FROM patient WHERE id = :id" );
    query.bindValue ( ":id", patientDbId );

    QMutexLocker mutexLocker(&medDataManager::instance().controller()->getDatabaseMutex());

    if ( !query.exec() )
    {
        query.finish();
        mutexLocker.unlock();
        FAILURE(query.lastError());
    }

    if (! query.first() )
    {
        query.finish();
        mutexLocker.unlock();
        FAILURE("No record in patient table for id:" + patientDbId.toString());
    }

    patientName = query.value ( 0 ).toString();
    birthdate = query.value ( 1 ).toString();
    gender = query.value ( 2 ).toString();
    patientId = query.value ( 3 ).toString();

    query.prepare ( "SELECT name, uid, studyId, time, date FROM study WHERE id = :id" );
    query.bindValue ( ":id", studyDbId );
    if ( !query.exec() )
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if (! query.first() )
    {
        query.finish();
        mutexLocker.unlock();
        FAILURE("No record in study table for id:" + studyDbId.toString());
    }

    studyName = query.value ( 0 ).toString();
    studyUid = query.value ( 1 ).toString();
    studyId = query.value ( 2 ).toString();
    studyTime = query.value ( 3 ).toString();
    studyDate = query.value ( 4 ).toString();

    query.prepare ( "SELECT name, uid, orientation, seriesNumber, sequenceName, sliceThickness, rows, columns, "
                    "age, description, protocol, comments, status, acquisitiondate, importationdate, referee, "
                    "institution, report, modality, seriesId, "
                    "origin, flipAngle, echoTime, repetitionTime, acquisitionTime, "
                    "path, thumbnail, isIndexed, time, date, kvp "
                    "FROM series WHERE id = :id" );
    query.bindValue ( ":id", seriesDbId );

    if ( !query.exec() )
    {
        query.finish();
        mutexLocker.unlock();
        FAILURE(query.lastError());
    }

    if ( ! query.first())
    {
        query.finish();
        mutexLocker.unlock();
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
    age = query.value ( 8 ).toString();
    description = query.value ( 9 ).toString();
    protocol = query.value ( 10 ).toString();
    comments = query.value ( 11 ).toString();
    status = query.value ( 12 ).toString();
    acquisitiondate = query.value ( 13 ).toString();
    importationdate = query.value ( 14 ).toString();
    referee = query.value ( 15 ).toString();
    institution = query.value ( 16 ).toString();
    report = query.value ( 17 ).toString();
    modality = query.value ( 18 ).toString();
    seriesId = query.value ( 19 ).toString();

    // MR
    origin          = query.value ( 20 ).toString();
    flipAngle       = query.value ( 21 ).toString();
    echoTime        = query.value ( 22 ).toString();
    repetitionTime  = query.value ( 23 ).toString();
    acquisitionTime = query.value ( 24 ).toString();

    seriesPath = query.value ( 25 ).toString();
    thumbnailPath = query.value ( 26 ).toString();
    indexed = query.value ( 27 ).toBool();
    seriesTime = query.value ( 28 ).toString();
    seriesDate = query.value ( 29 ).toString();
    kvp = query.value ( 30 ).toString();

    query.finish();
    mutexLocker.unlock();

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
    medMetaDataKeys::SeriesThumbnail.add (medData, fullThumbnailPath);

    medMetaDataKeys::PatientID.set ( medData, patientId );
    medMetaDataKeys::PatientName.set ( medData, patientName );
    medMetaDataKeys::BirthDate.set ( medData, birthdate );
    medMetaDataKeys::Gender.set ( medData, gender );
    medMetaDataKeys::StudyDescription.set ( medData, studyName );
    medMetaDataKeys::StudyID.set ( medData, studyId );
    medMetaDataKeys::StudyInstanceUID.set ( medData, studyUid );
    medMetaDataKeys::StudyTime.set ( medData, studyTime );
    medMetaDataKeys::StudyDate.set ( medData, studyDate );
    medMetaDataKeys::SeriesDescription.set ( medData, seriesName );
    medMetaDataKeys::SeriesID.set ( medData, seriesId );
    medMetaDataKeys::SeriesInstanceUID.set ( medData, seriesUid );
    medMetaDataKeys::Orientation.set ( medData, orientation );
    medMetaDataKeys::Columns.set ( medData, columns );
    medMetaDataKeys::Rows.set ( medData, rows );
    medMetaDataKeys::AcquisitionDate.set ( medData, acquisitiondate );
    medMetaDataKeys::Comments.set ( medData, comments );
    medMetaDataKeys::Age.set( medData, age);
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
    medMetaDataKeys::SeriesTime.set(medData, seriesTime);
    medMetaDataKeys::SeriesDate.set(medData, seriesDate);
    medMetaDataKeys::KVP.set(medData, kvp);

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
    medAbstractData *medData = nullptr;

    QList<QString> readers = medAbstractDataFactory::instance()->readers();

    for ( int i = 0; i < readers.size(); i++ )
    {

        dtkSmartPointer<dtkAbstractDataReader> dataReader;
        dataReader = medAbstractDataFactory::instance()->readerSmartPointer ( readers[i] );

        connect ( dataReader, SIGNAL ( progressed ( int ) ), this, SIGNAL ( progressed ( int ) ) );
        if ( dataReader->canRead ( filenames ) )
        {
            if (d->readMode == READ_INFORMATION)
            {
                dataReader->readInformation(filenames);
            }
            else
            {
                if (dataReader->read(filenames) == false)
                {
                    break;
                }
            }
            dataReader->enableDeferredDeletion ( false );
            medData = dynamic_cast<medAbstractData*>(dataReader->data());

            break;
        }

    }
    return medData;
}
