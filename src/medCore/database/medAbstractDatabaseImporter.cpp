/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDatabaseImporter.h>

#include <medAbstractImageData.h>

#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <medAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>
#include <medDatabaseController.h>
#include <medMetaDataKeys.h>
#include <medStorage.h>

class medAbstractDatabaseImporterPrivate
{
public:
    QString file;
    medAbstractData *data;
    static QMutex mutex;
    bool isCancelled;
    bool indexWithoutImporting;
    medDataIndex index;

    QMap<int, QString> volumeIdToImageFile;
    
    QUuid uuid;
};

QMutex medAbstractDatabaseImporterPrivate::mutex;

//-----------------------------------------------------------------------------------------------------------

medAbstractDatabaseImporter::medAbstractDatabaseImporter ( const QString& file, const QUuid& uuid, bool indexWithoutImporting) : medAbstractJob("File import"), d ( new medAbstractDatabaseImporterPrivate )
{
    d->isCancelled = false;
    d->file = file;
    d->data = NULL;
    d->indexWithoutImporting = indexWithoutImporting;
    d->uuid = uuid;
}

//-----------------------------------------------------------------------------------------------------------

medAbstractDatabaseImporter::medAbstractDatabaseImporter ( medAbstractData* medData, const QUuid& uuid) : medAbstractJob("Data import"), d ( new medAbstractDatabaseImporterPrivate )
{
    d->isCancelled = false;
    d->data = medData;
    d->file = QString("");
    d->indexWithoutImporting = false;
    d->uuid = uuid;
}

//-----------------------------------------------------------------------------------------------------------

medAbstractDatabaseImporter::~medAbstractDatabaseImporter ( void )
{
    delete d;

    d = NULL;
}

/**
* Returns file or directory used for import.
**/
QString medAbstractDatabaseImporter::file ( void )
{
    return d->file;
}

/**
* Returns if pocess has been cancelled.
**/
bool medAbstractDatabaseImporter::isCancelled ( void )   
{
    return d->isCancelled;
}

/**
* Returns true if process is indexing without importing.
**/
bool medAbstractDatabaseImporter::indexWithoutImporting ( void )   
{
    return d->indexWithoutImporting;
}

/**
* Returns a QMap linking volume id to image file.
**/
QMap<int, QString> medAbstractDatabaseImporter::volumeIdToImageFile ( void )  
{
    return d->volumeIdToImageFile;
}

/**
  Returns the index of the data which has been read. Index is not
  valid if reading was not successful.
*/
medDataIndex medAbstractDatabaseImporter::index() const
{
    return d->index;
}

/**
* Returns caller Uuid.
**/
QString medAbstractDatabaseImporter::callerUuid()
{
    return d->uuid;
}
    
/**
* Runs the import process based on the input file
* or directory given in the constructor
**/
void medAbstractDatabaseImporter::internalRun ( void )
{
    if(!d->file.isEmpty())
        importFile();
    else if ( d->data )
        importData();
}


