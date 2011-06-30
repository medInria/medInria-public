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

#include "medDatabaseImporter.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>
#include <medSql/medDatabaseController.h>
#include <medCore/medStorage.h>


class medDatabaseImporterPrivate
{
public:
    QString file;
    static QMutex mutex;
    QString lastSuccessfulReaderDescription;
    QString lastSuccessfulWriterDescription;
    bool isCancelled;
    bool indexWithoutImporting;
};

QMutex medDatabaseImporterPrivate::mutex;

//-----------------------------------------------------------------------------------------------------------

medDatabaseImporter::medDatabaseImporter(const QString& file, bool indexWithoutImporting = false) : medJobItem(), d(new medDatabaseImporterPrivate)
{
    d->isCancelled = false;
    d->lastSuccessfulReaderDescription = "";
    d->lastSuccessfulWriterDescription = "";
    d->file = file;
    d->indexWithoutImporting = indexWithoutImporting;
}

//-----------------------------------------------------------------------------------------------------------

medDatabaseImporter::~medDatabaseImporter(void)
{
    delete d;

    d = NULL;
}

//-----------------------------------------------------------------------------------------------------------

void medDatabaseImporter::run(void)
{
    if (d->indexWithoutImporting)
    {
        index();
    }
    else
    {
        import();
    }
}

