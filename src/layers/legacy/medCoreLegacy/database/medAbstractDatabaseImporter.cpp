/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractDataReader.h>
#include <dtkCoreSupport/dtkAbstractDataWriter.h>

#include <medAbstractData.h>
#include <medAbstractDatabaseImporter.h>
#include <medAbstractDataFactory.h>
#include <medAbstractImageData.h>
#include <medGlobalDefs.h>
#include <medMetaDataKeys.h>
#include <medStorage.h>

class medAbstractDatabaseImporterPrivate
{
public:
    QString file;
    dtkSmartPointer<medAbstractData> data;
    static QMutex mutex;
    bool isCancelled;
    bool indexWithoutImporting;
    medDataIndex index;

    QMap<int, QString> volumeIdToImageFile;

    QUuid uuid;
};

QMutex medAbstractDatabaseImporterPrivate::mutex;

//-----------------------------------------------------------------------------------------------------------

medAbstractDatabaseImporter::medAbstractDatabaseImporter ( const QString& file, const QUuid& uuid, bool indexWithoutImporting) : medJobItemL(), d ( new medAbstractDatabaseImporterPrivate )
{
    d->isCancelled = false;
    d->file = file;
    d->data = nullptr;
    d->indexWithoutImporting = indexWithoutImporting;
    d->uuid = uuid;
}

//-----------------------------------------------------------------------------------------------------------

medAbstractDatabaseImporter::medAbstractDatabaseImporter ( medAbstractData* medData, const QUuid& uuid, bool indexWithoutImporting) : medJobItemL(), d ( new medAbstractDatabaseImporterPrivate )
{
    d->isCancelled = false;
    d->data = medData;
    d->file = QString("");
    d->indexWithoutImporting = indexWithoutImporting;
    d->uuid = uuid;
}

//-----------------------------------------------------------------------------------------------------------

medAbstractDatabaseImporter::~medAbstractDatabaseImporter ( void )
{
    delete d;

    d = nullptr;
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
    return d->uuid.toString();
}

/**
* Runs the import process based on the input file
* or directory given in the constructor
**/
void medAbstractDatabaseImporter::internalRun ( void )
{
    if(!QDir(medStorage::dataLocation()).exists())
    {
        emit showError ( tr ( "Your database path does not exist" ), 5000 );
        emit failure(this);
        return;
    }
    if(!d->file.isEmpty())
        importFile();
    else if ( d->data )
        importData();
}


