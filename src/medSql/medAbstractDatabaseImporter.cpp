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
    dtkSmartPointer<dtkAbstractData> data;
    static QMutex mutex;
    QString lastSuccessfulReaderDescription;
    QString lastSuccessfulWriterDescription;
    bool isCancelled;
    bool indexWithoutImporting;
    medDataIndex index;

    // example of item in the list: ["patient", "study", "series"]
    QList<QStringList> partialAttemptsInfo;

    QMap<int, QString> volumeIdToImageFile;
    
    QString callerUuid;
};

QMutex medAbstractDatabaseImporterPrivate::mutex;

//-----------------------------------------------------------------------------------------------------------

medAbstractDatabaseImporter::medAbstractDatabaseImporter ( const QString& file, bool indexWithoutImporting, const QString& callerUuid ) : medJobItem(), d ( new medAbstractDatabaseImporterPrivate )
{
    d->isCancelled = false;
    d->lastSuccessfulReaderDescription = "";
    d->lastSuccessfulWriterDescription = "";
    d->file = file;
    d->data = NULL;
    d->indexWithoutImporting = indexWithoutImporting;
    d->callerUuid = callerUuid;
}

//-----------------------------------------------------------------------------------------------------------

medAbstractDatabaseImporter::medAbstractDatabaseImporter ( dtkAbstractData* dtkData, bool indexWithoutImporting, const QString& callerUuid ) : medJobItem(), d ( new medAbstractDatabaseImporterPrivate )
{
    d->isCancelled = false;
    d->lastSuccessfulReaderDescription = "";
    d->lastSuccessfulWriterDescription = "";
    d->data = dtkData;
    d->file = QString("");
    d->indexWithoutImporting = indexWithoutImporting;
    d->callerUuid = callerUuid;
}

//-----------------------------------------------------------------------------------------------------------

medAbstractDatabaseImporter::~medAbstractDatabaseImporter ( void )
{
    delete d;

    d = NULL;
}


QString medAbstractDatabaseImporter::file ( void )
{
    return d->file;
}

QString medAbstractDatabaseImporter::lastSuccessfulReaderDescription ( void )
{
    return d->lastSuccessfulReaderDescription;
}

QString medAbstractDatabaseImporter::lastSuccessfulWriterDescription ( void )
{
    return d->lastSuccessfulWriterDescription;
}

bool medAbstractDatabaseImporter::isCancelled ( void )   
{
    return d->isCancelled;
}

bool medAbstractDatabaseImporter::indexWithoutImporting ( void )   
{
    return d->indexWithoutImporting;
}

QList<QStringList>* medAbstractDatabaseImporter::partialAttemptsInfo ( void ) 
{
    return &(d->partialAttemptsInfo);
}

QMap<int, QString> medAbstractDatabaseImporter::volumeIdToImageFile ( void )  
{
    return d->volumeIdToImageFile;
}

medDataIndex medAbstractDatabaseImporter::index() const
{
    return d->index;
}

QString medAbstractDatabaseImporter::callerUuid()
{
    return d->callerUuid;
}
    