void medAbstractDatabaseImporter::importFile ( void )
{
    QMutexLocker locker ( &d->mutex );

    /* The idea of this algorithm can be summarized in 3 steps:
     * 1. Get a list of all the files that will (try to) be imported or indexed
     * 2. Filter files that cannot be read, or won't be possible to write afterwards, or are already in the db
     * 3. Fill files metadata, write them to the db, and populate db tables
     *
     * note that depending on the input files, they might be aggregated by volume
     */

    // 1) Obtain a list of all the files that are going to be processed
    // this flattens the tree structure (if the input is a directory)
    // and puts all the files in one single list
    QStringList fileList = getAllFilesToBeProcessed ( d->file );

    // Files that pass the filters named above are grouped
    // by volume in this map and will be written in the db after.
    // the key will be the name of the aggregated file with the volume
    QMap<QString, QStringList> imagesGroupedByVolume;
    QMap<QString, QString> imagesGroupedByPatient;
    QMap<QString, QString> imagesGroupedBySeriesId;

    int currentFileNumber = 0; // this variable will be used only for calculating progress

    // if importing, and depending on the input files, they might be aggregated
    // that is: files corresponding to the same volume will be written
    // in a single output meta file (e.g. .mha)
    // this map is used to store a unique id per volume and its volume number
    QMap<QString, int> volumeUniqueIdToVolumeNumber;
    int volumeNumber = 1;

    // 2) Select (by filtering) files to be imported
    //
    // In this first loop we read the headers of all the images to be imported
    // and check if we don't have any problem in reading the file, the header
    // or in selecting a proper format to store the new file afterwards
    // new files ARE NOT written in medInria database yet, but are stored in a map for writing in a posterior step

    QString tmpPatientId;
    QString currentPatientId = "";
    QString patientID;

    QString tmpSeriesUid;
    QString currentSeriesUid = "-1";
    QString currentSeriesId = "";

    bool atLeastOneImportSucceeded = false;

    foreach ( QString file, fileList )
    {
        if ( d->isCancelled ) // check if user cancelled the process
            break;

        emit progressed ( ( ( qreal ) currentFileNumber/ ( qreal ) fileList.count() ) * 50.0 ); //TODO: reading and filtering represents 50% of the importing process?

        currentFileNumber++;

        QFileInfo fileInfo ( file );

        dtkSmartPointer<medAbstractData> medData;

        // 2.1) Try reading file information, just the header not the whole file

        bool readOnlyImageInformation = true;
        medData = tryReadImages ( QStringList ( fileInfo.filePath() ), readOnlyImageInformation );

        if ( !medData )
        {
            qWarning() << "Reader was unable to read: " << fileInfo.filePath();
            continue;
        }

        // 2.2) Fill missing metadata
        populateMissingMetadata ( medData, fileInfo.baseName() );
        QString patientName = medMetaDataKeys::PatientName.getFirstValue(medData).simplified();
        QString birthDate = medMetaDataKeys::BirthDate.getFirstValue(medData);
        tmpPatientId = patientName + birthDate;

        if(tmpPatientId != currentPatientId)
        {
          currentPatientId = tmpPatientId;

          patientID = getPatientID(patientName, birthDate);
        }

        medData->setMetaData ( medMetaDataKeys::PatientID.key(), QStringList() << patientID );

        tmpSeriesUid = medMetaDataKeys::SeriesDicomID.getFirstValue(medData);

        if (tmpSeriesUid != currentSeriesUid)
        {
          currentSeriesUid = tmpSeriesUid;
          currentSeriesId = medMetaDataKeys::SeriesID.getFirstValue(medData);
        }
        else
          medData->setMetaData ( medMetaDataKeys::SeriesID.key(), QStringList() << currentSeriesId );

        // 2.3) Generate an unique id for each volume
        // all images of the same volume should share the same id
        QString volumeId = generateUniqueVolumeId ( medData );

        // check whether the image belongs to a new volume
        if ( !volumeUniqueIdToVolumeNumber.contains ( volumeId ) )
        {
            volumeUniqueIdToVolumeNumber[volumeId] = volumeNumber;
            volumeNumber++;
        }

        // 2.3) a) Determine future file name and path based on patient/study/series/image
        // i.e.: where we will write the imported image
        QString imageFileName = determineFutureImageFileName ( medData, volumeUniqueIdToVolumeNumber[volumeId] );
        #ifdef Q_OS_WIN32
                if ( (medStorage::dataLocation() + "/" + imageFileName).length() > 255 )
                {
                    emit showError ( tr ( "Your database path is too long" ), 5000 );
                    emit dataImported(medDataIndex(), d->uuid);
                emit failure ();
                return;
                }
            #endif
        // 2.3) b) Find the proper extension according to the type of the data
        // i.e.: in which format we will write the file in our database
        QString futureExtension  = determineFutureImageExtensionByDataType ( medData );

        // we care whether we can write the image or not if we are importing
        if (!d->indexWithoutImporting && futureExtension.isEmpty()) {
            emit showError(tr("Could not save file due to unhandled data type: ") + medData->identifier());
            continue;
        }

        imageFileName = imageFileName + futureExtension;

        // 2.3) c) Add the image to a map for writing them all in medInria's database in a posterior step

        // First check if patient/study/series/image path already exists in the database
        // Should we emit a message otherwise ??? TO
        if ( !checkIfExists ( medData, fileInfo.fileName() ) )
        {
            imagesGroupedByVolume[imageFileName] << fileInfo.filePath();
            imagesGroupedByPatient[imageFileName] = patientID;
            imagesGroupedBySeriesId[imageFileName] = currentSeriesId;
        }
    }

    // some checks to see if the user cancelled or something failed
    if ( d->isCancelled )
    {
        emit showError (tr ( "User cancelled import process" ), 5000 );
        emit dataImported(medDataIndex(), d->uuid);
        emit cancelled ();
        return;
    }

    // from now on the process cannot be cancelled
    emit disableCancel ();

    // 3) Re-read selected files and re-populate them with missing metadata
    //    then write them to medInria db and populate db tables

    QMap<QString, QStringList>::const_iterator it = imagesGroupedByVolume.begin();
    QMap<QString, QString>::const_iterator  itPat = imagesGroupedByPatient.begin();
    QMap<QString, QString>::const_iterator  itSer = imagesGroupedBySeriesId.begin();

    // 3.1) first check is after the filtering we have something to import
    // maybe we had problems with all the files, or they were already in the database
    if ( it == imagesGroupedByVolume.end() )
    {
        // TODO we know if it's either one or the other error, we can make this error better...
        emit showError (tr ( "No compatible image found or all of them had been already imported." ), 5000 );
        emit dataImported(medDataIndex(), d->uuid);
        emit failure ();
        return;
    }
    else
        qDebug() << "Image map contains " << imagesGroupedByVolume.size() << " files";

    int imagesCount = imagesGroupedByVolume.count(); // used only to calculate progress
    int currentImageIndex = 0; // used only to calculate progress

    medDataIndex index; //stores the last volume's index to be emitted on success

    // final loop: re-read, re-populate and write to db
    for ( ; it != imagesGroupedByVolume.end(); it++ )
    {
        emit progressed ( ( ( qreal ) currentImageIndex/ ( qreal ) imagesCount ) * 50.0 + 50.0 ); // 50? I do not think that reading all the headers is half the job...

        currentImageIndex++;

        QString aggregatedFileName = it.key(); // note that this file might be aggregating more than one input files
        QStringList filesPaths = it.value();   // input files being aggregated, might be only one or many
        patientID = itPat.value();
        QString seriesID = itSer.value();

        //qDebug() << currentImageIndex << ": " << aggregatedFileName << "with " << filesPaths.size() << " files";

        dtkSmartPointer<medAbstractData> imagemedData;

        QFileInfo imagefileInfo ( filesPaths[0] );

        // 3.2) Try to read the whole image, not just the header
        bool readOnlyImageInformation = false;
        imagemedData = tryReadImages ( filesPaths, readOnlyImageInformation );

        if ( imagemedData )
        {
            // 3.3) a) re-populate missing metadata
            // as files might be aggregated we use the aggregated file name as SeriesDescription (if not provided, of course)
            populateMissingMetadata ( imagemedData, imagefileInfo.baseName() );
            imagemedData->setMetaData ( medMetaDataKeys::PatientID.key(), QStringList() << patientID );
            imagemedData->setMetaData ( medMetaDataKeys::SeriesID.key(), QStringList() << seriesID );

            // 3.3) b) now we are able to add some more metadata
            addAdditionalMetaData ( imagemedData, aggregatedFileName, filesPaths );
        }
        else
        {
            qWarning() << "Could not repopulate data!";
            emit showError (tr ( "Could not read data: " ) + filesPaths[0], 5000 );
            emit dataImported(medDataIndex(), d->uuid);
            emit failure();
            return;
        }

        if ( !d->indexWithoutImporting )
        {
            // create location to store file
            QFileInfo fileInfo ( medStorage::dataLocation() + aggregatedFileName );
            if ( !fileInfo.dir().exists() && !medStorage::mkpath ( fileInfo.dir().path() ) )
            {
                qDebug() << "Cannot create directory: " << fileInfo.dir().path();
                continue;
            }

            // now writing file
            bool writeSuccess = tryWriteImage ( fileInfo.filePath(), imagemedData );

            if ( !writeSuccess )
            {
                emit showError (tr ( "Could not save data file: " ) + filesPaths[0], 5000 );
                continue;
            }
        }
        atLeastOneImportSucceeded = true;

        // and finally we populate the database
        QFileInfo aggregatedFileNameFileInfo ( aggregatedFileName );
        QString pathToStoreThumbnails = aggregatedFileNameFileInfo.dir().path() + "/" + aggregatedFileNameFileInfo.completeBaseName() + "/";
        index = this->populateDatabaseAndGenerateThumbnails ( imagemedData, pathToStoreThumbnails );
        
        if(!d->uuid.isNull())
        {
            emit dataImported(index, d->uuid);
        }
        else 
        {
            emit dataImported(index);
        }

        itPat++;
        itSer++;
    } // end of the final loop

    if ( ! atLeastOneImportSucceeded) {
        emit progressed ( 100 );
        emit dataImported(medDataIndex(), d->uuid);
        emit failure();
        return;
    }

    d->index = index;
    
    emit progressed ( 100 );
    emit success ( );
}

