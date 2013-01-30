/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseNonPersistentController.h"
#include "medDatabaseController.h"
#include "medDatabaseNonPersistentItem.h"
#include "medDatabaseNonPersistentItem_p.h"
#include "medDatabaseNonPersistentReader.h"

#include <medAbstractDataImage.h>
#include <medMetaDataKeys.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>

class medDatabaseNonPersistentReaderPrivate
{
public:
    medDatabaseNonPersistentReaderPrivate ( const QString& uuid ) :callerUuid ( uuid ) {}
    QString file;
    static QMutex mutex;
    bool isCancelled;
    const QString callerUuid;
    medDataIndex index;
};

QMutex medDatabaseNonPersistentReaderPrivate::mutex;

medDatabaseNonPersistentReader::medDatabaseNonPersistentReader ( const QString& file
        ,const QString& callerUuid ) : medAbstractDatabaseImporter(file,true), d ( new medDatabaseNonPersistentReaderPrivate ( callerUuid ) )
{
    d->file = file;
    d->isCancelled = false;
    qDebug() << "npr created with uuid:"<< d->callerUuid;
}

medDatabaseNonPersistentReader::~medDatabaseNonPersistentReader()
{
    delete d;

    d = NULL;
}

void medDatabaseNonPersistentReader::run()
{
    qDebug() << DTK_PRETTY_FUNCTION;
    
    QMutexLocker locker ( &d->mutex );

    if ( d->file.isEmpty() )
        return;

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
        
        emit progress ( this, ( int ) ( ( ( qreal ) currentFileNumber/ ( qreal ) fileList.count() ) *50.0 ) );

        currentFileNumber++;

        QFileInfo fileInfo ( file );

        dtkSmartPointer<dtkAbstractData> dtkdata;

        // 2.1) Try reading file information, just the header not the whole file

        bool readOnlyImageInformation = true;
        dtkdata = tryReadImages ( QStringList ( fileInfo.filePath() ), readOnlyImageInformation );

        if ( !dtkdata )
        {
            qWarning() << "Reader was unable to read: " << fileInfo.filePath();
            continue;
        }

         // 2.2) Fill missing metadata
        populateMissingMetadata ( dtkdata, fileInfo.baseName() );
        QString patientName = medMetaDataKeys::PatientName.getFirstValue(dtkdata);
        QString birthDate   = medMetaDataKeys::BirthDate.getFirstValue(dtkdata);
        tmpPatientId = patientName + birthDate;

        if(tmpPatientId != currentPatientId)
        {
          currentPatientId = tmpPatientId;

          patientID = getPatientID(patientName, birthDate);
        }

        dtkdata->setMetaData ( medMetaDataKeys::PatientID.key(), QStringList() << patientID );

        tmpSeriesUid = medMetaDataKeys::SeriesDicomID.getFirstValue(dtkdata);

        if (tmpSeriesUid != currentSeriesUid)
        {
          currentSeriesUid = tmpSeriesUid;
          currentSeriesId = medMetaDataKeys::SeriesID.getFirstValue(dtkdata);
        }
        else
          dtkdata->setMetaData ( medMetaDataKeys::SeriesID.key(), QStringList() << currentSeriesId );

        // 2.3) Generate an unique id for each volume
        // all images of the same volume should share the same id
        QString volumeId = generateUniqueVolumeId ( dtkdata );

        // check whether the image belongs to a new volume
        if ( !volumeUniqueIdToVolumeNumber.contains ( volumeId ) )
        {
            volumeUniqueIdToVolumeNumber[volumeId] = volumeNumber;
            volumeNumber++;
        }
        
        // 2.3) a) Determine future file name and path based on patient/study/series/image
        // i.e.: where we will write the imported image
        QString imageFileName = determineFutureImageFileName ( dtkdata, volumeUniqueIdToVolumeNumber[volumeId] );

#ifdef Q_OS_WIN32
        if ( (medStorage::dataLocation() + "/" + imageFileName).length() > 255 )
        {
            emit showError ( this, tr ( "Your database path is too long" ), 5000 );
            emit failure ( this );
            return;
        }
#endif
        // 2.3) b) Find the proper extension according to the type of the data
        // i.e.: in which format we will write the file in our database
        QString futureExtension  = determineFutureImageExtensionByDataType ( dtkdata );


        // 2.3) c) Add the image to a map for writing them all in medInria's database in a posterior step

        // First check if patient/study/series/image path already exists in the database
        // Should we emit a message otherwise ??? TO
        if ( !checkIfExists ( dtkdata, fileInfo.fileName() ) )
        {
            imagesGroupedByVolume[imageFileName] << fileInfo.filePath();
            imagesGroupedByPatient[imageFileName] = patientID;
            imagesGroupedBySeriesId[imageFileName] = currentSeriesId;
        }
    }
    
    // some checks to see if the user cancelled or something failed
    if ( d->isCancelled )
    {
        emit showError ( this, tr ( "User cancelled import process" ), 5000 );
        emit cancelled ( this );
        return;
    }

    // from now on the process cannot be cancelled
    emit disableCancel ( this );
    

    //QMap<QString, int>::const_iterator itk = keyToInt.begin();

    // read and write images in mhd format

    QList< dtkSmartPointer<dtkAbstractData> > dtkDataList;

    QMap<QString, QStringList>::const_iterator it = imagesGroupedByVolume.begin();
    QMap<QString, QString>::const_iterator itPat = imagesGroupedByPatient.begin();
    QMap<QString, QString>::const_iterator itSer = imagesGroupedBySeriesId.begin();

    
    // 3.1) first check is after the filtering we have something to import
    // maybe we had problems with all the files, or they were already in the database
    if ( it == imagesGroupedByVolume.end() )
    {
        // TODO we know if it's either one or the other error, we can make this error better...
        emit showError ( this, tr ( "No compatible image found or all of them had been already imported." ), 5000 );
        emit failure ( this );
        return;
    }
    else
        qDebug() << "Image map contains " << imagesGroupedByVolume.size() << " files";

    
    int imagesCount = imagesGroupedByVolume.count(); // used only to calculate progress
    int currentImageIndex = 0; // used only to calculate progress

    medDataIndex index; //stores the last volume's index to be emitted on success

    // final loop: re-read, re-populate and write to npdb
    for ( it; it!=imagesGroupedByVolume.end(); it++ )
    {
        emit progress ( this, ( int ) ( ( ( qreal ) currentImageIndex/ ( qreal ) imagesCount ) *50.0 + 50.0 ) );

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
            emit showError ( this, tr ( "Could not read data: " ) + filesPaths[0], 5000 );
            continue;
        }


        // and finally we populate the database
        QFileInfo aggregatedFileNameFileInfo ( aggregatedFileName );
        QString pathToStoreThumbnails = aggregatedFileNameFileInfo.dir().path() + "/" + aggregatedFileNameFileInfo.completeBaseName() + "/";
        index = this->populateDatabaseAndGenerateThumbnails ( imageDtkData, pathToStoreThumbnails );

        itPat++;
        itSer++;

    }

    d->index = index;

    emit progress ( this, 100 );
    emit success ( this );