void medAbstractDatabaseImporter::run ( void )
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

    foreach ( QString file, fileList )
    {
        if ( d->isCancelled ) // check if user cancelled the process
            break;

        emit progress ( this, ( ( qreal ) currentFileNumber/ ( qreal ) fileList.count() ) * 50.0 ); //TODO: reading and filtering represents 50% of the importing process?

        currentFileNumber++;

        QFileInfo fileInfo ( file );

        dtkSmartPointer<dtkAbstractData> dtkData;

        // 2.1) Try reading file information, just the header not the whole file

        bool readOnlyImageInformation = true;
        dtkData = tryReadImages ( QStringList ( fileInfo.filePath() ), readOnlyImageInformation );

        if ( !dtkData )
        {
            qWarning() << "Reader was unable to read: " << fileInfo.filePath();
            continue;
        }

        // 2.2) Fill missing metadata
        populateMissingMetadata ( dtkData, fileInfo.baseName() );
        QString patientName = medMetaDataKeys::PatientName.getFirstValue(dtkData).simplified();
        QString birthDate = medMetaDataKeys::BirthDate.getFirstValue(dtkData);
        tmpPatientId = patientName + birthDate;

        if(tmpPatientId != currentPatientId)
        {
          currentPatientId = tmpPatientId;

          patientID = getPatientID(patientName, birthDate);
        }

        dtkData->setMetaData ( medMetaDataKeys::PatientID.key(), QStringList() << patientID );

        tmpSeriesUid = medMetaDataKeys::SeriesDicomID.getFirstValue(dtkData);

        if (tmpSeriesUid != currentSeriesUid)
        {
          currentSeriesUid = tmpSeriesUid;
          currentSeriesId = medMetaDataKeys::SeriesID.getFirstValue(dtkData);
        }
        else
          dtkData->setMetaData ( medMetaDataKeys::SeriesID.key(), QStringList() << currentSeriesId );

        // 2.3) Generate an unique id for each volume
        // all images of the same volume should share the same id
        QString volumeId = generateUniqueVolumeId ( dtkData );

        // check whether the image belongs to a new volume
        if ( !volumeUniqueIdToVolumeNumber.contains ( volumeId ) )
        {
            volumeUniqueIdToVolumeNumber[volumeId] = volumeNumber;
            volumeNumber++;
        }

        // 2.3) a) Determine future file name and path based on patient/study/series/image
        // i.e.: where we will write the imported image
        QString imageFileName = determineFutureImageFileName ( dtkData, volumeUniqueIdToVolumeNumber[volumeId] );
        #ifdef Q_OS_WIN32
                if ( (medStorage::dataLocation() + "/" + imageFileName).length() > 255 )
                {
                    emit showError ( tr ( "Your database path is too long" ), 5000 );
                emit failure ( this );
                return;
                }
            #endif
        // 2.3) b) Find the proper extension according to the type of the data
        // i.e.: in which format we will write the file in our database
        QString futureExtension  = determineFutureImageExtensionByDataType ( dtkData );

        // we care whether we can write the image or not if we are importing
        if (!d->indexWithoutImporting && futureExtension.isEmpty()) {
            emit showError(tr("Could not save file due to unhandled data type: ") + dtkData->identifier(), 5000);
            continue;
        }

        imageFileName = imageFileName + futureExtension;

        // 2.3) c) Add the image to a map for writing them all in medInria's database in a posterior step

        // First check if patient/study/series/image path already exists in the database
        // Should we emit a message otherwise ??? TO
        if ( !checkIfExists ( dtkData, fileInfo.fileName() ) )
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
        emit cancelled ( this );
        return;
    }

    // from now on the process cannot be cancelled
    emit disableCancel ( this );

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
        emit failure ( this );
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
        emit progress ( this, ( ( qreal ) currentImageIndex/ ( qreal ) imagesCount ) * 50.0 + 50.0 ); // 50? I do not think that reading all the headers is half the job...

        currentImageIndex++;

        QString aggregatedFileName = it.key(); // note that this file might be aggregating more than one input files
        QStringList filesPaths = it.value();   // input files being aggregated, might be only one or many
        patientID = itPat.value();
        QString seriesID = itSer.value();

        //qDebug() << currentImageIndex << ": " << aggregatedFileName << "with " << filesPaths.size() << " files";

        dtkSmartPointer<dtkAbstractData> imageDtkData;

        QFileInfo imagefileInfo ( filesPaths[0] );

        // 3.2) Try to read the whole image, not just the header
        bool readOnlyImageInformation = false;
        imageDtkData = tryReadImages ( filesPaths, readOnlyImageInformation );

        if ( imageDtkData )
        {
            // 3.3) a) re-populate missing metadata
            // as files might be aggregated we use the aggregated file name as SeriesDescription (if not provided, of course)
            populateMissingMetadata ( imageDtkData, imagefileInfo.baseName() );
            imageDtkData->setMetaData ( medMetaDataKeys::PatientID.key(), QStringList() << patientID );
            imageDtkData->setMetaData ( medMetaDataKeys::SeriesID.key(), QStringList() << seriesID );

            // 3.3) b) now we are able to add some more metadata
            addAdditionalMetaData ( imageDtkData, aggregatedFileName, filesPaths );
        }
        else
        {
            qWarning() << "Could not repopulate data!";
            emit showError (tr ( "Could not read data: " ) + filesPaths[0], 5000 );
            emit failure(this);
            return;
        }

        // check for partial import attempts
        if ( isPartialImportAttempt ( imageDtkData ) )
            continue;

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
            bool writeSuccess = tryWriteImage ( fileInfo.filePath(), imageDtkData );

            if ( !writeSuccess )
            {
                emit showError (tr ( "Could not save data file: " ) + filesPaths[0], 5000 );
                continue;
            }
        }

        // and finally we populate the database
        QFileInfo aggregatedFileNameFileInfo ( aggregatedFileName );
        QString pathToStoreThumbnails = aggregatedFileNameFileInfo.dir().path() + "/" + aggregatedFileNameFileInfo.completeBaseName() + "/";
        index = this->populateDatabaseAndGenerateThumbnails ( imageDtkData, pathToStoreThumbnails );
        
        if(!d->callerUuid.isEmpty())
        {
            emit addedIndex ( index, d->callerUuid );
        }
        else 
        {
            emit addedIndex ( index );
        }

        itPat++;
        itSer++;
    } // end of the final loop


    // if a partial import was attempted we tell the user what to do
    // to perform a correct import next time
    if ( d->partialAttemptsInfo.size() > 0 )
    {
        QString process = d->indexWithoutImporting ? "index" : "import";
        QString msg = "It seems you are trying to " + process + " some images that belong to a volume which is already in the database." + "\n";
        msg += "For a more accurate " + process + " please first delete the following series: " + "\n" + "\n";

        foreach ( QStringList info, d->partialAttemptsInfo )
        {
            msg += "Series: " + info[2] + " (from patient: " + info[0] + " and study: " + info[1] + "\n";
        }

        emit partialImportAttempted ( msg );
    }

    d->index = index;
    
    emit progress ( this,100 );
    emit success ( this );
}