void medAbstractDatabaseImporter::importData()
{   
    QMutexLocker locker ( &d->mutex );
     
    if ( !d->data )
    {
        emit failure ( );
        emit dataImported(medDataIndex(), d->uuid);
        return;
    }

    populateMissingMetadata(d->data, "EmptySerie");
    
    if ( !d->data->hasMetaData ( medMetaDataKeys::FilePaths.key() ) )
        d->data->addMetaData ( medMetaDataKeys::FilePaths.key(), QStringList() << "generated with medInria" );
        

    QString size ="";
    if ( medAbstractImageData *imagedata = dynamic_cast<medAbstractImageData*> ( d->data) )
        size = QString::number ( imagedata->zDimension() );
    d->data->addMetaData ( medMetaDataKeys::Size.key(), size );

    QString patientName = medMetaDataKeys::PatientName.getFirstValue(d->data).simplified();
    QString birthDate   = medMetaDataKeys::BirthDate.getFirstValue(d->data);
    QString seriesId    = medMetaDataKeys::SeriesID.getFirstValue(d->data);
        
    QString patientId  = getPatientID(patientName, birthDate);

    d->data->setMetaData ( medMetaDataKeys::PatientID.key(), QStringList() << patientId );
 
    bool writeSuccess = true;
    QString     thumb_dir;

    if ( !d->indexWithoutImporting )
    {
        QString subDirName = "/" + patientId;
        QString imageFileNameBase =  subDirName + "/" +  seriesId;

        QDir dir ( medStorage::dataLocation() + subDirName );
        if ( !dir.exists() )
        {
            if ( !medStorage::mkpath ( medStorage::dataLocation() + subDirName ) )
            {
                qWarning() << "Unable to create directory for images";
                emit failure ( );
                emit dataImported(medDataIndex(), d->uuid);
                return ;
            }
        }

        QString extension  = determineFutureImageExtensionByDataType ( d->data );
        QString imageFileName = imageFileNameBase + extension;

        // writing file
        writeSuccess = tryWriteImage (  medStorage::dataLocation()+imageFileName, d->data );

        if ( !writeSuccess  )
        {
            // when creating empty patients or studies, we need to continue to populate the database

            qWarning() << "Unable to write image " + imageFileName;
            qWarning() << "Either there is nothing to write or a problem occured when writing.";
        }
        else
        {
            d->data->setMetaData ( "FileName", imageFileName );
        }
        
         QFileInfo   seriesInfo ( imageFileName );
         thumb_dir = seriesInfo.dir().path() + "/" + seriesInfo.completeBaseName() + "/";
    }
    

    // Now, populate the database
   medDataIndex index = this->populateDatabaseAndGenerateThumbnails (  d->data, thumb_dir );

    emit progressed(100);
    emit success();

    if (d->uuid == "")
        emit dataImported(index);
    else
        emit dataImported(index,d->uuid);
    
}