void medDatabaseImporter::import(void)
{
    QMutexLocker locker(&d->mutex);

    /* The idea of this algorithm can be summarized in 3 steps:
     * 1. Get a list of all the files that will (try to) be imported
     * 2. Filter files that cannot be read, or won't be possible to write afterwards, or are already in the db
     * 3. Fill files metadata, write them to the db, and populate db tables
     *
     * note that depending on the input files, they might be aggregated by volume
     */

    // 1) Obtain a list of all the files that are going to be processed
    // this flattens the tree structure (if the input is a directory)
    // and puts all the files in one single list
    QStringList fileList = getAllFilesToBeProcessed(d->file);

    // Files that pass the filters named above are put
    // in this map so they are written in the db after
    QMap<QString, QStringList> imagesToWriteMap;

    int currentFileNumber = 0; // this variable will be used only for calculating progress

    // depending on the input files, they might be aggregated
    // that is: files corresponding to the same volume will be written
    // in a single output meta file (.mha)
    // this map is used to store a unique id per volume and its volume number
    QMap<QString, int> volumeUniqueIdToVolumeNumber;
    int volumeNumber = 1;

    // 2) Select (by filtering) files to be imported
    //
    // In this first loop we read the headers of all the images to be imported
    // and check if we don't have any problem in reading the file, the header
    // or in selecting a proper format to store the new file afterwards
    // new files ARE NOT written in medinria database yet, but are stored in a map for writing in a posterior step
    foreach (QString file, fileList)
    {
        if (d->isCancelled) // check if user cancelled the process
            break;

        emit progressed(this,((qreal)currentFileNumber/(qreal)fileList.count())*50.0); //TODO: reading and filtering represents 50% of the importing process?

        currentFileNumber++;

        QFileInfo fileInfo(file);

        dtkAbstractData* dtkData = NULL;

        // 2.1) Try reading file information, just the header not the whole file

        bool readOnlyImageInformation = true;
        dtkData = tryReadImages(QStringList(fileInfo.filePath()), readOnlyImageInformation);

        if (!dtkData)
        {
            qWarning() << "Reader was unable to read: " << fileInfo.filePath();
            continue;
        }

        // 2.2) Fill missing metadata
        populateMissingMetadata(dtkData, fileInfo.baseName());

        // 2.3) Generate an unique id for each volume
        // all images of the same volume should share the same id
        QString volumeId = generateUniqueVolumeId(dtkData);

        // check whether the image belongs to a new volume
        if (!volumeUniqueIdToVolumeNumber.contains(volumeId))
        {
            volumeUniqueIdToVolumeNumber[volumeId] = volumeNumber;
            volumeNumber++;
        }        

        // 2.3) a) Determine future file name and path based on patient/study/series/image
        // i.e.: where we will write the imported image
        QString imageFileName = determineFutureImageFileName(dtkData, volumeUniqueIdToVolumeNumber[volumeId]);

        // 2.3) b) Find the proper extension according to the type of the data
        // i.e.: in which format we will write the file in our database
        QString futureExtension  = determineFutureImageExtensionByDataType(dtkData);

        if (futureExtension.isEmpty())
        {
            emit showError(this, tr("Could not save file due to unhandled data type: ") + dtkData->description(), 5000);
            continue;
        }

        imageFileName = imageFileName + futureExtension;

        // 2.4) c) Add the image to a map for writing them all in medinria's database in a posterior step

        // First check if patient/study/series/image path already exists in the database
        if (!checkIfExists(dtkData, &fileInfo))
            imagesToWriteMap[imageFileName] << fileInfo.filePath();

        // afterwards we will re-read all the files, so we destroy the dtkData object
        delete dtkData;
    }

    // some checks to see if the user cancelled or something failed
    if (d->isCancelled)
    {
        emit showError(this, tr("User cancelled import process"), 5000);
        emit cancelled(this);
        return;
    }


    // 3) Re-read selected files and re-populate them with missing metadata
    //    then write them to medinria db and populate db tables

    QMap<QString, QStringList>::const_iterator it = imagesToWriteMap.begin();

    // 3.1) first check is after the filtering we have something to import
    // maybe we had problems with all the files, or they were already in the database
    if (it == imagesToWriteMap.end())
    {
        // TODO we know if it's either one or the other error, we can make this error better...
        emit showError(this, tr("No compatible image found or all of them had been already imported."), 5000);
        emit failure(this);
        return;
    }
    else
        qDebug() << "Image map contains " << imagesToWriteMap.size() << " files";

    int imagesCount = imagesToWriteMap.count(); // used only to calculate progress
    int currentImageIndex = 0; // used only to calculate progress

    // final loop: re-read, re-populate and write to db
    for (it; it != imagesToWriteMap.end(); it++)
    {
        emit progressed(this,((qreal)currentImageIndex/(qreal)imagesCount)*50.0 + 50.0); // 50? I do not think that reading all the headers is half the job...

        currentImageIndex++;

        QString aggregatedFileName = it.key(); // note that this file might be aggregating more than one input files
        QStringList filesPaths = it.value(); // input files being aggregated, might be only one or many

        //qDebug() << currentImageIndex << ": " << aggregatedFileName << "with " << filesPaths.size() << " files";

        dtkAbstractData* imageDtkData = NULL;

        QFileInfo imagefileInfo(filesPaths[0]);

        // 3.2) Try to read the whole image, not just the header
        bool readOnlyImageInformation = false;
        imageDtkData = tryReadImages(filesPaths, readOnlyImageInformation);

        if (imageDtkData)
        {
            // 3.3) a) re-populate missing metadata
            populateMissingMetadata(imageDtkData, imagefileInfo.baseName());

            // 3.3) b) now we are able to add some more metadata
            addAdditionalMetaData(imageDtkData, aggregatedFileName, filesPaths);
           }
        else
        {
            qWarning() << "Could not repopulate data!";
            emit showError(this, tr ("Could not read data: ") + filesPaths[0], 5000);
            continue;
        }

        // create location to store file
        QFileInfo fileInfo( medStorage::dataLocation() + aggregatedFileName );
        if ( !fileInfo.dir().exists() && !medStorage::mkpath(fileInfo.dir().path()) )
        {
            qDebug() << "Cannot create directory: " << fileInfo.dir().path();
            continue;
        }

        // now writing file
        bool writeSuccess = tryWriteImage(fileInfo.filePath(), imageDtkData);

        if (!writeSuccess){
            emit showError(this, tr ("Could not save data file: ") + filesPaths[0], 5000);
            continue;
        }

        // and finally we populate the database
        QFileInfo seriesInfo( aggregatedFileName );
        this->popupateDatabase(imageDtkData, &seriesInfo);

        delete imageDtkData;
        imageDtkData = NULL;
    } // end of the final loop


    emit progressed(this,100);
    emit success(this);
}

void medDatabaseImporter::index(void)
{
    // TODO...
    qDebug() << "TODO.....";
}

//-----------------------------------------------------------------------------------------------------------

void medDatabaseImporter::onCancel( QObject* )
{
    d->isCancelled = true;
}

//-----------------------------------------------------------------------------------------------------------