//    qDebug() << "uuid value before signal"<< d->callerUuid;
    emit nonPersistentRead ( index,d->callerUuid );
}


QString medDatabaseNonPersistentReader::getPatientID(QString patientName, QString birthDate)
{
    //TODO GPR : à reprendre
    QString patientID = QUuid::createUuid().toString().replace("{","").replace("}","");

    return patientID;
}

medDataIndex medDatabaseNonPersistentReader::populateDatabaseAndGenerateThumbnails ( dtkAbstractData* data, QString pathToStoreThumbnails )
{
    QPointer<medDatabaseNonPersistentControllerImpl> npdc =
            medDatabaseNonPersistentController::instance();
            
    QList<medDatabaseNonPersistentItem*> items = npdc->items();

    int     patientDbId   = -1;
    QString patientName = medMetaDataKeys::PatientName.getFirstValue(data);
    QString patientId = medMetaDataKeys::PatientID.getFirstValue(data);

    // check if patient is already in the persistent database
    medDataIndex databaseIndex = medDatabaseController::instance()->indexForPatient ( patientName );
    if ( databaseIndex.isValid() )
    {
        qDebug() << "Patient exists in the database, I reuse her Id";
        patientDbId = databaseIndex.patientId();
    }
    else
    {
        for ( int i=0; i<items.count(); i++ )
            if ( items[i]->name() ==patientName )
            {
                patientDbId = items[i]->index().patientId();
                break;
            }
    }

    if ( patientDbId==-1 )
        patientDbId = npdc->patientId ( true );

    int     studyDbId   = -1;
    QString studyName = medMetaDataKeys::StudyDescription.getFirstValue(data);
    QString studyId = medMetaDataKeys::StudyID.getFirstValue(data);

    databaseIndex = medDatabaseController::instance()->indexForStudy ( patientName, studyName );
    if ( databaseIndex.isValid() )
    {
        qDebug() << "Study exists in the database, I reuse its Id";
        studyDbId = databaseIndex.studyId();
    }
    else
    {
        for ( int i=0; i<items.count(); i++ )
            if ( items[i]->name() ==patientName && items[i]->studyName() ==studyName )
            {
                studyDbId = items[i]->index().studyId();
                break;
            }
    }

    if ( studyDbId==-1 )
        studyDbId = npdc->studyId ( true );

    medDataIndex index = medDataIndex ( npdc->dataSourceId(), patientDbId, studyDbId, npdc->seriesId ( true ), -1 );

    QString seriesName = medMetaDataKeys::SeriesDescription.getFirstValue(data);
    QString seriesId = medMetaDataKeys::SeriesID.getFirstValue(data);

    QFileInfo info ( d->file );

    medDatabaseNonPersistentItem *item = new medDatabaseNonPersistentItem;

    if ( !patientName.isEmpty() )
        item->d->name = patientName;
    else
        item->d->name = info.baseName();

    item->d->patientId = patientId;
    item->d->studyName = studyName;
    item->d->seriesName = seriesName;
    item->d->seriesId = seriesId;
    item->d->file = d->file;
    item->d->thumb = data->thumbnail();
    item->d->index = index;
    item->d->data = data;

    npdc->insert ( index, item );
    return index;
}


