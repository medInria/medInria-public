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

#include <medAbstractDataImage.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>
#include <medDatabaseController.h>
#include <medMetaDataHelper.h>
#include <medStorage.h>


class medDatabaseImporterPrivate
{
public:
    QString file;
    static QMutex mutex;
    QString lastSuccessfulReaderDescription;
    QString lastSuccessfulWriterDescription;
    bool isCancelled;
};

QMutex medDatabaseImporterPrivate::mutex;

//-----------------------------------------------------------------------------------------------------------

medDatabaseImporter::medDatabaseImporter(const QString& file) :medJobItem(), d(new medDatabaseImporterPrivate)
{
    d->isCancelled = false;
    d->lastSuccessfulReaderDescription = "";
    d->lastSuccessfulWriterDescription = "";
    d->file = file;
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
    QMutexLocker locker(&d->mutex);

    QString file = d->file;

    QDir dir(file);
    dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

    QStringList fileList;
    if (dir.exists()) {
        QDirIterator directory_walker(file, QDir::Files, QDirIterator::Subdirectories);
        while (directory_walker.hasNext()) {
            fileList << directory_walker.next();
        }
    }
    else
        fileList << file;

    fileList.sort();


    QMap<QString, QStringList> imagesToWriteMap;


    int fileCount = fileList.count();
    int fileIndex = 0;

    QMap<QString, int> keyToInt;
    int currentIndex = 1;

    // Image importing loop
    foreach (QString file, fileList) {

        if (d->isCancelled)
            break;

        emit progressed(this,((qreal)fileIndex/(qreal)fileCount)*50.0);

        fileIndex++;

        QFileInfo fileInfo( file );
        QString filename = fileInfo.filePath();
        dtkSmartPointer<dtkAbstractData> dtkdata;

        dtkSmartPointer<dtkAbstractDataReader> dataReader = this->getSuitableReader(QStringList(fileInfo.filePath()));
        if (dataReader){
            dataReader->readInformation( filename );
            dtkdata.takePointer(dataReader->data());
            dtkdata->enableDeferredDeletion(false);
            //dataReader = NULL;
        }
        else {
        qWarning() << "No suitable reader found for file: " << fileInfo.filePath() << " unable to import!";
        continue;
        }


        if (!dtkdata) {
            qWarning() << "Reader was unable to read: " << fileInfo.filePath();
            continue;
        }

        QString key  = populateMissingMetadata(dtkdata, &fileInfo);

        if (!keyToInt.contains(key)) {
            keyToInt[key] = currentIndex;
            currentIndex++;
        }        

        // we append the uniqueID at the end of the filename to have unique filenames for each volume
        QString uniqueSeriesId;
        uniqueSeriesId.setNum(keyToInt[key]);

        QString patientName = dtkdata->metaDataValues(medMetaDataHelper::KEY_PatientName())[0];
        QString studyName   = dtkdata->metaDataValues(medMetaDataHelper::KEY_StudyDescription())[0];
        QString seriesName  = dtkdata->metaDataValues(medMetaDataHelper::KEY_SeriesDescription())[0];

        medDatabaseControllerImpl * dbi = medDatabaseController::instance();

        QString s_patientName = dbi->stringForPath( patientName ) ;
        QString s_studyName   = dbi->stringForPath( studyName );
        QString s_seriesName  = dbi->stringForPath( seriesName );

        QString imageFileName = "/" + s_patientName + "/" +
                s_studyName   + "/" +
                s_seriesName  + uniqueSeriesId;
        QString description = dtkdata->description();


        // Determine the appropriate extension to use according to the type
        // of data.
        // todo: The image type is weakly recognized (contains("Image")). to be improved
        if (description == "vtkDataMesh") {
            imageFileName = imageFileName + ".vtk";
            qDebug()<<"vtkDataMesh";}
        else if (description == "vtkDataMesh4D"){
            imageFileName = imageFileName + ".v4d";
            qDebug()<<"vtkDataMesh4D";}
        else if (description == "v3dDataFibers"){
            imageFileName = imageFileName + ".xml";
            qDebug()<<"vtkDataMesh4D";}
        else if (description.contains ("vistal")){
            imageFileName = imageFileName + ".dim";
            qDebug()<<"Vistal Image";}
        else if (description.contains ("Image")){
            imageFileName = imageFileName + ".mha";
            //qDebug()<<"Image";
        }
        else {
            emit showError(this, tr ("Could not save data file (unhandled type: ") + description,5000);
        }

        // Check if PATIENT/STUDY/SERIES/IMAGE already exists in the database
        if (!checkIfExists(dtkdata, &fileInfo))
            imagesToWriteMap[imageFileName] << fileInfo.filePath();

    } // END of Image importing loop


    // some checks to see if the user cancelled or something failed
    if (d->isCancelled)
    {
        emit showError(this, tr("User cancelled import process"),5000);
        emit cancelled(this);
        return;
    }


    // read and write images in mhd format
    QMap<QString, int>::const_iterator itk = keyToInt.begin();
    QList<dtkAbstractData*> dtkDataList;
    QMap<QString, QStringList>::const_iterator it = imagesToWriteMap.begin();

    if (it == imagesToWriteMap.end())
    {
        emit showError(this, tr("No compatible or new image was found"),5000);
        emit failure(this);
        return;
    }
    else
    {
        qDebug() << "image map contains: " << it.value().count() << " files";
    }


    int imagesCount = imagesToWriteMap.count();
    int imageIndex = 0;

    // Image write loop
    for (it; it!=imagesToWriteMap.end(); it++) {

        emit progressed(this,((qreal)imageIndex/(qreal)imagesCount)*50.0 + 50.0);

        imageIndex++;

        dtkSmartPointer<dtkAbstractData> imData;

        // reading again ...
        dtkSmartPointer<dtkAbstractDataReader> dataReader = this->getSuitableReader(it.value());
        if (dataReader) {
            dataReader->readInformation( it.value() );
            dataReader->read( it.value() );
            imData.takePointer(dataReader->data());
            QFileInfo fileInfo(it.value()[0]);
            if (imData) {
                // populating again...
                populateMissingMetadata(imData, &fileInfo);

                // some addition information
                QStringList size;
                if (medAbstractDataImage *imagedata = dynamic_cast<medAbstractDataImage*> (imData.data()) ) {
                    size << QString::number (imagedata->zDimension() );
                }
                else {
                    size << "";
                }
                imData->setMetaData (medMetaDataHelper::KEY_Size(), size);

                if (!imData->hasMetaData (medMetaDataHelper::KEY_FilePaths()))
                    imData->addMetaData  (medMetaDataHelper::KEY_FilePaths(), it.value());

                imData->addMetaData ("FileName", it.key() );
            }
            else {
                qWarning() << "Could not repopulate data!";
            }
        }

        if (!imData) {
            emit showError(this, tr ("Could not read data: ") + it.value()[0],5000);
            continue;
        }

        // create location to store file
        QFileInfo fileInfo ( medStorage::dataLocation() + it.key());
        if (!fileInfo.dir().exists() && !medStorage::mkpath (fileInfo.dir().path())) {
            qDebug() << "Cannot create directory: " << fileInfo.dir().path();
            continue;
        }

        // now writing file
        bool writeSuccess = false;
        dtkSmartPointer<dtkAbstractDataWriter> dataWriter = getSuitableWriter(fileInfo.filePath(), imData);
        if( dataWriter) {
            dataWriter->setData (imData);
            if (dataWriter->write( fileInfo.filePath() )){
                dtkDataList.push_back (imData);
                writeSuccess = true;
            }
            dataWriter = NULL;
        }

        if (!writeSuccess){
            emit showError(this, tr ("Could not save data file: ") + it.value()[0],5000);
            continue;
        }

        // Now, populate the database
        if (imData) {
            QFileInfo seriesInfo( it.key() );
            this->popupateDatabase(imData, &seriesInfo);
            imData = NULL;
        }
    } // END if Image write loop


    emit progressed(this,100);
    emit success(this);

}