//-----------------------------------------------------------------------------------------------------------

void medAbstractDatabaseImporter::onCancel ( QObject* )
{
    d->isCancelled = true;
}

//-----------------------------------------------------------------------------------------------------------

/**
* Populates the missing metadata in the @medAbstractData object.
* If metadata is not present it's filled with default or empty values.
* @param medData - the object whose missing metadata will be filled
* @param seriesDescription - string used to fill SeriesDescription field if not present
**/
void medAbstractDatabaseImporter::populateMissingMetadata ( medAbstractData* medData, const QString seriesDescription )
{
    if ( !medData )
    {
        qWarning() << "data invalid";
        return;
    }


    QString newSeriesDescription;
    // check if image have basic information like patient, study, etc.
    // DICOMs, for instance, do provide it
    if ( !medData->hasMetaData ( medMetaDataKeys::PatientName.key() ) &&
         !medData->hasMetaData ( medMetaDataKeys::StudyDescription.key() ) &&
         !medData->hasMetaData ( medMetaDataKeys::SeriesDescription.key() ) )
    {
        // if none of these fields could be read from the file(s) then we won't be able to know for sure
        // if it was previously imported/indexed as it could happen that it is just another file with the same path
        // see http://pm-med.inria.fr/issues/292
        medData->addMetaData ( medMetaDataKeys::ContainsBasicInfo.key(), "false" );

        // it could be that we have already another image with this characteristics
        // so we would like to check whether the image filename is on the db
        // and if so we would add some suffix to distinguish it

        newSeriesDescription = ensureUniqueSeriesName(seriesDescription);
    }
    else
    {
        medData->addMetaData ( medMetaDataKeys::ContainsBasicInfo.key(), "true" );
        newSeriesDescription = seriesDescription;
    }

    if ( !medData->hasMetaData ( medMetaDataKeys::PatientName.key() ) )
        medData->addMetaData ( medMetaDataKeys::PatientName.key(), QStringList() << "John Doe" );

    if (!medData->hasMetaData ( medMetaDataKeys::PatientID.key() ) )
      medData->addMetaData ( medMetaDataKeys::PatientID.key(), QStringList() << "0" );

    if ( !medData->hasMetaData ( medMetaDataKeys::StudyDescription.key() ) )
        medData->addMetaData ( medMetaDataKeys::StudyDescription.key(), QStringList() << "EmptyStudy" );

    if ( !medData->hasMetaData ( medMetaDataKeys::SeriesDescription.key() ) )
        medData->addMetaData ( medMetaDataKeys::SeriesDescription.key(), QStringList() << newSeriesDescription );

    if ( !medData->hasMetaData ( medMetaDataKeys::StudyID.key() ) )
        medData->addMetaData ( medMetaDataKeys::StudyID.key(), QStringList() << "0" );

    if ( !medData->hasMetaData ( medMetaDataKeys::StudyDicomID.key() ) )
        medData->addMetaData ( medMetaDataKeys::StudyDicomID.key(), QStringList() << "" );

    QString generatedSeriesId = QUuid::createUuid().toString().replace("{","").replace("}","");

    if ( !medData->hasMetaData ( medMetaDataKeys::SeriesID.key() ) )
      medData->addMetaData ( medMetaDataKeys::SeriesID.key(), QStringList() << generatedSeriesId);

    QString generatedSeriesDicomID = QUuid::createUuid().toString().replace("{","").replace("}","");

    if ( !medData->hasMetaData ( medMetaDataKeys::SeriesDicomID.key() ) )
        medData->addMetaData ( medMetaDataKeys::SeriesDicomID.key(), QStringList() << generatedSeriesDicomID );

    if ( !medData->hasMetaData ( medMetaDataKeys::Orientation.key() ) )
        medData->addMetaData ( medMetaDataKeys::Orientation.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::SeriesNumber.key() ) )
        medData->addMetaData ( medMetaDataKeys::SeriesNumber.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::SequenceName.key() ) )
        medData->addMetaData ( medMetaDataKeys::SequenceName.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::SliceThickness.key() ) )
        medData->addMetaData ( medMetaDataKeys::SliceThickness.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::Rows.key() ) )
        medData->addMetaData ( medMetaDataKeys::Rows.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::Columns.key() ) )
        medData->addMetaData ( medMetaDataKeys::Columns.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::Age.key() ) )
        medData->addMetaData ( medMetaDataKeys::Age.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::BirthDate.key() ) )
        medData->addMetaData ( medMetaDataKeys::BirthDate.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::Gender.key() ) )
        medData->addMetaData ( medMetaDataKeys::Gender.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::Description.key() ) )
        medData->addMetaData ( medMetaDataKeys::Description.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::Modality.key() ) )
        medData->addMetaData ( medMetaDataKeys::Modality.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::Protocol.key() ) )
        medData->addMetaData ( medMetaDataKeys::Protocol.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::Comments.key() ) )
        medData->addMetaData ( medMetaDataKeys::Comments.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::Status.key() ) )
        medData->addMetaData ( medMetaDataKeys::Status.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::AcquisitionDate.key() ) )
        medData->addMetaData ( medMetaDataKeys::AcquisitionDate.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::ImportationDate.key() ) )
        medData->addMetaData ( medMetaDataKeys::ImportationDate.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::Referee.key() ) )
        medData->addMetaData ( medMetaDataKeys::Referee.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::Performer.key() ) )
        medData->addMetaData ( medMetaDataKeys::Performer.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::Institution.key() ) )
        medData->addMetaData ( medMetaDataKeys::Institution.key(), QStringList() << "" );

    if ( !medData->hasMetaData ( medMetaDataKeys::Report.key() ) )
        medData->addMetaData ( medMetaDataKeys::Report.key(), QStringList() << "" );
}

