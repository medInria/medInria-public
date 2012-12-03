/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseImporter.h"

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

class medDatabaseImporterPrivate
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

QMutex medDatabaseImporterPrivate::mutex;

//-----------------------------------------------------------------------------------------------------------

medDatabaseImporter::medDatabaseImporter ( const QString& file, bool indexWithoutImporting = false ) : medJobItem(), d ( new medDatabaseImporterPrivate )
{
    d->isCancelled = false;
    d->lastSuccessfulReaderDescription = "";
    d->lastSuccessfulWriterDescription = "";
    d->file = file;
    d->indexWithoutImporting = indexWithoutImporting;
}

//-----------------------------------------------------------------------------------------------------------

medDatabaseImporter::~medDatabaseImporter()
{
    delete d;

    d = NULL;
}

//-----------------------------------------------------------------------------------------------------------

void medDatabaseImporter::run()
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

    d->volumeIdToImageFile.clear();

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

          //Let's see if the patient is already in the db
          QSqlDatabase db = * ( medDatabaseController::instance()->database() );
          QSqlQuery query ( db );

          query.prepare ( "SELECT patientId FROM patient WHERE name = :name AND birthdate = :birthdate" );
          query.bindValue ( ":name", patientName );
          query.bindValue ( ":birthdate", birthDate );

          if ( !query.exec() )
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

          if ( query.first() )
            patientID = query.value ( 0 ).toString();
          else
            patientID = QUuid::createUuid().toString().replace("{","").replace("}","");
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
	            emit showError (tr ( "Your database path is too long" ), 5000 );
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
        emit showError ( tr ( "User cancelled import process" ), 5000 );
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
        emit showError ( tr ( "No compatible image found or all of them had been already imported." ), 5000 );
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
            emit showError ( tr ( "Could not read data: " ) + filesPaths[0], 5000 );
            continue;
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

    emit progress ( this,100 );
    emit success ( this );
    emit addedIndex ( index );
}

//-----------------------------------------------------------------------------------------------------------

void medDatabaseImporter::onCancel ( QObject* )
{
    d->isCancelled = true;
}