void medAbstractDatabaseImporter::importFile ( void )
{
    QMutexLocker locker ( &d->mutex );

    /* [WARNING]
     * This algorithm has been very modified to fix bugs or unexpected behaviours
     * A big refactoring of this file is expected soon
     * Some comments in this method and in ImportData are deprecated
     */

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
    // new files ARE NOT written in the database yet, but are stored in a map for writing in a posterior step

    QString tmpSeriesUid;
    QString currentSeriesUid = "-1";
    QString currentSeriesId = "";

    bool atLeastOneImportSucceeded = false;
    bool atLeastOneImportError = false;

    for( QString file: fileList )
    {
        if ( d->isCancelled ) // check if user canceled the process
            break;

        emit progress ( this, ( ( qreal ) currentFileNumber/ ( qreal ) fileList.count() ) * 50.0 ); //TODO: reading and filtering represents 50% of the importing process?

        currentFileNumber++;

        QFileInfo fileInfo ( file );
        if (fileInfo.size() != 0)
        {
            dtkSmartPointer<medAbstractData> medData;

            // 2.1) Try reading file information, just the header not the whole file
            bool readOnlyImageInformation = true;
            medData = tryReadImages ( QStringList ( fileInfo.filePath() ), readOnlyImageInformation );

            if ( !medData )
            {
                if (!atLeastOneImportError)
                {
                    qWarning() << "Reader was unable to read at least: " << fileInfo.filePath();
                    atLeastOneImportError = true;
                }
                continue;
            }

            // 2.2) Fill missing metadata
            populateMissingMetadata(medData, med::smartBaseName(fileInfo.fileName()));
            QString tmpPatientID = medMetaDataKeys::PatientID.getFirstValue(medData).simplified();

            QString generatedSeriesId = QUuid::createUuid().toString().replace("{", "").replace("}", "");
            medData->setMetaData(medMetaDataKeys::SeriesID.key(), QStringList() << generatedSeriesId);

            tmpSeriesUid = medMetaDataKeys::SeriesInstanceUID.getFirstValue(medData);

            if (tmpSeriesUid != currentSeriesUid)
            {
                currentSeriesUid = tmpSeriesUid;
                currentSeriesId = medMetaDataKeys::SeriesID.getFirstValue(medData);
            }
            else
                medData->setMetaData ( medMetaDataKeys::SeriesID.key(), QStringList() << currentSeriesId );

            // 2.3) Generate an unique id for each volume
            // all images of the same volume should share the same id
            bool isdicom = isDicomName(fileInfo.fileName());
            QString volumeId = generateUniqueVolumeId ( medData, isdicom );

            // check whether the image belongs to a new volume
            if ( !volumeUniqueIdToVolumeNumber.contains ( volumeId ) )
            {
                volumeUniqueIdToVolumeNumber[volumeId] = volumeNumber;
                volumeNumber++;
            }

            // 2.3) b) Find the proper extension according to the type of the data
            // i.e.: in which format we will write the file in our database
            // [CHG] this code is here only to determine if the extension is ok. We must re-compute the extension again below...
            QString futureExtension  = determineFutureImageExtensionByDataType ( medData );

            // we care whether we can write the image or not if we are importing
            if (!d->indexWithoutImporting && futureExtension.isEmpty()) {
                emit showError(tr("Could not save file due to unhandled data type: ") + medData->identifier(), 5000);
                continue;
            }

            // 2.3) c) Add the image to a map for writing them all in the database in a posterior step
            imagesGroupedByVolume[volumeId] << fileInfo.filePath();
            imagesGroupedByPatient[volumeId] = tmpPatientID;
            imagesGroupedBySeriesId[volumeId] = currentSeriesId;
        }
        else
        {
            QString error = QString(tr("Could not read empty file: ") + fileInfo.completeBaseName());
            qWarning() << __FUNCTION__ << error;
            emit showError(error, 5000);
        }
    }

    // some checks to see if the user cancelled or something failed
    if (d->isCancelled)
    {
        emit showError(tr("User cancelled import process"), 5000);
        emit dataImported(medDataIndex(), d->uuid);
        emit cancelled(this);
        return;
    }

    // from now on the process cannot be cancelled
    emit disableCancel(this);

    // 3) Re-read selected files and re-populate them with missing metadata
    //    then write them to the database, and populate db tables
    QMap<QString, QStringList>::const_iterator it = imagesGroupedByVolume.begin();
    QMap<QString, QString>::const_iterator itPat = imagesGroupedByPatient.begin();
    QMap<QString, QString>::const_iterator itSer = imagesGroupedBySeriesId.begin();

    // 3.1) first check is after the filtering we have something to import
    // maybe we had problems with all the files, or they were already in the database
    if (it == imagesGroupedByVolume.end())
    {
        // TODO we know if it's either one or the other error, we can make this error better...
        emit showError(tr("No compatible image found or all of them had been already imported."), 5000);
        emit dataImported(medDataIndex(), d->uuid);
        emit failure(this);
        return;
    }
    else
        qDebug() << "Chosen directory contains " << imagesGroupedByVolume.size() << " files";
    setNumberOfFilesInDirectory(imagesGroupedByVolume.size());
    int imagesCount = imagesGroupedByVolume.count(); // used only to calculate progress
    int currentImageIndex = 0;                       // used only to calculate progress

    medDataIndex index; //stores the last volume's index to be emitted on success

    // final loop: re-read, re-populate and write to db
    for (; it != imagesGroupedByVolume.end(); it++)
    {
        emit progress(this, ((qreal)currentImageIndex / (qreal)imagesCount) * 50.0 + 50.0); // 50? I do not think that reading all the headers is half the job...

        currentImageIndex++;

        QString volumeIdKey = it.key();
        QStringList filesPaths = it.value();   // input files being aggregated, might be only one or many
        QString patientID = itPat.value();
        QString seriesID = itSer.value();

        dtkSmartPointer<medAbstractData> imagemedData;

        // 3.2) Try to read the whole image, not just the header
        bool readOnlyImageInformation = false;
        imagemedData = tryReadImages ( filesPaths, readOnlyImageInformation );

        if ( !imagemedData )
        {
            qWarning() << "Could not repopulate data!";
            emit showError (tr ( "Could not read data: " ) + filesPaths[0], 5000 );
            emit dataImported(medDataIndex(), d->uuid);
            emit failure(this);
            return;
        }
        // 3.3) a) re-populate missing metadata
        // as files might be aggregated we use the aggregated file name as SeriesDescription (if not provided, of course)
        QFileInfo imagefileInfo ( filesPaths[0] );
        populateMissingMetadata ( imagemedData, med::smartBaseName(imagefileInfo.fileName()) );
        /* [WARN - HACK] In tryReadImages, we may read seriesId, but this tag has no sense (in dicom purpose for instance)
         * This tag is used to compute the series path while writing data on disk and saving in db
         * Ensure the medAbstractData has the seriesId computed above
         * */
        imagemedData->setMetaData ( medMetaDataKeys::SeriesID.key(), QStringList() << seriesID );
        // Check if patientID was read in Header. If not, we have to set it again
        if (patientID.isEmpty())
        {
            patientID = medMetaDataKeys::PatientID.getFirstValue(imagemedData).simplified();
            // if even after reading the whole file, there is no patientID, we must:
            // 1/ check if there is already a patientID in db for this patient (patientName + birthdate)
            // 2/ create it if not
            if (patientID.isEmpty())
            {
                QString patientName = medMetaDataKeys::PatientName.getFirstValue(imagemedData).simplified();
                QString birthdate = medMetaDataKeys::BirthDate.getFirstValue(imagemedData).simplified();
                patientID = getPatientID(patientName, birthdate);
                imagemedData->setMetaData(medMetaDataKeys::PatientID.key(), QStringList() << patientID );
            }
        }

        QString aggregatedFileName = determineFutureImageFileName ( imagemedData, volumeUniqueIdToVolumeNumber[volumeIdKey] );
#ifdef Q_OS_WIN32
        if ( (medStorage::dataLocation() + "/" + aggregatedFileName).length() > 255 )
        {
            emit showError ( tr ( "Your database path is too long" ), 5000 );
            emit dataImported(medDataIndex(), d->uuid);
            emit failure ( this );
            return;
        }
#endif
        QString futureExtension  = determineFutureImageExtensionByDataType ( imagemedData );
        aggregatedFileName = aggregatedFileName + futureExtension;

        // 3.3) b) now we are able to add some more metadata
        addAdditionalMetaData ( imagemedData, aggregatedFileName, filesPaths );

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
        QString pathToStoreThumbnails = aggregatedFileNameFileInfo.dir().path() + QDir::separator() + aggregatedFileNameFileInfo.completeBaseName() + QDir::separator();
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
        emit progress ( this,100 );
        emit dataImported(medDataIndex(), d->uuid);
        emit failure(this);
        return;
    }

    d->index = index;

    emit progress(this, 100);
    emit success(this);
}