//-----------------------------------------------------------------------------------------------------------
/**
* Generates and saves the thumbnails for images in @dtkAbstractData.
* Also stores as metada with key RefThumbnailPath the path of the image that
* will be used as reference for patient, study and series.
* @param medData - @medAbstractData object whose thumbnails will be generated and saved
* @param pathToStoreThumbnails - path where the thumbnails will be stored
* @return a list of the thumbnails paths
**/
QStringList medAbstractDatabaseImporter::generateThumbnails ( medAbstractData* medData, QString pathToStoreThumbnails )
{
    QList<QImage> thumbnails = medData->thumbnails();

    QStringList thumbPaths;

    if ( !medStorage::mkpath ( medStorage::dataLocation() + pathToStoreThumbnails ) )
        qDebug() << "Cannot create directory: " << pathToStoreThumbnails;

    for ( int i=0; i < thumbnails.count(); i++ )
    {
        QString thumb_name = pathToStoreThumbnails + QString().setNum ( i ) + ".png";
        thumbnails[i].save ( medStorage::dataLocation() + thumb_name, "PNG" );
        thumbPaths << thumb_name;
    }

    QImage refThumbnail = medData->thumbnail(); // representative thumbnail for PATIENT/STUDY/SERIES
    QString refThumbPath = pathToStoreThumbnails + "ref.png";
    refThumbnail.save ( medStorage::dataLocation() + refThumbPath, "PNG" );

    medData->addMetaData ( medMetaDataKeys::ThumbnailPath.key(), refThumbPath );

    return thumbPaths;
}