//-----------------------------------------------------------------------------------------------------------

bool medDatabaseNonPersistentReader::checkIfExists ( dtkAbstractData* dtkdata, QString imageName )
{
    bool imageExists = false;

    QPointer<medDatabaseNonPersistentControllerImpl> npdc =
            medDatabaseNonPersistentController::instance();
            
    QList<medDatabaseNonPersistentItem*> items = npdc->items();
    
    foreach(medDatabaseNonPersistentItem* item, items)
    {
        imageExists = item->file() == imageName;
        if (imageExists)
            break;
    }
    
    return imageExists;
}

//-----------------------------------------------------------------------------------------------------------



QString medDatabaseNonPersistentReader::ensureUniqueSeriesName ( const QString seriesName )
{
    QPointer<medDatabaseNonPersistentControllerImpl> npdc =
        medDatabaseNonPersistentController::instance();

    QList<medDatabaseNonPersistentItem*> items = npdc->items();

    QStringList seriesNames;
    foreach(medDatabaseNonPersistentItem* item, items)
    {
        QString sname = item->seriesName();
        if(sname.startsWith(seriesName) )
            seriesNames << sname;
    }

    QString originalSeriesName = seriesName;
    QString newSeriesName = seriesName;
    int suffix = 0;

    bool continueSearch = !seriesNames.isEmpty();

    while (seriesNames.contains(newSeriesName))
    {
        // it exist
        suffix++;
        newSeriesName = originalSeriesName + "_" + QString::number(suffix);
    }

    return newSeriesName;
}

medDataIndex medDatabaseNonPersistentReader::index() const
{
    return d->index;
}