void medAbstractDatabaseImporter::importData()
{
    QMutexLocker locker(&d->mutex);

    if (!d->data)
    {
        emit failure(this);
        emit dataImported(medDataIndex(), d->uuid);
        return;
    }
    qDebug() << "import data " << d->data->data();
    // Update name of the series if a permanent data has this name already
    QString seriesDescription = d->data->metadata(medMetaDataKeys::SeriesDescription.key());
    QString studyInstanceUID = d->data->metadata(medMetaDataKeys::StudyInstanceUID.key());
    QString seriesInstanceUID = d->data->metadata(medMetaDataKeys::SeriesInstanceUID.key());

    if (!d->data->hasMetaData(medMetaDataKeys::FilePaths.key()))
    {
        d->data->addMetaData(medMetaDataKeys::FilePaths.key(), QStringList() << "data created internally");
    }

    // Information about the app and version of the application
    QString attachedInfoApp = QString("generated with " +
                                      QCoreApplication::applicationName() +
                                      " " +
                                      QString(MEDINRIA_VERSION));
    d->data->setMetaData(medMetaDataKeys::Description.key(), attachedInfoApp);

    QString size = "";
    if (medAbstractImageData *imagedata = dynamic_cast<medAbstractImageData *>(d->data.data()))
        size = QString::number(imagedata->zDimension());
    d->data->setMetaData(medMetaDataKeys::Size.key(), size);

    QString patientId = medMetaDataKeys::PatientID.getFirstValue(d->data);
    QString patientName = medMetaDataKeys::PatientName.getFirstValue(d->data).simplified();
    QString birthDate = medMetaDataKeys::BirthDate.getFirstValue(d->data);
    QString seriesId = medMetaDataKeys::SeriesID.getFirstValue(d->data);

    if (patientId.isEmpty())
    {
        QString patientId = getPatientID(patientName, birthDate);
        d->data->setMetaData(medMetaDataKeys::PatientID.key(), QStringList() << patientId);
    }
    bool writeSuccess = true;
    QString thumb_dir;

    if (!d->indexWithoutImporting)
    {
        QString subDirName = QDir::separator() + patientId;
        QString imageFileNameBase =  subDirName + QDir::separator() +  seriesId;

        QDir dir ( medStorage::dataLocation() + subDirName );
        if ( !dir.exists() )
        {
            if ( !medStorage::mkpath ( medStorage::dataLocation() + subDirName ) )
            {
                qWarning() << "Unable to create directory for images";
                emit failure ( this );
                emit dataImported(medDataIndex(), d->uuid);
                return ;
            }
        }

        QString extension  = determineFutureImageExtensionByDataType ( d->data );
        QString imageFileName = imageFileNameBase + extension;

        // writing file
        writeSuccess = tryWriteImage(medStorage::dataLocation() + imageFileName, d->data);
        qDebug() << "image File Name " << imageFileName;
        if (!writeSuccess)
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
         thumb_dir = seriesInfo.dir().path() + QDir::separator() + seriesInfo.completeBaseName() + QDir::separator();
    }

    // Now, populate the database
    medDataIndex index = this->populateDatabaseAndGenerateThumbnails (  d->data, thumb_dir );

    if (d->data->hasMetaData(medMetaDataKeys::Toolbox.key()) &&
    d->data->metadata(medMetaDataKeys::Toolbox.key())=="PolygonROI" &&
    d->data->hasMetaData(medMetaDataKeys::FileMetadataPath.key()))
    {
        this->createDBEntryForMetadataAttachedFile(d->data, index.seriesId());
    }
    emit progress(this, 100);
    emit success(this);

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
void medAbstractDatabaseImporter::populateMissingMetadata(medAbstractData *medData, const QString seriesDescription)
{
    if (!medData)
    {
        qWarning() << "data invalid";
        return;
    }

    QString generatedSeriesInstanceUID = QUuid::createUuid().toString().replace("{", "").replace("}", "");

    if (!medData->hasMetaData(medMetaDataKeys::SeriesInstanceUID.key()))
        medData->setMetaData(medMetaDataKeys::SeriesInstanceUID.key(), QStringList() << generatedSeriesInstanceUID);

    // check if image have basic information like patient, study, etc.
    // DICOMs, for instance, do provide it
    if (!medData->hasMetaData(medMetaDataKeys::PatientName.key()) &&
        !medData->hasMetaData(medMetaDataKeys::StudyDescription.key()) &&
        !medData->hasMetaData(medMetaDataKeys::SeriesDescription.key()))
    {
        // if none of these fields could be read from the file(s) then we won't be able to know for sure
        // if it was previously imported/indexed as it could happen that it is just another file with the same path
        // see http://pm-med.inria.fr/issues/292
        medData->setMetaData(medMetaDataKeys::ContainsBasicInfo.key(), "false");
    }
    else
    {
        medData->setMetaData(medMetaDataKeys::ContainsBasicInfo.key(), "true");
    }

    if (!medData->hasMetaData(medMetaDataKeys::PatientName.key()) ||
        medData->metadata(medMetaDataKeys::PatientName.key()).isEmpty())
        medData->setMetaData(medMetaDataKeys::PatientName.key(), QStringList() << "John Doe");

    if (!medData->hasMetaData(medMetaDataKeys::StudyDescription.key()) ||
        medData->metadata(medMetaDataKeys::StudyDescription.key()).isEmpty())
        medData->setMetaData(medMetaDataKeys::StudyDescription.key(), QStringList() << "EmptyStudy");

    if (!medData->hasMetaData(medMetaDataKeys::SeriesDescription.key()) ||
        medData->metadata(medMetaDataKeys::SeriesDescription.key()).isEmpty())
        medData->setMetaData(medMetaDataKeys::SeriesDescription.key(), QStringList() << seriesDescription);

    if (!medData->hasMetaData(medMetaDataKeys::StudyID.key()))
        medData->setMetaData(medMetaDataKeys::StudyID.key(), QStringList() << "0");
    
    if (!medData->hasMetaData(medMetaDataKeys::Orientation.key()))
        medData->setMetaData(medMetaDataKeys::Orientation.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::SeriesNumber.key()))
        medData->setMetaData(medMetaDataKeys::SeriesNumber.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::SequenceName.key()))
        medData->setMetaData(medMetaDataKeys::SequenceName.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::SliceThickness.key()))
        medData->setMetaData(medMetaDataKeys::SliceThickness.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::Rows.key()))
        medData->setMetaData(medMetaDataKeys::Rows.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::Columns.key()))
        medData->setMetaData(medMetaDataKeys::Columns.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::Age.key()))
        medData->setMetaData(medMetaDataKeys::Age.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::BirthDate.key()))
        medData->setMetaData(medMetaDataKeys::BirthDate.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::Gender.key()))
        medData->setMetaData(medMetaDataKeys::Gender.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::Description.key()))
        medData->setMetaData(medMetaDataKeys::Description.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::Modality.key()))
        medData->setMetaData(medMetaDataKeys::Modality.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::Protocol.key()))
        medData->setMetaData(medMetaDataKeys::Protocol.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::Comments.key()))
        medData->setMetaData(medMetaDataKeys::Comments.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::Status.key()))
        medData->setMetaData(medMetaDataKeys::Status.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::AcquisitionDate.key()))
        medData->setMetaData(medMetaDataKeys::AcquisitionDate.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::ImportationDate.key()))
        medData->setMetaData(medMetaDataKeys::ImportationDate.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::Referee.key()))
        medData->setMetaData(medMetaDataKeys::Referee.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::Performer.key()))
        medData->setMetaData(medMetaDataKeys::Performer.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::Institution.key()))
        medData->setMetaData(medMetaDataKeys::Institution.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::Report.key()))
        medData->setMetaData(medMetaDataKeys::Report.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::Origin.key()))
        medData->setMetaData(medMetaDataKeys::Origin.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::FlipAngle.key()))
        medData->setMetaData(medMetaDataKeys::FlipAngle.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::EchoTime.key()))
        medData->setMetaData(medMetaDataKeys::EchoTime.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::RepetitionTime.key()))
        medData->setMetaData(medMetaDataKeys::RepetitionTime.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::AcquisitionTime.key()))
        medData->setMetaData(medMetaDataKeys::AcquisitionTime.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::StudyTime.key()))
        medData->setMetaData(medMetaDataKeys::StudyTime.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::StudyDate.key()))
        medData->setMetaData(medMetaDataKeys::StudyDate.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::SeriesTime.key()))
        medData->setMetaData(medMetaDataKeys::SeriesTime.key(), QStringList() << "");

    if (!medData->hasMetaData(medMetaDataKeys::SeriesDate.key()))
        medData->setMetaData(medMetaDataKeys::SeriesDate.key(), QStringList() << "");

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
QString medAbstractDatabaseImporter::generateThumbnail ( medAbstractData* medData, QString pathToStoreThumbnail )
{
    QImage thumbnail = medData->generateThumbnail(med::defaultThumbnailSize);
    QString thumbnailPath = pathToStoreThumbnail + "thumbnail.png";
    QString fullThumbnailPath = medStorage::dataLocation() + thumbnailPath;

    if ( ! medStorage::mkpath ( medStorage::dataLocation() + pathToStoreThumbnail ) )
    {
        qWarning("medAbstractDatabaseImporter: Cannot create directory: %s", qPrintable(pathToStoreThumbnail));
    }

    thumbnail.save(fullThumbnailPath, "PNG");

    medData->addMetaData ( medMetaDataKeys::ThumbnailPath.key(), thumbnailPath );

    return thumbnailPath;
}