//-----------------------------------------------------------------------------------------------------------
/**
* Tries to find a @dtkAbstractDataReader able to read input file/s.
* @param filename - Input file/s we would like to find a reader for
* @return a proper reader if found, NULL otherwise
**/
dtkSmartPointer<dtkAbstractDataReader> medAbstractDatabaseImporter::getSuitableReader ( QStringList filename )
{
    QList<QString> readers = medAbstractDataFactory::instance()->readers();

    if ( readers.size() ==0 )
    {
        emit showError (tr ( "No reader plugin" ), 5000 );
        emit failure ();
        return NULL;
    }

    dtkSmartPointer<dtkAbstractDataReader> dataReader;
    for (int i=0; i<readers.size(); i++) {
        dataReader = medAbstractDataFactory::instance()->readerSmartPointer(readers[i]);
        if (dataReader->canRead(filename)) {
            dataReader->enableDeferredDeletion(false);
            return dataReader;
        }
    }

    qWarning() << "No suitable reader found!";
    return NULL;
}

//-----------------------------------------------------------------------------------------------------------
/**
* Tries to find a @dtkAbstractDataWriter able to write input file/s.
* @param filename - name of the file we want to write
* @param medData - the @medAbstractData object we want to write
* @return a proper writer if found, NULL otherwise
**/
dtkSmartPointer<dtkAbstractDataWriter> medAbstractDatabaseImporter::getSuitableWriter(QString filename,medAbstractData* medData)
{
    if ( !medData )
        return NULL;

    QList<QString> writers = medAbstractDataFactory::instance()->writers();
    dtkSmartPointer<dtkAbstractDataWriter> dataWriter;
    for ( int i=0; i<writers.size(); i++ )
    {
        dataWriter = medAbstractDataFactory::instance()->writerSmartPointer ( writers[i] );
        dataWriter->setData(medData);

        if (dataWriter->handled().contains(medData->identifier()) &&
             dataWriter->canWrite( filename ) ) {
            dataWriter->enableDeferredDeletion(false);
            return dataWriter;
        }
    }
    return NULL;
}

