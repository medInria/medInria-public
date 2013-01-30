/* medDatabaseImporter.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jan 19 13:42:32 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Oct  7 15:48:22 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 48
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medAbstractDatabaseImporter.h"

#include <medAbstractDataImage.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>
#include <medDatabaseController.h>
#include <medMetaDataKeys.h>
#include <medStorage.h>

class medAbstractDatabaseImporterPrivate
{
public:
    QString file;
    static QMutex mutex;
    QString lastSuccessfulReaderDescription;
    QString lastSuccessfulWriterDescription;
    bool isCancelled;
    bool indexWithoutImporting;

    // example of item in the list: ["patient", "study", "series"]
    QList<QStringList> partialAttemptsInfo;

    QMap<int, QString> volumeIdToImageFile;
};

QMutex medAbstractDatabaseImporterPrivate::mutex;

//-----------------------------------------------------------------------------------------------------------

medAbstractDatabaseImporter::medAbstractDatabaseImporter ( const QString& file, bool indexWithoutImporting = false ) : medJobItem(), d ( new medAbstractDatabaseImporterPrivate )
{
    d->isCancelled = false;
    d->lastSuccessfulReaderDescription = "";
    d->lastSuccessfulWriterDescription = "";
    d->file = file;
    d->indexWithoutImporting = indexWithoutImporting;
}

//-----------------------------------------------------------------------------------------------------------

medAbstractDatabaseImporter::~medAbstractDatabaseImporter ( void )
{
    delete d;

    d = NULL;
}

//-----------------------------------------------------------------------------------------------------------

void medAbstractDatabaseImporter::onCancel ( QObject* )
{
    d->isCancelled = true;
}

void medAbstractDatabaseImporter::populateMissingMetadata ( dtkAbstractData* dtkData, const QString seriesDescription )
{
    if ( !dtkData )
    {
        qWarning() << "data invalid";
        return;
    }


    QString newSeriesDescription;
    // check if image have basic information like patient, study, etc.
    // DICOMs, for instance, do provide it
    if ( !dtkData->hasMetaData ( medMetaDataKeys::PatientName.key() ) &&
         !dtkData->hasMetaData ( medMetaDataKeys::StudyDescription.key() ) &&
         !dtkData->hasMetaData ( medMetaDataKeys::SeriesDescription.key() ) )
    {
        // if none of these fields could be read from the file(s) then we won't be able to know for sure
        // if it was previously imported/indexed as it could happen that it is just another file with the same path
        // see http://pm-med.inria.fr/issues/292
        dtkData->addMetaData ( medMetaDataKeys::ContainsBasicInfo.key(), "false" );

        // it could be that we have already another image with this characteristics
        // so we would like to check whether the image filename is on the db
        // and if so we would add some suffix to distinguish it

        newSeriesDescription = ensureUniqueSeriesName(seriesDescription);
    }
    else
    {
        dtkData->addMetaData ( medMetaDataKeys::ContainsBasicInfo.key(), "true" );
        newSeriesDescription = seriesDescription;
    }

    if ( !dtkData->hasMetaData ( medMetaDataKeys::PatientName.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::PatientName.key(), QStringList() << "John Doe" );

    if (!dtkData->hasMetaData ( medMetaDataKeys::PatientID.key() ) )
      dtkData->addMetaData ( medMetaDataKeys::PatientID.key(), QStringList() << "0" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::StudyDescription.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::StudyDescription.key(), QStringList() << "EmptyStudy" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::SeriesDescription.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::SeriesDescription.key(), QStringList() << newSeriesDescription );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::StudyID.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::StudyID.key(), QStringList() << "0" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::StudyDicomID.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::StudyDicomID.key(), QStringList() << "" );

    QString generatedSeriesId = QUuid::createUuid().toString().replace("{","").replace("}","");

    if ( !dtkData->hasMetaData ( medMetaDataKeys::SeriesID.key() ) )
      dtkData->addMetaData ( medMetaDataKeys::SeriesID.key(), QStringList() << generatedSeriesId);

    QString generatedSeriesDicomID = QUuid::createUuid().toString().replace("{","").replace("}","");

    if ( !dtkData->hasMetaData ( medMetaDataKeys::SeriesDicomID.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::SeriesDicomID.key(), QStringList() << generatedSeriesDicomID );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::Orientation.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::Orientation.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::SeriesNumber.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::SeriesNumber.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::SequenceName.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::SequenceName.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::SliceThickness.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::SliceThickness.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::Rows.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::Rows.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::Columns.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::Columns.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::Age.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::Age.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::BirthDate.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::BirthDate.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::Gender.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::Gender.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::Description.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::Description.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::Modality.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::Modality.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::Protocol.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::Protocol.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::Comments.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::Comments.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::Status.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::Status.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::AcquisitionDate.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::AcquisitionDate.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::ImportationDate.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::ImportationDate.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::Referee.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::Referee.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::Performer.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::Performer.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::Institution.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::Institution.key(), QStringList() << "" );

    if ( !dtkData->hasMetaData ( medMetaDataKeys::Report.key() ) )
        dtkData->addMetaData ( medMetaDataKeys::Report.key(), QStringList() << "" );
}

//-----------------------------------------------------------------------------------------------------------

QStringList medAbstractDatabaseImporter::generateThumbnails ( dtkAbstractData* dtkData, QString pathToStoreThumbnails )
{
    QList<QImage> &thumbnails = dtkData->thumbnails();

    QStringList thumbPaths;

    if ( !medStorage::mkpath ( medStorage::dataLocation() + pathToStoreThumbnails ) )
        qDebug() << "Cannot create directory: " << pathToStoreThumbnails;

    for ( int i=0; i < thumbnails.count(); i++ )
    {
        QString thumb_name = pathToStoreThumbnails + QString().setNum ( i ) + ".png";
        thumbnails[i].save ( medStorage::dataLocation() + thumb_name, "PNG" );
        thumbPaths << thumb_name;
    }

    QImage refThumbnail = dtkData->thumbnail(); // representative thumbnail for PATIENT/STUDY/SERIES
    QString refThumbPath = pathToStoreThumbnails + "ref.png";
    refThumbnail.save ( medStorage::dataLocation() + refThumbPath, "PNG" );

    dtkData->addMetaData ( medMetaDataKeys::ThumbnailPath.key(), refThumbPath );

    return thumbPaths;
}

//-----------------------------------------------------------------------------------------------------------

dtkSmartPointer<dtkAbstractDataReader> medAbstractDatabaseImporter::getSuitableReader ( QStringList filename )
{
    QList<QString> readers = dtkAbstractDataFactory::instance()->readers();

    if ( readers.size() ==0 )
    {
        emit showError ( this, tr ( "No reader plugin" ), 5000 );
        emit failure ( this );
        return NULL;
    }

    // cycle through readers to see if the last used reader can handle the file
    dtkSmartPointer<dtkAbstractDataReader> dataReader;
    for (int i=0; i<readers.size(); i++) {
        dataReader = dtkAbstractDataFactory::instance()->readerSmartPointer(readers[i]);
        if (d->lastSuccessfulReaderDescription == dataReader->identifier() && dataReader->canRead(filename)) {
            dataReader->enableDeferredDeletion(false);
            return dataReader;
        }
    }

    for (int i=0; i<readers.size(); i++) {
        dataReader = dtkAbstractDataFactory::instance()->readerSmartPointer(readers[i]);
        if (dataReader->canRead(filename)) {
            d->lastSuccessfulReaderDescription = dataReader->identifier();
            dataReader->enableDeferredDeletion(false);
            return dataReader;
        }
    }

    qWarning() << "No suitable reader found!";
    return NULL;
}

//-----------------------------------------------------------------------------------------------------------

dtkSmartPointer<dtkAbstractDataWriter> medAbstractDatabaseImporter::getSuitableWriter(QString filename,dtkAbstractData* dtkData)
{
    if ( !dtkData )
        return NULL;

    QList<QString> writers = dtkAbstractDataFactory::instance()->writers();
    dtkSmartPointer<dtkAbstractDataWriter> dataWriter;
    // first try with the last
    for (int i=0; i<writers.size(); i++) {
        dataWriter = dtkAbstractDataFactory::instance()->writerSmartPointer(writers[i]);
        if (d->lastSuccessfulWriterDescription==dataWriter->identifier()) {
            if (dataWriter->handled().contains(dtkData->identifier()) && dataWriter->canWrite(filename)) {

                d->lastSuccessfulWriterDescription = dataWriter->identifier();
                dataWriter->enableDeferredDeletion(false);
                return dataWriter;
            }
        }
    }

    // cycle all
    for ( int i=0; i<writers.size(); i++ )
    {
        dataWriter = dtkAbstractDataFactory::instance()->writerSmartPointer ( writers[i] );

        if (dataWriter->handled().contains(dtkData->identifier()) &&
             dataWriter->canWrite( filename ) ) {

            d->lastSuccessfulWriterDescription = dataWriter->identifier();
            dataWriter->enableDeferredDeletion(false);
            return dataWriter;
        }
    }
    return NULL;
}

//-----------------------------------------------------------------------------------------------------------

QStringList medAbstractDatabaseImporter::getAllFilesToBeProcessed ( QString fileOrDirectory )
{
    QString file = fileOrDirectory;

    QDir dir ( file );
    dir.setFilter ( QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot );

    QStringList fileList;
    if ( dir.exists() )
    {
        QDirIterator directory_walker ( file, QDir::Files, QDirIterator::Subdirectories );
        while ( directory_walker.hasNext() )
        {
            fileList << directory_walker.next();
        }
    }
    else
        fileList << file;

    fileList.sort();

    return fileList;
}

dtkSmartPointer<dtkAbstractData> medAbstractDatabaseImporter::tryReadImages ( const QStringList& filesPaths,const bool readOnlyImageInformation )
{
    dtkSmartPointer<dtkAbstractData> dtkData = 0;

    dtkSmartPointer<dtkAbstractDataReader> dataReader;
    dataReader = getSuitableReader ( filesPaths );

    if ( dataReader )
    {
        if ( readOnlyImageInformation )
            dataReader->readInformation ( filesPaths );
        else
            dataReader->read ( filesPaths );

        dtkData = dataReader->data();
    }
    else
    {
        // we take the first one for debugging just for simplicity
        qWarning() << "No suitable reader found for file: " << filesPaths[0] << ". Unable to import!";
    }

    return dtkData;
}

// QString medDatabaseImporter::determineFutureImageFileName(const dtkAbstractData* dtkdata, int volumeNumber)
QString medAbstractDatabaseImporter::determineFutureImageFileName ( const dtkAbstractData* dtkdata, int volumeNumber )
{
    // we cache the generated file name corresponding to volume number
    // because:
    // 1. it ensures that all data belonging to the same volume will have the
    //    same file name
    // 2. if data belonging to the same volume do not have the metaData SeriesId,
    //    one unique Id will be generated in populateMissingData(), creating a new
    //    file name in the rest of this code, leading to point 1.

    if (!d->volumeIdToImageFile[volumeNumber].isEmpty())
        return d->volumeIdToImageFile[volumeNumber];

    // we append the uniqueID at the end of the filename to have unique filenames for each volume
    QString s_volumeNumber;
    s_volumeNumber.setNum ( volumeNumber );

    QString patientID = medMetaDataKeys::PatientID.getFirstValue(dtkdata);
    QString seriesID   = medMetaDataKeys::SeriesID.getFirstValue(dtkdata);

    QString imageFileName = QDir::separator() + QString ( patientID )
                          + QDir::separator() + QString ( seriesID ) + s_volumeNumber;

    d->volumeIdToImageFile[volumeNumber] = imageFileName;

    return imageFileName;
}

QString medAbstractDatabaseImporter::determineFutureImageExtensionByDataType ( const dtkAbstractData* dtkdata )
{
    QString identifier = dtkdata->identifier();
    QString extension = "";

     // Determine the appropriate extension to use according to the type of data.
     // TODO: The image and CompositeDatasets types are weakly recognized (contains("Image/CompositeData")). to be improved
     if (identifier == "vtkDataMesh") {
         extension = ".vtk";
         qDebug() << "vtkDataMesh";
     } else if (identifier == "vtkDataMesh4D") {
         extension = ".v4d";
         qDebug() << "vtkDataMesh4D";
     } else if (identifier == "v3dDataFibers") {
         extension = ".xml";
         qDebug() << "vtkDataMesh4D";
     } else if (identifier.contains("vistal")) {
         extension = ".dim";
         qDebug() << "Vistal Image";
     } else if (identifier.contains ("CompositeData")) {
        extension = ".cds";
        qDebug() << "composite Dataset";
     } else if (identifier.contains ("Image")) {
         extension = ".mha";
         //qDebug() << identifier;
     }

    return extension;
}

bool medAbstractDatabaseImporter::tryWriteImage ( QString filePath, dtkAbstractData* imData )
{
    dtkSmartPointer<dtkAbstractDataWriter> dataWriter = getSuitableWriter ( filePath, imData );
    if ( dataWriter )
    {
        dataWriter->setData ( imData );
        if ( dataWriter->write ( filePath ) )
            return true;
    }
    return false;
}

void medAbstractDatabaseImporter::addAdditionalMetaData ( dtkAbstractData* imData, QString aggregatedFileName, QStringList aggregatedFilesPaths )
{
    QStringList size;
    if ( medAbstractDataImage *imageData = dynamic_cast<medAbstractDataImage*> ( imData ) )
    {
        size << QString::number ( imageData->zDimension() );
    }
    else
    {
        size << "";
    }

    imData->setMetaData ( "Size", size );

    if ( !imData->hasMetaData ( "FilePaths" ) )
        imData->addMetaData ( "FilePaths", aggregatedFilesPaths );

    imData->addMetaData ( "FileName", aggregatedFileName );
}

QString medAbstractDatabaseImporter::generateUniqueVolumeId ( const dtkAbstractData* dtkData )
{
    if ( !dtkData )
    {
        qWarning() << "data invalid";
        return "invalid";
    }

    // Get all the information from the dtkAbstractData metadata.
    // This information will then be passed to the database.
    QString patientName = medMetaDataKeys::PatientName.getFirstValue(dtkData);
    QString studyDicomId = medMetaDataKeys::StudyDicomID.getFirstValue(dtkData);
    QString seriesDicomId = medMetaDataKeys::SeriesDicomID.getFirstValue(dtkData);
    QString orientation = medMetaDataKeys::Orientation.getFirstValue(dtkData); // orientation sometimes differ by a few digits, thus this is not reliable
    QString seriesNumber = medMetaDataKeys::SeriesNumber.getFirstValue(dtkData);
    QString sequenceName = medMetaDataKeys::SequenceName.getFirstValue(dtkData);
    QString sliceThickness = medMetaDataKeys::SliceThickness.getFirstValue(dtkData);
    QString rows = medMetaDataKeys::Rows.getFirstValue(dtkData);
    QString columns = medMetaDataKeys::Columns.getFirstValue(dtkData);

    QStringList orientations = orientation.split ( " " );

    orientation = "";

    // truncate orientation to 5 digits for a more robust import since
    // sometimes orientation only differs with the last 2 digits, creating
    // multiple series
    foreach ( QString orient, orientations )
    {
        double d_orient = orient.toDouble();
        orientation += QString::number ( d_orient, 'g', 5 );
    }

    // define a unique key string to identify which volume an image belongs to.
    // we use: patientName, studyId, seriesId, orientation, seriesNumber, sequenceName, sliceThickness, rows, columns.
    // All images of the same volume should share similar values of these parameters
    QString key = patientName+studyDicomId+seriesDicomId+orientation+seriesNumber+sequenceName+sliceThickness+rows+columns;

    return key;
}