bool medDatabaseImporter::isPartialImportAttempt ( dtkAbstractData* dtkData )
{
    // here we check if the series we try to import is already in the database

    // if the image didn't provide basic information like patient, study or series
    // we cannot tell whether we are importing the same file twice or
    // if we want to import now a file with a path that was previously imported before
    // see http://pm-med.inria.fr/issues/292 for more details
    QString containsBasicInfo = medMetaDataKeys::ContainsBasicInfo.getFirstValue(dtkData);
    if (containsBasicInfo.compare("true", Qt::CaseInsensitive) != 0)
        return false;

    QSqlDatabase db = * ( medDatabaseController::instance()->database() );
    QSqlQuery query ( db );

    QString patientName = medMetaDataKeys::PatientName.getFirstValue(dtkData).simplified();

    query.prepare ( "SELECT id FROM patient WHERE name = :name" );
    query.bindValue ( ":name", patientName );

    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if ( query.first() )
    {
        int patientDbId = query.value ( 0 ).toInt();

        query.clear();

        QString studyName = medMetaDataKeys::StudyDescription.getFirstValue(dtkData).simplified();
        QString studyUid = medMetaDataKeys::StudyDicomID.getFirstValue(dtkData);

        query.prepare ( "SELECT id FROM study WHERE patient = :patient AND name = :studyName AND uid = :studyUid" );
        query.bindValue ( ":patient", patientDbId );
        query.bindValue ( ":studyName", studyName );
        query.bindValue ( ":studyUid", studyUid );

        if ( !query.exec() )
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

        if ( query.first() )
        {
            int studyDbId = query.value ( 0 ).toInt();

            query.clear();

            QString seriesName = medMetaDataKeys::SeriesDescription.getFirstValue(dtkData).simplified();
            QString seriesUid = medMetaDataKeys::SeriesDicomID.getFirstValue(dtkData);
            QString orientation = medMetaDataKeys::Orientation.getFirstValue(dtkData);
            QString seriesNumber = medMetaDataKeys::SeriesNumber.getFirstValue(dtkData);
            QString sequenceName = medMetaDataKeys::SequenceName.getFirstValue(dtkData);
            QString sliceThickness = medMetaDataKeys::SliceThickness.getFirstValue(dtkData);
            QString rows = medMetaDataKeys::Rows.getFirstValue(dtkData);
            QString columns = medMetaDataKeys::Columns.getFirstValue(dtkData);

            query.prepare ( "SELECT * FROM series WHERE study = :study AND name = :seriesName AND uid = :seriesUid AND orientation = :orientation AND seriesNumber = :seriesNumber AND sequenceName = :sequenceName AND sliceThickness = :sliceThickness AND rows = :rows AND columns = :columns" );
            query.bindValue ( ":study", studyDbId );
            query.bindValue ( ":seriesName", seriesName );
            query.bindValue ( ":seriesUid", seriesUid );
            query.bindValue ( ":orientation", orientation );
            query.bindValue ( ":seriesNumber", seriesNumber );
            query.bindValue ( ":sequenceName", sequenceName );
            query.bindValue ( ":sliceThickness", sliceThickness );
            query.bindValue ( ":rows", rows );
            query.bindValue ( ":columns", columns );

            if ( !query.exec() )
                qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

            if ( query.first() )
            {
                QStringList filePaths = dtkData->metaDataValues ( medMetaDataKeys::FilePaths.key() );
                d->partialAttemptsInfo << ( QStringList() << patientName << studyName << seriesName << filePaths[0] );
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------------------------------------

void medDatabaseImporter::populateMissingMetadata ( dtkAbstractData* dtkData, const QString seriesDescription )
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

bool medDatabaseImporter::checkIfExists ( dtkAbstractData* dtkdata, QString imageName )
{
    bool imageExists = false;

    QSqlQuery query ( * ( medDatabaseController::instance()->database() ) );

    // first we query patient table
    QString patientName = medMetaDataKeys::PatientName.getFirstValue(dtkdata);
    QString birthDate = medMetaDataKeys::BirthDate.getFirstValue(dtkdata);

    query.prepare ( "SELECT id FROM patient WHERE name = :name AND birthDate = :birthdate");
    query.bindValue ( ":name", patientName );
    query.bindValue ( ":birthdate", birthDate );

    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if ( query.first() )
    {
        QVariant patientDbId = query.value ( 0 );
        // if patient already exists we now verify the study

        QString studyName = medMetaDataKeys::StudyDescription.getFirstValue(dtkdata);
        QString studyUid  = medMetaDataKeys::StudyDicomID.getFirstValue(dtkdata);

        query.prepare ( "SELECT id FROM study WHERE patient = :patient AND name = :name AND uid = :studyID" );
        query.bindValue ( ":patient", patientDbId );
        query.bindValue ( ":name", studyName );
        query.bindValue ( ":studyUid", studyUid );

        if ( !query.exec() )
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

        if ( query.first() )
        {
            QVariant studyDbId = query.value ( 0 );
            // both patient and study exists, let's check series
            QString seriesName  = medMetaDataKeys::SeriesDescription.getFirstValue(dtkdata);
            QString seriesUid = medMetaDataKeys::SeriesDicomID.getFirstValue(dtkdata);
            QString orientation = medMetaDataKeys::Orientation.getFirstValue(dtkdata); // orientation sometimes differ by a few digits, thus this is not reliable
            QString seriesNumber = medMetaDataKeys::SeriesNumber.getFirstValue(dtkdata);
            QString sequenceName = medMetaDataKeys::SequenceName.getFirstValue(dtkdata);
            QString sliceThickness = medMetaDataKeys::SliceThickness.getFirstValue(dtkdata);
            QString rows = medMetaDataKeys::Rows.getFirstValue(dtkdata);
            QString columns = medMetaDataKeys::Columns.getFirstValue(dtkdata);

            query.prepare ( "SELECT id FROM series WHERE study = :study AND name = :name AND uid = :seriesUid AND orientation = :orientation AND seriesNumber = :seriesNumber AND sequenceName = :sequenceName AND sliceThickness = :sliceThickness AND rows = :rows AND columns = :columns" );
            query.bindValue ( ":study", studyDbId );
            query.bindValue ( ":name", seriesName );
            query.bindValue ( ":seriesUid", seriesUid );
            query.bindValue ( ":orientation", orientation );
            query.bindValue ( ":seriesNumber", seriesNumber );
            query.bindValue ( ":sequenceName", sequenceName );
            query.bindValue ( ":sliceThickness", sliceThickness );
            query.bindValue ( ":rows", rows );
            query.bindValue ( ":columns", columns );

            if ( !query.exec() )
                qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

            if ( query.first() )
            {
                QVariant seriesDbId = query.value ( 0 );

                // finally we check the image table

                query.prepare ( "SELECT id FROM image WHERE series = :series AND name = :name" );
                query.bindValue ( ":series", seriesDbId );
                query.bindValue ( ":name", imageName );

                if ( !query.exec() )
                    qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

                if ( query.first() )
                {
                    imageExists = true;
                }
            }
        }
    }
    return imageExists;
}

//-----------------------------------------------------------------------------------------------------------

medDataIndex medDatabaseImporter::populateDatabaseAndGenerateThumbnails ( dtkAbstractData* dtkData, QString pathToStoreThumbnails )
{
    QSqlDatabase db = * ( medDatabaseController::instance()->database() );

    QStringList thumbPaths = generateThumbnails ( dtkData, pathToStoreThumbnails );

    int patientDbId = getOrCreatePatient ( dtkData, db );

    int studyDbId = getOrCreateStudy ( dtkData, db, patientDbId );

    int seriesDbId = getOrCreateSeries ( dtkData, db, studyDbId );

    createMissingImages ( dtkData, db, seriesDbId, thumbPaths );

    medDataIndex index = medDataIndex ( medDatabaseController::instance()->dataSourceId(), patientDbId, studyDbId, seriesDbId, -1 );
    return index;
}

//-----------------------------------------------------------------------------------------------------------

QStringList medDatabaseImporter::generateThumbnails ( dtkAbstractData* dtkData, QString pathToStoreThumbnails )
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

int medDatabaseImporter::getOrCreatePatient ( const dtkAbstractData* dtkData, QSqlDatabase db )
{
    int patientDbId = -1;

    QSqlQuery query ( db );

    QString patientName = medMetaDataKeys::PatientName.getFirstValue(dtkData).simplified();
    QString birthDate = medMetaDataKeys::BirthDate.getFirstValue(dtkData);
    QString patientId = medMetaDataKeys::PatientID.getFirstValue(dtkData);

    query.prepare ( "SELECT id FROM patient WHERE name = :name AND birthdate = :birthdate" );
    query.bindValue ( ":name", patientName );
    query.bindValue ( ":birthdate", birthDate );

    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if ( query.first() )
    {
        patientDbId = query.value ( 0 ).toInt();
    }
    else
    {
        QString refThumbPath   = medMetaDataKeys::ThumbnailPath.getFirstValue(dtkData);
        QString birthdate      = medMetaDataKeys::BirthDate.getFirstValue(dtkData);
        QString gender         = medMetaDataKeys::Gender.getFirstValue(dtkData);

        query.prepare ( "INSERT INTO patient (name, thumbnail, birthdate, gender, patientId) VALUES (:name, :thumbnail, :birthdate, :gender, :patientId)" );
        query.bindValue ( ":name", patientName );
        query.bindValue ( ":thumbnail", refThumbPath );
        query.bindValue ( ":birthdate", birthdate );
        query.bindValue ( ":gender",    gender );
        query.bindValue ( ":patientId", patientId);
        query.exec();

        patientDbId = query.lastInsertId().toInt();
    }

    return patientDbId;
}

int medDatabaseImporter::getOrCreateStudy ( const dtkAbstractData* dtkData, QSqlDatabase db, int patientDbId )
{
    int studyDbId = -1;

    QSqlQuery query ( db );

    QString studyName   = medMetaDataKeys::StudyDescription.getFirstValue(dtkData).simplified();
    QString studyUid    = medMetaDataKeys::StudyDicomID.getFirstValue(dtkData);
    QString studyId    = medMetaDataKeys::StudyID.getFirstValue(dtkData);

    query.prepare ( "SELECT id FROM study WHERE patient = :patient AND name = :studyName AND uid = :studyUid" );
    query.bindValue ( ":patient", patientDbId );
    query.bindValue ( ":studyName", studyName );
    query.bindValue ( ":studyUid", studyUid );

    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if ( query.first() )
    {
        studyDbId = query.value ( 0 ).toInt();
    }
    else
    {
        QString refThumbPath = medMetaDataKeys::ThumbnailPath.getFirstValue(dtkData);

        query.prepare ( "INSERT INTO study (patient, name, uid, thumbnail, studyId) VALUES (:patient, :studyName, :studyUid, :thumbnail, :studyId)" );
        query.bindValue ( ":patient", patientDbId );
        query.bindValue ( ":studyName", studyName );
        query.bindValue ( ":studyUid", studyUid );
        query.bindValue ( ":thumbnail", refThumbPath );
        query.bindValue ( ":studyId", studyId);

        query.exec();

        studyDbId = query.lastInsertId().toInt();
    }

    return studyDbId;
}

int medDatabaseImporter::getOrCreateSeries ( const dtkAbstractData* dtkData, QSqlDatabase db, int studyDbId )
{
    int seriesDbId = -1;

    QSqlQuery query ( db );

    QString seriesName     = medMetaDataKeys::SeriesDescription.getFirstValue(dtkData).simplified();
    QString seriesUid      = medMetaDataKeys::SeriesDicomID.getFirstValue(dtkData);
    QString seriesId       = medMetaDataKeys::SeriesID.getFirstValue(dtkData);

    QString orientation    = medMetaDataKeys::Orientation.getFirstValue(dtkData);
    QString seriesNumber   = medMetaDataKeys::SeriesNumber.getFirstValue(dtkData);
    QString sequenceName   = medMetaDataKeys::SequenceName.getFirstValue(dtkData);
    QString sliceThickness = medMetaDataKeys::SliceThickness.getFirstValue(dtkData);
    QString rows           = medMetaDataKeys::Rows.getFirstValue(dtkData);
    QString columns        = medMetaDataKeys::Columns.getFirstValue(dtkData);

    query.prepare ( "SELECT * FROM series WHERE study = :study AND name = :seriesName AND uid = :seriesUid AND orientation = :orientation AND seriesNumber = :seriesNumber AND sequenceName = :sequenceName AND sliceThickness = :sliceThickness AND rows = :rows AND columns = :columns" );
    query.bindValue ( ":study", studyDbId );
    query.bindValue ( ":seriesName", seriesName );
    query.bindValue ( ":seriesUid", seriesUid );
    query.bindValue ( ":orientation", orientation );
    query.bindValue ( ":seriesNumber", seriesNumber );
    query.bindValue ( ":sequenceName", sequenceName );
    query.bindValue ( ":sliceThickness", sliceThickness );
    query.bindValue ( ":rows", rows );
    query.bindValue ( ":columns", columns );

    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if ( query.first() )
    {
        seriesDbId = query.value ( 0 ).toInt();
    }
    else
    {
        // if we are creating a new series while indexing then we need to empty
        // the column 'path', as there won't be a file aggregating the images

        QString seriesPath = "";
        if ( !d->indexWithoutImporting )
            //seriesPath = dtkData->metaDataValues (medMetaDataKeys::FilePaths.key())[0];
            seriesPath = dtkData->metaDataValues ( "FileName" ) [0];
        int size               = medMetaDataKeys::Size.getFirstValue(dtkData).toInt();
        QString refThumbPath   = medMetaDataKeys::ThumbnailPath.getFirstValue(dtkData);
        QString age            = medMetaDataKeys::Age.getFirstValue(dtkData);
        QString description    = medMetaDataKeys::Description.getFirstValue(dtkData);
        QString modality       = medMetaDataKeys::Modality.getFirstValue(dtkData);
        QString protocol       = medMetaDataKeys::Protocol.getFirstValue(dtkData);
        QString comments       = medMetaDataKeys::Comments.getFirstValue(dtkData);
        QString status         = medMetaDataKeys::Status.getFirstValue(dtkData);
        QString acqdate        = medMetaDataKeys::AcquisitionDate.getFirstValue(dtkData);
        QString importdate     = medMetaDataKeys::ImportationDate.getFirstValue(dtkData);
        QString referee        = medMetaDataKeys::Referee.getFirstValue(dtkData);
        QString performer      = medMetaDataKeys::Performer.getFirstValue(dtkData);
        QString institution    = medMetaDataKeys::Institution.getFirstValue(dtkData);
        QString report         = medMetaDataKeys::Report.getFirstValue(dtkData);

        query.prepare ( "INSERT INTO series (study, seriesId, size, name, path, uid, orientation, seriesNumber, sequenceName, sliceThickness, rows, columns, thumbnail, age, description, modality, protocol, comments, status, acquisitiondate, importationdate, referee, performer, institution, report) \
                                     VALUES (:study, :seriesId, :size, :seriesName, :seriesPath, :seriesUid, :orientation, :seriesNumber, :sequenceName, :sliceThickness, :rows, :columns, :refThumbPath, :age, :description, :modality, :protocol, :comments, :status, :acquisitiondate, :importationdate, :referee, :performer, :institution, :report)" );
        query.bindValue ( ":study",          studyDbId );
        query.bindValue ( ":seriesId",       seriesId );
        query.bindValue ( ":size",           size );
        query.bindValue ( ":seriesName",     seriesName );
        query.bindValue ( ":seriesPath",     seriesPath );
        query.bindValue ( ":seriesUid",      seriesUid );
        query.bindValue ( ":orientation",    orientation );
        query.bindValue ( ":seriesNumber",   seriesNumber );
        query.bindValue ( ":sequenceName",   sequenceName );
        query.bindValue ( ":sliceThickness", sliceThickness );
        query.bindValue ( ":rows",           rows );
        query.bindValue ( ":columns",        columns );
        query.bindValue ( ":thumbnail",      refThumbPath );
        query.bindValue ( ":age",            age );
        query.bindValue ( ":description",    description );
        query.bindValue ( ":modality",       modality );
        query.bindValue ( ":protocol",       protocol );
        query.bindValue ( ":comments",       comments );
        query.bindValue ( ":status",         status );
        query.bindValue ( ":acquisitiondate",acqdate );
        query.bindValue ( ":importationdate",importdate );
        query.bindValue ( ":referee",        referee );
        query.bindValue ( ":performer",      performer );
        query.bindValue ( ":institution",    institution );
        query.bindValue ( ":report",         report );

        if ( !query.exec() )
          qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

        seriesDbId = query.lastInsertId().toInt();
    }

    return seriesDbId;
}

void medDatabaseImporter::createMissingImages ( dtkAbstractData* dtkData, QSqlDatabase db, int seriesDbId, QStringList thumbPaths )
{
    QSqlQuery query ( db );

    QStringList filePaths  = dtkData->metaDataValues ( medMetaDataKeys::FilePaths.key() );

    if ( filePaths.count() == 1 && thumbPaths.count() > 1 ) // special case to 1 image and multiple thumbnails
    {
        QString seriesName  = medMetaDataKeys::SeriesDescription.getFirstValue(dtkData);

        QFileInfo fileInfo ( filePaths[0] );
        for ( int i = 0; i < thumbPaths.count(); i++ )
        {
            query.prepare ( "SELECT id FROM image WHERE series = :series AND name = :name" );
            query.bindValue ( ":series", seriesDbId );
            query.bindValue ( ":name", fileInfo.fileName() + QString().setNum ( i ) );

            if ( !query.exec() )
                qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

            if ( query.first() )
            {
                ; //qDebug() << "Image" << file << "already in database";
            }
            else
            {
                QString name_str = seriesName + "." + fileInfo.completeSuffix() + QString().setNum ( i );

                query.prepare ( "INSERT INTO image (series, name, path, instance_path, thumbnail, isIndexed) VALUES (:series, :name, :path, :instance_path, :thumbnail, :isIndexed)" );
                query.bindValue ( ":series", seriesDbId );
                query.bindValue ( ":name", name_str);
                query.bindValue ( ":path", fileInfo.filePath() );
                query.bindValue ( ":thumbnail", thumbPaths[i] );
                query.bindValue ( ":isIndexed", d->indexWithoutImporting );

                // if we are indexing we want to leave the 'instance_path' column blank
                // as we will use the full path in 'path' column to load them
                QString relativeFilePath = dtkData->metaDataValues ( "FileName" ) [0];
                query.bindValue ( ":instance_path", d->indexWithoutImporting ? "" : relativeFilePath );

                if ( !query.exec() )
                    qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
            }
        }
    }
    else
    {
        for ( int i = 0; i < filePaths.count(); i++ )
        {
            QFileInfo fileInfo ( filePaths[i] );

            query.prepare ( "SELECT id FROM image WHERE series = :series AND name = :name" );
            query.bindValue ( ":series", seriesDbId );
            query.bindValue ( ":name", fileInfo.fileName() );

            if ( !query.exec() )
                qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

            if ( query.first() )
            {
                ; //qDebug() << "Image" << file << "already in database";
            }
            else
            {
                query.prepare ( "INSERT INTO image (series, name, path, instance_path, thumbnail, isIndexed) VALUES (:series, :name, :path, :instance_path, :thumbnail, :isIndexed)" );
                query.bindValue ( ":series", seriesDbId );
                query.bindValue ( ":name", fileInfo.fileName() );
                query.bindValue ( ":path", fileInfo.filePath() );
                query.bindValue ( ":isIndexed", d->indexWithoutImporting );

                // if we are indexing we want to leave the 'instance_path' column blank
                // as we will use the full path in 'path' column to load them
                QString relativeFilePath = dtkData->metaDataValues ( "FileName" ) [0];
                query.bindValue ( ":instance_path", d->indexWithoutImporting ? "" : relativeFilePath );

                if ( i < thumbPaths.count() )
                    query.bindValue ( ":thumbnail", thumbPaths[i] );
                else
                    query.bindValue ( ":thumbnail", "" );

                if ( !query.exec() )
                    qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------------------

dtkSmartPointer<dtkAbstractDataReader> medDatabaseImporter::getSuitableReader ( QStringList filename )
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

dtkSmartPointer<dtkAbstractDataWriter> medDatabaseImporter::getSuitableWriter(QString filename,dtkAbstractData* dtkData)
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

QStringList medDatabaseImporter::getAllFilesToBeProcessed ( QString fileOrDirectory )
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

dtkSmartPointer<dtkAbstractData> medDatabaseImporter::tryReadImages ( const QStringList& filesPaths,const bool readOnlyImageInformation )
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
QString medDatabaseImporter::determineFutureImageFileName ( const dtkAbstractData* dtkdata, int volumeNumber )
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

QString medDatabaseImporter::determineFutureImageExtensionByDataType ( const dtkAbstractData* dtkdata )
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

bool medDatabaseImporter::tryWriteImage ( QString filePath, dtkAbstractData* imData )
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

void medDatabaseImporter::addAdditionalMetaData ( dtkAbstractData* imData, QString aggregatedFileName, QStringList aggregatedFilesPaths )
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

QString medDatabaseImporter::generateUniqueVolumeId ( const dtkAbstractData* dtkData )
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

QString medDatabaseImporter::ensureUniqueSeriesName ( const QString seriesName )
{
    QSqlDatabase db = * ( medDatabaseController::instance()->database() );

    QString originalSeriesName = seriesName;
    QString newSeriesName = seriesName;
    int suffix = 0;

    bool continueSearch = true;

    while (continueSearch)
    {
        QSqlQuery query ( db );
        query.prepare ( "SELECT * FROM series WHERE name = :seriesName" );
        query.bindValue ( ":seriesName", newSeriesName );

        if ( !query.exec() )
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

        if ( query.first() )
        {
           // it exist
            suffix++;
            newSeriesName = originalSeriesName + "_" + QString::number(suffix);
        }
        else
        {
            // it does not exist
            continueSearch = false;
        }
    }

    return newSeriesName;
}