void medDatabaseImporter::populateMissingMetadata( dtkAbstractData* dtkData, const QString fileBaseName )
{
    if (!dtkData)
        return;

    if(!dtkData->hasMetaData("PatientName"))
        dtkData->addMetaData("PatientName", QStringList() << "John Doe");

    if(!dtkData->hasMetaData("StudyDescription"))
        dtkData->addMetaData("StudyDescription", QStringList() << "EmptyStudy");

    if(!dtkData->hasMetaData("SeriesDescription"))
        dtkData->addMetaData("SeriesDescription", QStringList() << fileBaseName);

    if(!dtkData->hasMetaData("StudyID"))
        dtkData->addMetaData("StudyID", QStringList() << "");

    if(!dtkData->hasMetaData("SeriesID"))
        dtkData->addMetaData("SeriesID", QStringList() << "");

    if(!dtkData->hasMetaData("Orientation"))
        dtkData->addMetaData("Orientation", QStringList() << "");

    if(!dtkData->hasMetaData("SeriesNumber"))
        dtkData->addMetaData("SeriesNumber", QStringList() << "");

    if(!dtkData->hasMetaData("SequenceName"))
        dtkData->addMetaData("SequenceName", QStringList() << "");

    if(!dtkData->hasMetaData("SliceThickness"))
        dtkData->addMetaData("SliceThickness", QStringList() << "");

    if(!dtkData->hasMetaData("Rows"))
        dtkData->addMetaData("Rows", QStringList() << "");

    if(!dtkData->hasMetaData("Columns"))
        dtkData->addMetaData("Columns", QStringList() << "");

    if(!dtkData->hasMetaData("Age"))
        dtkData->addMetaData("Age", QStringList() << "");

    if(!dtkData->hasMetaData("BirthDate"))
        dtkData->addMetaData("BirthDate", QStringList() << "");

    if(!dtkData->hasMetaData("Gender"))
        dtkData->addMetaData("Gender", QStringList() << "");

    if(!dtkData->hasMetaData("Description"))
        dtkData->addMetaData("Description", QStringList() << "");

    if(!dtkData->hasMetaData("Modality"))
        dtkData->addMetaData("Modality", QStringList() << "");

    if(!dtkData->hasMetaData("Protocol"))
        dtkData->addMetaData("Protocol", QStringList() << "");

    if(!dtkData->hasMetaData("Comments"))
        dtkData->addMetaData("Comments", QStringList() << "");

    if(!dtkData->hasMetaData("Status"))
        dtkData->addMetaData("Status", QStringList() << "");

    if(!dtkData->hasMetaData("AcquisitionDate"))
        dtkData->addMetaData("AcquisitionDate", QStringList() << "");

    if(!dtkData->hasMetaData("ImportationDate"))
        dtkData->addMetaData("ImportationDate", QStringList() << "");

    if(!dtkData->hasMetaData("Referee"))
        dtkData->addMetaData("Referee", QStringList() << "");

    if(!dtkData->hasMetaData("Performer"))
        dtkData->addMetaData("Performer", QStringList() << "");

    if(!dtkData->hasMetaData("Institution"))
        dtkData->addMetaData("Institution", QStringList() << "");

    if(!dtkData->hasMetaData("Report"))
        dtkData->addMetaData("Report", QStringList() << "");
}

//-----------------------------------------------------------------------------------------------------------