//-----------------------------------------------------------------------------------------------------------

void medDatabaseImporter::onCancel( QObject* )
{
    d->isCancelled = true;
}

//-----------------------------------------------------------------------------------------------------------

QString medDatabaseImporter::populateMissingMetadata( dtkAbstractData * dtkdata, const QFileInfo * fileInfo )
{
    if (!dtkdata)
    {
        qWarning() << "data invalid";
        return "invalid";
    }

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_PatientName()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_PatientName(), QStringList() << "John Doe");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_StudyDescription()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_StudyDescription(), QStringList() << "EmptyStudy");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_SeriesDescription()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_SeriesDescription(), QStringList() << fileInfo->baseName());

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_StudyID()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_StudyID(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_SeriesID()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_SeriesID(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_Orientation()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_Orientation(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_SeriesNumber()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_SeriesNumber(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_SequenceName()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_SequenceName(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_SliceThickness()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_SliceThickness(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_Rows()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_Rows(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_Columns()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_Columns(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_Age()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_Age(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_BirthDate()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_BirthDate(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_Gender()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_Gender(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_Description()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_Description(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_Modality()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_Modality(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_Protocol()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_Protocol(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_Comments()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_Comments(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_Status()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_Status(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_AcquisitionDate()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_AcquisitionDate(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_ImportationDate()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_ImportationDate(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_Referee()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_Referee(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_Performer()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_Performer(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_Institution()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_Institution(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_Report()))
        dtkdata->addMetaData(medMetaDataHelper::KEY_Report(), QStringList() << "");

    // Query all information from the dtkAbstractData metadata.
    // This information will then be parsed to the database.
    QString patientName = dtkdata->metaDataValues(medMetaDataHelper::KEY_PatientName())[0];
    QString studyName   = dtkdata->metaDataValues(medMetaDataHelper::KEY_StudyDescription())[0];
    QString seriesName  = dtkdata->metaDataValues(medMetaDataHelper::KEY_SeriesDescription())[0];

    QString studyId = dtkdata->metaDataValues(medMetaDataHelper::KEY_StudyID())[0];
    QString seriesId = dtkdata->metaDataValues(medMetaDataHelper::KEY_SeriesID())[0];
    QString orientation = dtkdata->metaDataValues(medMetaDataHelper::KEY_Orientation())[0]; // orientation sometimes differ by a few digits, thus this is not reliable
    QString seriesNumber = dtkdata->metaDataValues(medMetaDataHelper::KEY_SeriesNumber())[0];
    QString sequenceName = dtkdata->metaDataValues(medMetaDataHelper::KEY_SequenceName())[0];
    QString sliceThickness = dtkdata->metaDataValues(medMetaDataHelper::KEY_SliceThickness())[0];
    QString rows = dtkdata->metaDataValues(medMetaDataHelper::KEY_Rows())[0];
    QString columns = dtkdata->metaDataValues(medMetaDataHelper::KEY_Columns())[0];

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
    // we use: patientName, studyID, seriesID, orientation, seriesNumber, sequenceName, sliceThickness, rows, columns. 
    // All images of the same volume should share similar values of these parameters
    QString key = patientName+studyId+seriesId+orientation+seriesNumber+sequenceName+sliceThickness+rows+columns;

    return key;

}

//-----------------------------------------------------------------------------------------------------------

bool medDatabaseImporter::checkIfExists( dtkAbstractData* dtkdata, const QFileInfo * fileInfo  )
{
    bool imageExists = false;

    QSqlQuery query(*(medDatabaseController::instance()->database()));

    QString patientName = dtkdata->metaDataValues(medMetaDataHelper::KEY_PatientName())[0];
    QString studyName   = dtkdata->metaDataValues(medMetaDataHelper::KEY_StudyDescription())[0];
    QString seriesName  = dtkdata->metaDataValues(medMetaDataHelper::KEY_SeriesDescription())[0];

    QString studyId = dtkdata->metaDataValues(medMetaDataHelper::KEY_StudyID())[0];
    QString seriesId = dtkdata->metaDataValues(medMetaDataHelper::KEY_SeriesID())[0];
    QString orientation = dtkdata->metaDataValues(medMetaDataHelper::KEY_Orientation())[0]; // orientation sometimes differ by a few digits, thus this is not reliable
    QString seriesNumber = dtkdata->metaDataValues(medMetaDataHelper::KEY_SeriesNumber())[0];
    QString sequenceName = dtkdata->metaDataValues(medMetaDataHelper::KEY_SequenceName())[0];
    QString sliceThickness = dtkdata->metaDataValues(medMetaDataHelper::KEY_SliceThickness())[0];
    QString rows = dtkdata->metaDataValues(medMetaDataHelper::KEY_Rows())[0];
    QString columns = dtkdata->metaDataValues(medMetaDataHelper::KEY_Columns())[0];

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

void medDatabaseImporter::popupateDatabase( dtkAbstractData* dtkdata, const QFileInfo * seriesInfo )
{

    QString patientName = dtkdata->metaDataValues(medMetaDataHelper::KEY_PatientName())[0].simplified();
    QString studyName   = dtkdata->metaDataValues(medMetaDataHelper::KEY_StudyDescription())[0].simplified();
    QString seriesName  = dtkdata->metaDataValues(medMetaDataHelper::KEY_SeriesDescription())[0].simplified();

    QString studyId        = dtkdata->metaDataValues(medMetaDataHelper::KEY_StudyID())[0];
    QString seriesId       = dtkdata->metaDataValues(medMetaDataHelper::KEY_SeriesID())[0];
    int size               = dtkdata->metaDataValues(medMetaDataHelper::KEY_Size())[0].toInt();

    QString orientation    = dtkdata->metaDataValues(medMetaDataHelper::KEY_Orientation())[0];
    QString seriesNumber   = dtkdata->metaDataValues(medMetaDataHelper::KEY_SeriesNumber())[0];
    QString sequenceName   = dtkdata->metaDataValues(medMetaDataHelper::KEY_SequenceName())[0];
    QString sliceThickness = dtkdata->metaDataValues(medMetaDataHelper::KEY_SliceThickness())[0];
    QString rows           = dtkdata->metaDataValues(medMetaDataHelper::KEY_Rows())[0];
    QString columns        = dtkdata->metaDataValues(medMetaDataHelper::KEY_Columns())[0];

    QString age            = dtkdata->metaDataValues(medMetaDataHelper::KEY_Age())[0];
    QString birthdate      = dtkdata->metaDataValues(medMetaDataHelper::KEY_BirthDate())[0];
    QString gender         = dtkdata->metaDataValues(medMetaDataHelper::KEY_Gender())[0];
    QString description    = dtkdata->metaDataValues(medMetaDataHelper::KEY_Description())[0];
    QString modality       = dtkdata->metaDataValues(medMetaDataHelper::KEY_Modality())[0];
    QString protocol       = dtkdata->metaDataValues(medMetaDataHelper::KEY_Protocol())[0];
    QString comments       = dtkdata->metaDataValues(medMetaDataHelper::KEY_Comments())[0];
    QString status         = dtkdata->metaDataValues(medMetaDataHelper::KEY_Status())[0];
    QString acqdate        = dtkdata->metaDataValues(medMetaDataHelper::KEY_AcquisitionDate())[0];
    QString importdate     = dtkdata->metaDataValues(medMetaDataHelper::KEY_ImportationDate())[0];
    QString referee        = dtkdata->metaDataValues(medMetaDataHelper::KEY_Referee())[0];
    QString performer      = dtkdata->metaDataValues(medMetaDataHelper::KEY_Performer())[0];
    QString institution    = dtkdata->metaDataValues(medMetaDataHelper::KEY_Institution())[0];
    QString report         = dtkdata->metaDataValues(medMetaDataHelper::KEY_Report())[0];

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

dtkSmartPointer<dtkAbstractDataReader> medDatabaseImporter::getSuitableReader( QStringList filename )
{
    QList<QString> readers = dtkAbstractDataFactory::instance()->readers();

    // cycle through readers to see if the last used reader can handle the file
    dtkSmartPointer<dtkAbstractDataReader> dataReader;
    for (int i=0; i<readers.size(); i++) {
        dataReader = dtkAbstractDataFactory::instance()->readerSmartPointer(readers[i]);
        if (d->lastSuccessfulReaderDescription == dataReader->description() && dataReader->canRead( filename ))
        {
            dataReader->enableTrace(true);
            dataReader->enableDeferredDeletion(false);
            return dataReader;
        }
    }

    for (int i=0; i<readers.size(); i++) {
        dataReader = dtkAbstractDataFactory::instance()->readerSmartPointer(readers[i]);
        if (dataReader->canRead( filename )){
            d->lastSuccessfulReaderDescription = dataReader->description();
            {
                dataReader->enableTrace(true);
                dataReader->enableDeferredDeletion(false);
                return dataReader;
            }
        }
    }
    qWarning() << "No suitable reader found!";
    dataReader = NULL;
    return dataReader;
}

//-----------------------------------------------------------------------------------------------------------

dtkSmartPointer<dtkAbstractDataWriter> medDatabaseImporter::getSuitableWriter( QString filename, dtkAbstractData* dtkdata )
{
    if (!dtkdata)
        return NULL;

    QList<QString> writers = dtkAbstractDataFactory::instance()->writers();
    dtkSmartPointer<dtkAbstractDataWriter> dataWriter;
    // first try with the last
    for (int i=0; i<writers.size(); i++) {
        dataWriter = dtkAbstractDataFactory::instance()->writerSmartPointer(writers[i]);
        if (d->lastSuccessfulReaderDescription == dataWriter->description()) {

            if ( dataWriter->handled().contains(dtkdata->description()) &&
                 dataWriter->canWrite( filename ) ) {

                d->lastSuccessfulWriterDescription = dataWriter->description();
                dataWriter->enableDeferredDeletion(false);
                return dataWriter;
            }
        }
    }

    // cycle all
    for (int i=0; i<writers.size(); i++) {
        dataWriter = dtkAbstractDataFactory::instance()->writerSmartPointer(writers[i]);

        if ( dataWriter->handled().contains(dtkdata->description()) &&
             dataWriter->canWrite( filename ) ) {

            d->lastSuccessfulWriterDescription = dataWriter->description();
            dataWriter->enableDeferredDeletion(false);
            return dataWriter;
        }
    }
    dataWriter = NULL;
    return NULL;
}

//-----------------------------------------------------------------------------------------------------------