//-----------------------------------------------------------------------------------------------------------
/**
* Tries to find a @dtkAbstractDataReader able to read input file/s.
* @param filename - Input file/s we would like to find a reader for
* @return a proper reader if found, nullptr otherwise
**/
dtkSmartPointer<dtkAbstractDataReader> medAbstractDatabaseImporter::getSuitableReader ( QStringList filename )
{
    QList<QString> readers = medAbstractDataFactory::instance()->readers();

    if ( readers.size() ==0 )
    {
        emit showError (tr ( "No reader plugin" ), 5000 );
        emit failure ( this );
        return nullptr;
    }

    dtkSmartPointer<dtkAbstractDataReader> dataReader;
    for (int i=0; i<readers.size(); i++)
    {
        dataReader = medAbstractDataFactory::instance()->readerSmartPointer(readers[i]);
        dataReader->enableDeferredDeletion(false);

        if (dataReader->canRead(filename))
        {
            return dataReader;
        }
    }

    return nullptr;
}

//-----------------------------------------------------------------------------------------------------------
/**
* Tries to find a @dtkAbstractDataWriter able to write input file/s.
* @param filename - name of the file we want to write
* @param medData - the @medAbstractData object we want to write
* @return a proper writer if found, nullptr otherwise
**/
dtkSmartPointer<dtkAbstractDataWriter> medAbstractDatabaseImporter::getSuitableWriter(QString filename,medAbstractData* medData)
{
    if ( !medData )
        return nullptr;

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
    return nullptr;
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
    medAbstractData *medData = nullptr;

    dtkSmartPointer<dtkAbstractDataReader> dataReader;
    dataReader = getSuitableReader ( filesPaths );

    if ( dataReader )
    {
        bool readSuccessful = false;
        if ( readOnlyImageInformation )
        {
            readSuccessful = dataReader->readInformation ( filesPaths );
        }
        else
        {
            readSuccessful = dataReader->read ( filesPaths );
        }

        if (readSuccessful)
        {
            medData = dynamic_cast<medAbstractData*>(dataReader->data());
        }
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

    // first let's try to retrieve extension for writer information
    for ( int i=0; i<writers.size(); i++ )
    {
        auto dataWriter = medAbstractDataFactory::instance()->writerSmartPointer ( writers[i] );

        if (dataWriter && dataWriter->handled().contains(medData->identifier()) )
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
void medAbstractDatabaseImporter::addAdditionalMetaData(medAbstractData *imData, QString aggregatedFileName, QStringList aggregatedFilesPaths)
{
    QStringList size;
    if (medAbstractImageData *imageData = dynamic_cast<medAbstractImageData *>(imData))
    {
        size << QString::number(imageData->zDimension());
    }
    else
    {
        size << "";
    }

    imData->setMetaData("Size", size);
    qDebug() << "aggregated " << aggregatedFileName;
    if (!imData->hasMetaData("FilePaths"))
        imData->setMetaData("FilePaths", aggregatedFilesPaths);

    imData->setMetaData("FileName", aggregatedFileName);
}

//-----------------------------------------------------------------------------------------------------------
/**
* Generates an Id intended to be unique for each volume
* @param medData - @medAbstractData object whose id will be generate
* @return the volume id of the medData object
**/
QString medAbstractDatabaseImporter::generateUniqueVolumeId ( const medAbstractData* medData, bool isDicom )
{
    if ( !medData )
    {
        qWarning() << "data invalid";
        return "invalid";
    }

    // Get all the information from the medAbstractData metadata.
    // This information will then be passed to the database.
    QString patientName = medMetaDataKeys::PatientName.getFirstValue(medData);
    QString studyDicomId = medMetaDataKeys::StudyInstanceUID.getFirstValue(medData);

    // We don't use the seriesDicomID, too unreliable : you can have images part
    // of the same series with different UIDs, and different volumes within the
    // same study with the same UIDs... instead, use SeriesNumber for dicom
    QString seriesNumber = medMetaDataKeys::SeriesNumber.getFirstValue(medData);
    // seriesDescription is used for the other type of data
    QString seriesDescription = medMetaDataKeys::SeriesDescription.getFirstValue(medData);

    // orientation sometimes differ by a few digits, thus this is not reliable
    QString orientation = medMetaDataKeys::Orientation.getFirstValue(medData);

    QString sequenceName = medMetaDataKeys::SequenceName.getFirstValue(medData);
    QString sliceThickness = medMetaDataKeys::SliceThickness.getFirstValue(medData);
    QString rows = medMetaDataKeys::Rows.getFirstValue(medData);
    QString columns = medMetaDataKeys::Columns.getFirstValue(medData);
    QString echotime = medMetaDataKeys::EchoTime.getFirstValue(medData);
    QString flipAngle = medMetaDataKeys::FlipAngle.getFirstValue(medData);

    QStringList orientations = orientation.split ( " " );

    orientation = "";

    // truncate orientation to 4 digits for a more robust import since
    // sometimes orientation only differs with the last digits, creating
    // multiple series
    for( QString orient : orientations )
    {
        double d_orient = orient.toDouble();
        orientation += QString::number ( d_orient, 'f', 4 );
    }

    // define a unique key string to identify which volume an image belongs to.
    // we use: patientName, studyId, seriesId, orientation, seriesNumber, sequenceName, sliceThickness, rows, columns,
    // echoTime and flipAngle.
    // All images of the same volume should share similar values of these parameters
    QString key;
    if(isDicom)
    {
        key= patientName+studyDicomId+seriesNumber+orientation+sequenceName+sliceThickness+rows+columns
            +echotime+flipAngle;
    }
    else
    {
        key= patientName+studyDicomId+seriesDescription+orientation+sequenceName+sliceThickness+rows+columns;
    }

    return key;
}

//-----------------------------------------------------------------------------------------------------------
/**
* Look for dicom extension in file name
* @param fileName - name of the file that we try to import
* @return  true if dicom file, false otherwise
**/
bool medAbstractDatabaseImporter::isDicomName(const QString & fileName)
{
    QFileInfo info(fileName);
    return info.suffix().toLower() == "dcm";
}