bool medDatabaseImporter::checkIfExists( dtkAbstractData* dtkdata, const QFileInfo * fileInfo  )
{
    bool imageExists = false;

    QSqlQuery query(*(medDatabaseController::instance()->database()));

    QString patientName = dtkdata->metaDataValues(tr("PatientName"))[0];
    QString studyName   = dtkdata->metaDataValues(tr("StudyDescription"))[0];
    QString seriesName  = dtkdata->metaDataValues(tr("SeriesDescription"))[0];

    QString studyId = dtkdata->metaDataValues(tr("StudyID"))[0];
    QString seriesId = dtkdata->metaDataValues(tr("SeriesID"))[0];
    QString orientation = dtkdata->metaDataValues(tr("Orientation"))[0]; // orientation sometimes differ by a few digits, thus this is not reliable
    QString seriesNumber = dtkdata->metaDataValues(tr("SeriesNumber"))[0];
    QString sequenceName = dtkdata->metaDataValues(tr("SequenceName"))[0];
    QString sliceThickness = dtkdata->metaDataValues(tr("SliceThickness"))[0];
    QString rows = dtkdata->metaDataValues(tr("Rows"))[0];
    QString columns = dtkdata->metaDataValues(tr("Columns"))[0];

    query.prepare("SELECT id FROM patient WHERE name = :name");
    query.bindValue(":name", patientName);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    QVariant id;
    if(query.first()) {
        id = query.value(0);

        query.prepare("SELECT id FROM study WHERE patient = :id AND name = :name AND uid = :studyID");
        query.bindValue(":id", id);
        query.bindValue(":name", studyName);
        query.bindValue(":studyID", studyId);
        if(!query.exec())
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

        if(query.first()) {
            id = query.value(0);

            query.prepare("SELECT id FROM series WHERE study = :id AND name = :name AND uid = :seriesID AND orientation = :orientation AND seriesNumber = :seriesNumber AND sequenceName = :sequenceName AND sliceThickness = :sliceThickness AND rows = :rows AND columns = :columns");
            query.bindValue(":id", id);
            query.bindValue(":name", seriesName);
            query.bindValue(":seriesID", seriesId);
            query.bindValue(":orientation", orientation);
            query.bindValue(":seriesNumber", seriesNumber);
            query.bindValue(":sequenceName", sequenceName);
            query.bindValue(":sliceThickness", sliceThickness);
            query.bindValue(":rows", rows);
            query.bindValue(":columns", columns);

            if(!query.exec())
                qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

            if(query.first()) {
                id = query.value(0);

                query.prepare("SELECT id FROM image WHERE series = :id AND name = :name");
                query.bindValue(":id", id);
                query.bindValue(":name", fileInfo->fileName());

                if(!query.exec())
                    qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

                if(query.first()) {
                    imageExists = true;
                }
            }
        }
    }

    return imageExists;
}

//-----------------------------------------------------------------------------------------------------------