//-----------------------------------------------------------------------------------------------------------
/**
* Walks through the whole directory tree and returns a list of every file found.
* @param fileOrDirectory - File or directory to search
* @return a list containing all files found
**/
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

//-----------------------------------------------------------------------------------------------------------
/**
* Tries to read the file/s indicated by filesPath.
* Only the header is read is specified by readOnlyImageInformation parameter.
* @param filesPath - path/s of the file/s we want to read
* @param readOnlyImageInformation - if true only image header is read, otherwise the full image
* @return a @medAbstractData containing the read data
**/
medAbstractData* medAbstractDatabaseImporter::tryReadImages ( const QStringList& filesPaths,const bool readOnlyImageInformation )
{
    medAbstractData *medData = NULL;

    dtkSmartPointer<dtkAbstractDataReader> dataReader;
    dataReader = getSuitableReader ( filesPaths );

    if ( dataReader )
    {
        bool readSuccessful = false;
        if ( readOnlyImageInformation )
            readSuccessful = dataReader->readInformation ( filesPaths );
        else
            readSuccessful = dataReader->read ( filesPaths );

        if (readSuccessful)
            medData = dynamic_cast<medAbstractData*>(dataReader->data());
    }
    else
    {
        // we take the first one for debugging just for simplicity
        qWarning() << "No suitable reader found for file: " << filesPaths[0] << ". Unable to import!";
    }

    return medData;
}

//-----------------------------------------------------------------------------------------------------------
/**
* Determines the filename where the medData object will be written (if importing).
* @param medData - the @medAbstractData that will be written
* @param volumeNumber - the volume number
* @return a string with the new filename
**/
QString medAbstractDatabaseImporter::determineFutureImageFileName ( const medAbstractData* medData, int volumeNumber )
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

    QString patientID = medMetaDataKeys::PatientID.getFirstValue(medData);
    QString seriesID   = medMetaDataKeys::SeriesID.getFirstValue(medData);

    QString imageFileName = QDir::separator() + QString ( patientID )
                          + QDir::separator() + QString ( seriesID ) + s_volumeNumber;

    d->volumeIdToImageFile[volumeNumber] = imageFileName;

    return imageFileName;
}