void medAbstractDatabaseImporter::importData()
{   
    QMutexLocker locker ( &d->mutex );
     
    if ( !d->data )
    {
        emit failure ( this );
        return;
    }

    populateMissingMetadata(d->data, "EmptySerie");
    
    if ( !d->data->hasMetaData ( medMetaDataKeys::FilePaths.key() ) )
        d->data->addMetaData ( medMetaDataKeys::FilePaths.key(), QStringList() << "generated with medInria" );
        

    QString size ="";
    if ( medAbstractDataImage *imagedata = dynamic_cast<medAbstractDataImage*> ( d->data.data() ) )
        size = QString::number ( imagedata->zDimension() );
    d->data->addMetaData ( medMetaDataKeys::Size.key(), size );

    QString patientName = medMetaDataKeys::PatientName.getFirstValue(d->data).simplified();
    QString birthDate   = medMetaDataKeys::BirthDate.getFirstValue(d->data);
    QString seriesId    = medMetaDataKeys::SeriesID.getFirstValue(d->data);
        
    QString patientId  = getPatientID(patientName, birthDate);

    d->data->setMetaData ( medMetaDataKeys::PatientID.key(), QStringList() << patientId );
 

    // Check if PATIENT/STUDY/SERIES already exists in the database
    bool dataExists = isPartialImportAttempt(d->data);

    if ( dataExists )
    {
        qDebug() << "data is already in the database, skipping";
        emit failure ( this );
        return;
    }
     
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
                emit failure ( this );
                return ;
            }
        }

        QString extension  = determineFutureImageExtensionByDataType ( d->data );
        QString imageFileName = imageFileNameBase + extension;

        // writing file
        writeSuccess = tryWriteImage (  medStorage::dataLocation()+imageFileName, d->data );

        if ( !writeSuccess  )
        {
            //emit failure ( this );
            //return;

            // when creating empty patients or studies, we need to continue to populate the database

            qWarning() << "Unable to write image " + imageFileName;
            qWarning() << "Either there is nothing to write or a problem occured when writing.";
        }
        else
        {
            //d->data->addMetaData ( "FileName", imageFileName );
            d->data->setMetaData ( "FileName", imageFileName );
        }
        
         QFileInfo   seriesInfo ( imageFileName );
         thumb_dir = seriesInfo.dir().path() + "/" + seriesInfo.completeBaseName() + "/";
    }
    

    // Now, populate the database
   medDataIndex index = this->populateDatabaseAndGenerateThumbnails (  d->data, thumb_dir );

    emit progress(this, 100);
    emit success(this);

    if (d->callerUuid == "")
        emit addedIndex(index);
    else
        emit addedIndex(index,d->callerUuid);
    
}

//-----------------------------------------------------------------------------------------------------------

void medAbstractDatabaseImporter::onCancel ( QObject* )
{
    d->isCancelled = true;
}

//-----------------------------------------------------------------------------------------------------------

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
        emit showError (tr ( "No reader plugin" ), 5000 );
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

//-----------------------------------------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------------------------------------

QString medAbstractDatabaseImporter::determineFutureImageExtensionByDataType ( const dtkAbstractData* dtkdata )
{
    QString identifier = dtkdata->identifier();
    QString extension = "";

    QList<QString> writers = dtkAbstractDataFactory::instance()->writers();

    dtkSmartPointer<dtkAbstractDataWriter> dataWriter;

    // first let's try to retrieve extension for writer information
    for ( int i=0; i<writers.size(); i++ )
    {
        dataWriter = dtkAbstractDataFactory::instance()->writerSmartPointer ( writers[i] );

        if (dataWriter->handled().contains(dtkdata->identifier()) )
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
        if (identifier == "vtkDataMesh") {
            extension = ".vtk";
        } else if (identifier == "vtkDataMesh4D") {
            extension = ".v4d";
        } else if (identifier == "v3dDataFibers") {
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

//-----------------------------------------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------------------------------------

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