void medDatabaseImporter::popupateDatabase( dtkAbstractData* dtkdata, const QFileInfo* seriesInfo )
{

    QString patientName = dtkdata->metaDataValues(tr("PatientName"))[0].simplified();
    QString studyName   = dtkdata->metaDataValues(tr("StudyDescription"))[0].simplified();
    QString seriesName  = dtkdata->metaDataValues(tr("SeriesDescription"))[0].simplified();

    QString studyId        = dtkdata->metaDataValues(tr("StudyID"))[0];
    QString seriesId       = dtkdata->metaDataValues(tr("SeriesID"))[0];
    int size               = dtkdata->metaDataValues(tr("Size"))[0].toInt();

    QString orientation    = dtkdata->metaDataValues(tr("Orientation"))[0];
    QString seriesNumber   = dtkdata->metaDataValues(tr("SeriesNumber"))[0];
    QString sequenceName   = dtkdata->metaDataValues(tr("SequenceName"))[0];
    QString sliceThickness = dtkdata->metaDataValues(tr("SliceThickness"))[0];
    QString rows           = dtkdata->metaDataValues(tr("Rows"))[0];
    QString columns        = dtkdata->metaDataValues(tr("Columns"))[0];

    QString age            = dtkdata->metaDataValues(tr("Age"))[0];
    QString birthdate      = dtkdata->metaDataValues(tr("BirthDate"))[0];
    QString gender         = dtkdata->metaDataValues(tr("Gender"))[0];
    QString description    = dtkdata->metaDataValues(tr("Description"))[0];
    QString modality       = dtkdata->metaDataValues(tr("Modality"))[0];
    QString protocol       = dtkdata->metaDataValues(tr("Protocol"))[0];
    QString comments       = dtkdata->metaDataValues(tr("Comments"))[0];
    QString status         = dtkdata->metaDataValues(tr("Status"))[0];
    QString acqdate        = dtkdata->metaDataValues(tr("AcquisitionDate"))[0];
    QString importdate     = dtkdata->metaDataValues(tr("ImportationDate"))[0];
    QString referee        = dtkdata->metaDataValues(tr("Referee"))[0];
    QString performer      = dtkdata->metaDataValues(tr("Performer"))[0];
    QString institution    = dtkdata->metaDataValues(tr("Institution"))[0];
    QString report         = dtkdata->metaDataValues(tr("Report"))[0];

    QStringList filePaths  = dtkdata->metaDataValues (tr("FilePaths"));


    //            QString s_age;
    //            if (dtkdata->hasMetaData(tr("(0010,1010)")))
    //                s_age=dtkdata->metaDataValues(tr("(0010,1010)"))[0];


    // QString patientPath;
    // QString studyPath;
    QString seriesPath = dtkdata->metaDataValues (tr("FileName"))[0];

    QSqlQuery query(*(medDatabaseController::instance()->database()));
    QVariant id;

    // generate and save the thumbnails
    QList<QImage> &thumbnails = dtkdata->thumbnails();
    //QFileInfo seriesInfo( it.key() );
    QString thumb_dir = seriesInfo->dir().path() + "/" + seriesInfo->completeBaseName() /*seriesName.simplified()*/ + "/";
    QStringList thumbPaths;

    // if (thumbnails.count())
    if (!medStorage::mkpath (medStorage::dataLocation() + thumb_dir))
        qDebug() << "Cannot create directory: " << thumb_dir;

    for (int j=0; j<thumbnails.count(); j++) {
        QString thumb_name = thumb_dir + QString().setNum (j) + ".png";
        thumbnails[j].save(medStorage::dataLocation() + thumb_name, "PNG");
        thumbPaths << thumb_name;
    }

    QImage thumbnail = dtkdata->thumbnail(); // representative thumbnail for PATIENT/STUDY/SERIES
    QString thumbPath = thumb_dir + "ref.png";
    thumbnail.save (medStorage::dataLocation() + thumbPath, "PNG");


    ////////////////////////////////////////////////////////////////// PATIENT

    query.prepare("SELECT id FROM patient WHERE name = :name");
    query.bindValue(":name", patientName);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first()) {
        id = query.value(0);
        //patientPath = this->dataLocation() + "/" + QString().setNum (id.toInt());
    }
    else {
        query.prepare("INSERT INTO patient (name, thumbnail, birthdate, gender) VALUES (:name, :thumbnail, :birthdate, :gender)");
        query.bindValue(":name", patientName);
        query.bindValue(":thumbnail", thumbPath );
        query.bindValue(":birthdate", birthdate );
        query.bindValue(":gender",    gender );
        query.exec(); id = query.lastInsertId();

        //patientPath = this->dataLocation() + "/" + QString().setNum (id.toInt());

        //                if (!QDir (patientPath).exists() && !this->mkpath (patientPath))
        //                    qDebug() << "Cannot create directory: " << patientPath;

    }


    ////////////////////////////////////////////////////////////////// STUDY

    query.prepare("SELECT id FROM study WHERE patient = :id AND name = :name AND uid = :studyID");
    query.bindValue(":id", id);
    query.bindValue(":name", studyName);
    query.bindValue(":studyID", studyId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first()) {
        id = query.value(0);
        //studyPath = patientPath + "/" + QString().setNum (id.toInt());
    }
    else {
        query.prepare("INSERT INTO study (patient, name, uid, thumbnail) VALUES (:patient, :study, :studyID, :thumbnail)");
        query.bindValue(":patient", id);
        query.bindValue(":study", studyName);
        query.bindValue(":studyID", studyId);
        //if (thumbPaths.count())
        query.bindValue(":thumbnail", thumbPath );
        //                else
        //                query.bindValue(":thumbnail", "");

        query.exec(); id = query.lastInsertId();

        //studyPath = patientPath + "/" + QString().setNum (id.toInt());

        //                if (!QDir (studyPath).exists() && !this->mkpath (studyPath))
        //                    qDebug() << "Cannot create directory: " << studyPath;

    }


    ///////////////////////////////////////////////////////////////// SERIES

    query.prepare("SELECT * FROM series WHERE study = :id AND name = :name AND uid = :seriesID AND orientation = :orientation AND seriesNumber = :seriesNumber AND sequenceName = :sequenceName AND sliceThickness = :sliceThickness AND rows = :rows AND columns = :columns");
    query.bindValue(":id", id);
    query.bindValue(":name", seriesName);
    query.bindValue(":seriesID", seriesId);
    query.bindValue(":orientation", orientation);
    query.bindValue(":seriesNumber", seriesNumber);
    query.bindValue(":sequenceName", sequenceName);
    query.bindValue(":sliceThickness", sliceThickness);
    query.bindValue(":rows", rows);
    query.bindValue(":columns", columns);

    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first()) {
        id = query.value(0);
        QVariant seCount = query.value (2);
        QVariant seName  = query.value (3);
        QVariant sePath  = query.value (4);

        //seriesPath = studyPath + "/" + QString().setNum (id.toInt()) + ".mhd";
    }
    else {

        query.prepare("INSERT INTO series (study, size, name, path, uid, orientation, seriesNumber, sequenceName, sliceThickness, rows, columns, thumbnail, age, description, modality, protocol, comments, status, acquisitiondate, importationdate, referee, performer, institution, report) VALUES (:study, :size, :name, :path, :seriesID, :orientation, :seriesNumber, :sequenceName, :sliceThickness, :rows, :columns, :thumbnail, :age, :description, :modality, :protocol, :comments, :status, :acquisitiondate, :importationdate, :referee, :performer, :institution, :report)");
        query.bindValue(":study",          id);
        query.bindValue(":size",           size);
        query.bindValue(":name",           seriesName);
        query.bindValue(":path",           seriesPath);
        query.bindValue(":seriesID",       seriesId);
        query.bindValue(":orientation",    orientation);
        query.bindValue(":seriesNumber",   seriesNumber);
        query.bindValue(":sequenceName",   sequenceName);
        query.bindValue(":sliceThickness", sliceThickness);
        query.bindValue(":rows",           rows);
        query.bindValue(":columns",        columns);
        query.bindValue(":thumbnail",      thumbPath );
        query.bindValue(":age",         age);
        query.bindValue(":description", description);
        query.bindValue(":modality",    modality);
        query.bindValue(":protocol",    protocol);
        query.bindValue(":comments",    comments);
        query.bindValue(":status",      status);
        query.bindValue(":acquisitiondate",     acqdate);
        query.bindValue(":importationdate",  importdate);
        query.bindValue(":referee",     referee);
        query.bindValue(":performer",   performer);
        query.bindValue(":institution", institution);
        query.bindValue(":report",      report);

        if(!query.exec())
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

        id = query.lastInsertId();

        //seriesPath = studyPath + "/" + QString().setNum (id.toInt()) + ".mhd";
    }


    ///////////////////////////////////////////////////////////////// IMAGE

    if (filePaths.count()==1 && thumbPaths.count()>1) // special case to 1 image and multiple thumbnails
    {

        QFileInfo fileInfo( filePaths[0] );
        for (int j=0; j<thumbPaths.count(); j++)
        {
            query.prepare("SELECT id FROM image WHERE series = :id AND name = :name");
            query.bindValue(":id", id);
            query.bindValue(":name", fileInfo.fileName()+QString().setNum (j));

            if(!query.exec())
                qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

            if(query.first()) {
                ; //qDebug() << "Image" << file << "already in database";
            }
            else {

                query.prepare("INSERT INTO image (series, size, name, path, instance_path, thumbnail) VALUES (:series, :size, :name, :path, :instance_path, :thumbnail)");
                query.bindValue(":series", id);
                query.bindValue(":size", 64);
                query.bindValue(":name", fileInfo.fileName()+QString().setNum (j));
                query.bindValue(":path", fileInfo.filePath());
                query.bindValue(":instance_path", seriesPath);
                query.bindValue(":thumbnail", thumbPaths[j]);

                if(!query.exec())
                    qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
            }
        }

    }
    else {

        for (int j=0; j<filePaths.count(); j++) {

            QFileInfo fileInfo( filePaths[j] );

            query.prepare("SELECT id FROM image WHERE series = :id AND name = :name");
            query.bindValue(":id", id);
            query.bindValue(":name", fileInfo.fileName());

            if(!query.exec())
                qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

            if(query.first()) {
                ; //qDebug() << "Image" << file << "already in database";
            }
            else {

                query.prepare("INSERT INTO image (series, size, name, path, instance_path, thumbnail) VALUES (:series, :size, :name, :path, :instance_path, :thumbnail)");
                query.bindValue(":series", id);
                query.bindValue(":size", 64);
                query.bindValue(":name", fileInfo.fileName());
                query.bindValue(":path", fileInfo.filePath());
                query.bindValue(":instance_path", seriesPath);
                if (j<thumbPaths.count())
                    query.bindValue(":thumbnail", thumbPaths[j]);
                else
                    query.bindValue(":thumbnail", "");

                if(!query.exec())
                    qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------------------

dtkAbstractDataReader* medDatabaseImporter::getSuitableReader( QStringList filename )
{
    QList<QString> readers = dtkAbstractDataFactory::instance()->readers();

    // cycle through readers to see if the last used reader can handle the file
    for (int i=0; i<readers.size(); i++) {
        dtkAbstractDataReader* dataReader = dtkAbstractDataFactory::instance()->reader(readers[i]);
        if (d->lastSuccessfulReaderDescription == dataReader->description() && dataReader->canRead( filename ))
            return dataReader;
        else
            delete dataReader;
    }

    for (int i=0; i<readers.size(); i++) {
        dtkAbstractDataReader* dataReader = dtkAbstractDataFactory::instance()->reader(readers[i]);
        if (dataReader->canRead( filename )){
            d->lastSuccessfulReaderDescription = dataReader->description();
            return dataReader;
        }
        else
            delete dataReader;
    }

    qWarning() << "No suitable reader found!";

    return NULL;
}

//-----------------------------------------------------------------------------------------------------------

dtkAbstractDataWriter* medDatabaseImporter::getSuitableWriter( QString filename, dtkAbstractData* dtkdata )
{
    if (!dtkdata)
        return NULL;

    QList<QString> writers = dtkAbstractDataFactory::instance()->writers();

    // first try with the last
    for (int i=0; i<writers.size(); i++) {
        dtkAbstractDataWriter *dataWriter = dtkAbstractDataFactory::instance()->writer(writers[i]);
        if (d->lastSuccessfulReaderDescription == dataWriter->description()) {

            if ( dataWriter->handled().contains(dtkdata->description()) &&
                 dataWriter->canWrite( filename ) ) {

                d->lastSuccessfulWriterDescription = dataWriter->description();
                return dataWriter;
            }
            else
                delete dataWriter;
        }
    }

    // cycle all
    for (int i=0; i<writers.size(); i++) {
        dtkAbstractDataWriter *dataWriter = dtkAbstractDataFactory::instance()->writer(writers[i]);

        if ( dataWriter->handled().contains(dtkdata->description()) &&
             dataWriter->canWrite( filename ) ) {

            d->lastSuccessfulWriterDescription = dataWriter->description();
            return dataWriter;
        }
        else
            delete dataWriter;
    }

    return NULL;
}

//-----------------------------------------------------------------------------------------------------------

QStringList medDatabaseImporter::getAllFilesToBeProcessed(QString fileOrDirectory)
{
    QString file = fileOrDirectory;

    QDir dir(file);
    dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

    QStringList fileList;
    if (dir.exists())
    {
       QDirIterator directory_walker(file, QDir::Files, QDirIterator::Subdirectories);
       while (directory_walker.hasNext())
       {
           fileList << directory_walker.next();
       }
    }
    else
       fileList << file;

    fileList.sort();

    return fileList;
}

dtkAbstractData* medDatabaseImporter::tryReadImages(QStringList filesPaths, bool readOnlyImageInformation)
{
    dtkAbstractData* dtkData = 0;

    dtkAbstractDataReader* dataReader = getSuitableReader(filesPaths);

    if (dataReader)
    {
        if (readOnlyImageInformation)
        {
            dataReader->readInformation( filesPaths );
        }
        else
        {
            dataReader->read( filesPaths );
        }

        dtkData = dataReader->data();
        delete dataReader;
    }
    else
    {
        // we take the first one for debugging just for simplicity
        qWarning() << "No suitable reader found for file: " << filesPaths[0] << ". Unable to import!";
    }

    return dtkData;
}

QString medDatabaseImporter::determineFutureImageFileName(dtkAbstractData* dtkdata, int volumeNumber)
{
    // we append the uniqueID at the end of the filename to have unique filenames for each volume
    QString s_volumeNumber;
    s_volumeNumber.setNum(volumeNumber);

    QString patientName = dtkdata->metaDataValues(tr("PatientName"))[0];
    QString studyName   = dtkdata->metaDataValues(tr("StudyDescription"))[0];
    QString seriesName  = dtkdata->metaDataValues(tr("SeriesDescription"))[0];

    QString s_patientName = patientName.simplified();
    QString s_studyName   = studyName.simplified();
    QString s_seriesName  = seriesName.simplified();

    s_patientName.replace (0x00EA, 'e');
    s_studyName.replace   (0x00EA, 'e');
    s_seriesName.replace  (0x00EA, 'e');
    s_patientName.replace (0x00E4, 'a');
    s_studyName.replace   (0x00E4, 'a');
    s_seriesName.replace  (0x00E4, 'a');

    QString imageFileName = "/" + s_patientName + "/" +
            s_studyName   + "/" +
            s_seriesName  + s_volumeNumber;

    return imageFileName;
}

QString medDatabaseImporter::determineFutureImageExtensionByDataType(dtkAbstractData* dtkdata)
{
    QString description = dtkdata->description();
    QString extension = "";

     // Determine the appropriate extension to use according to the type of data.
     // TODO: The image type is weakly recognized (contains("Image")). to be improved
     if (description == "vtkDataMesh")
     {
         extension = ".vtk";
         qDebug() << "vtkDataMesh";
     }
     else if (description == "vtkDataMesh4D")
     {
         extension = ".v4d";
         qDebug() << "vtkDataMesh4D";
     }
     else if (description == "v3dDataFibers")
     {
         extension = ".xml";
         qDebug() << "vtkDataMesh4D";
     }
     else if (description.contains("vistal"))
     {
         extension = ".dim";
         qDebug() << "Vistal Image";
     }
     else if (description.contains ("Image"))
     {
         extension = ".mha";
         //qDebug() << description;
     }

     return extension;
}

bool medDatabaseImporter::tryWriteImage(QString filePath, dtkAbstractData* imData)
{
    bool writeSuccess = false;

    dtkAbstractDataWriter *dataWriter = getSuitableWriter(filePath, imData);
    if( dataWriter)
    {
        dataWriter->setData (imData);
        if ( dataWriter->write(filePath))
        {
            writeSuccess = true;
        }
        delete dataWriter;
    }

    return writeSuccess;
}

void medDatabaseImporter::addAdditionalMetaData(dtkAbstractData* imData, QString fileName, QStringList filePaths)
{
    QStringList size;
    if (dtkAbstractDataImage *imagedata = dynamic_cast<dtkAbstractDataImage*>(imData))
    {
        size << QString::number( imagedata->zDimension() );
    }
    else {
        size << "";
    }

    imData->setMetaData ("Size", size);

    if (!imData->hasMetaData ("FilePaths"))
        imData->addMetaData  ("FilePaths", filePaths);

    imData->addMetaData ("FileName", fileName);
}

QString medDatabaseImporter::generateUniqueVolumeId(dtkAbstractData* dtkData)
{
    if (!dtkData)
    {
        qWarning() << "data invalid";
        return "invalid";
    }

    // Get all the information from the dtkAbstractData metadata.
    // This information will then be passed to the database.
    QString patientName = dtkData->metaDataValues(tr("PatientName"))[0];
    QString studyName   = dtkData->metaDataValues(tr("StudyDescription"))[0];
    QString seriesName  = dtkData->metaDataValues(tr("SeriesDescription"))[0];

    QString studyId = dtkData->metaDataValues(tr("StudyID"))[0];
    QString seriesId = dtkData->metaDataValues(tr("SeriesID"))[0];
    QString orientation = dtkData->metaDataValues(tr("Orientation"))[0]; // orientation sometimes differ by a few digits, thus this is not reliable
    QString seriesNumber = dtkData->metaDataValues(tr("SeriesNumber"))[0];
    QString sequenceName = dtkData->metaDataValues(tr("SequenceName"))[0];
    QString sliceThickness = dtkData->metaDataValues(tr("SliceThickness"))[0];
    QString rows = dtkData->metaDataValues(tr("Rows"))[0];
    QString columns = dtkData->metaDataValues(tr("Columns"))[0];

    QStringList orientations = orientation.split(" ");

    orientation = "";

    // truncate orientation to 5 digits for a more robust import since
    // sometimes orientation only differs with the last 2 digits, creating
    // multiple series
    foreach(QString orient, orientations)
    {
        double d_orient = orient.toDouble();
        orientation += QString::number(d_orient, 'g', 5);
    }

    // define a unique key string to identify which volume an image belongs to.
    // we use: patientName, studyId, seriesId, orientation, seriesNumber, sequenceName, sliceThickness, rows, columns.
    // All images of the same volume should share similar values of these parameters
    QString key = patientName+studyId+seriesId+orientation+seriesNumber+sequenceName+sliceThickness+rows+columns;

    return key;
}