//-----------------------------------------------------------------------------------------------------------
/**
* Determines the extension (i.e. file format) which
* will be used for writing the medData object (if importing).
* @param medData - the @medAbstractData that will be written
* @return a string with the desired extension if found, and empty string otherwise
**/
QString medAbstractDatabaseImporter::determineFutureImageExtensionByDataType ( const medAbstractData* medData )
{
    QString identifier = medData->identifier();
    QString extension = "";

    QList<QString> writers = medAbstractDataFactory::instance()->writers();

    dtkSmartPointer<dtkAbstractDataWriter> dataWriter;

    // first let's try to retrieve extension for writer information
    for ( int i=0; i<writers.size(); i++ )
    {
        dataWriter = medAbstractDataFactory::instance()->writerSmartPointer ( writers[i] );

        if (dataWriter->handled().contains(medData->identifier()) )
        {
            QStringList extensions = dataWriter->supportedFileExtensions();
            if(!extensions.isEmpty())
            {
                extension = extensions[0];
                break;
            }
        }
    }

    // and if it fails, let's do it manually
    // but this could be avoided by updating writers implementation (supportedFileExtensions)
    if(extension.isEmpty())
    {
        // Determine the appropriate extension to use according to the type of data.
        // TODO: The image and CompositeDatasets types are weakly recognized (contains("Image/CompositeData")). to be improved
        if (identifier == "medVtkMeshData") {
            extension = ".vtk";
        } else if (identifier == "medVtk4DMeshData") {
            extension = ".v4d";
        } else if (identifier == "medVtkFibersData") {
            extension = ".xml";
        } else if (identifier.contains("vistal")) {
            extension = ".dim";
        } else if (identifier.contains ("CompositeData")) {
            extension = ".cds";
        } else if (identifier.contains ("Image")) {
            extension = ".mha";
        }
    }

    return extension;
}

//-----------------------------------------------------------------------------------------------------------
/**
* Tries writing the medData object in filePath.
* @param filePath - file path to use for writing
* @param medData - @medAbstractData object to be written
* @return true is writing was successful, false otherwise
**/
bool medAbstractDatabaseImporter::tryWriteImage ( QString filePath, medAbstractData* imData )
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

//-----------------------------------------------------------------------------------------------------------
/**
* Adds some additional metadata (e.g. Size, FilePaths
* and FileName) to the medData object.
* @param medData - a @medAbstractData object to add metadata to
* @param fileName - file name where the object will be written to
* @param filePaths - if the file is aggregating more than one file, all of them will be listed here
**/
void medAbstractDatabaseImporter::addAdditionalMetaData ( medAbstractData* imData, QString aggregatedFileName, QStringList aggregatedFilesPaths )
{
    QStringList size;
    if ( medAbstractImageData *imageData = dynamic_cast<medAbstractImageData*> ( imData ) )
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

//-----------------------------------------------------------------------------------------------------------
/**
* Generates an Id intended to be unique for each volume
* @param medData - @medAbstractData object whose id will be generate
* @return the volume id of the medData object
**/
QString medAbstractDatabaseImporter::generateUniqueVolumeId ( const medAbstractData* medData )
{
    if ( !medData )
    {
        qWarning() << "data invalid";
        return "invalid";
    }

    // Get all the information from the medAbstractData metadata.
    // This information will then be passed to the database.
    QString patientName = medMetaDataKeys::PatientName.getFirstValue(medData);
    QString studyDicomId = medMetaDataKeys::StudyDicomID.getFirstValue(medData);

    // We don't use the seriesDicomID, too unreliable : you can have images part
    // of the same series with different UIDs, and different volumes within the
    // same study with the same UIDs... instead, use Series Description
    QString seriesDescription = medMetaDataKeys::SeriesDescription.getFirstValue(medData);

    // orientation sometimes differ by a few digits, thus this is not reliable
    QString orientation = medMetaDataKeys::Orientation.getFirstValue(medData);

    QString sequenceName = medMetaDataKeys::SequenceName.getFirstValue(medData);
    QString sliceThickness = medMetaDataKeys::SliceThickness.getFirstValue(medData);
    QString rows = medMetaDataKeys::Rows.getFirstValue(medData);
    QString columns = medMetaDataKeys::Columns.getFirstValue(medData);

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
    QString key = patientName+studyDicomId+seriesDescription+orientation+sequenceName+sliceThickness+rows+columns;

    return key;
}
