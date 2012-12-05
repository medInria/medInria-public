/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseController.h>
#include <medDatabaseWriter.h>
#include <medMetaDataKeys.h>
#include <medStorage.h>
#include <medDataIndex.h>
#include <medAbstractDataImage.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>

class medDatabaseWriterPrivate
{
public:
    dtkSmartPointer<dtkAbstractData> data;
    QString callerUuid;
    bool isCancelled;
};

medDatabaseWriter::medDatabaseWriter(dtkAbstractData *data, const QString &callerUuid) : medJobItem(), d(new medDatabaseWriterPrivate)
{
    d->data = data;
    d->callerUuid = callerUuid;
    d->isCancelled = false;
}

medDatabaseWriter::~medDatabaseWriter()
{
    delete d;

    d = NULL;
}

void medDatabaseWriter::run()
{
    if ( !d->data )
    {
        emit failure ( this );
        return;
    }

    //test GPR pour essayer de créer des patients vides
    /*if ( !d->data->hasMetaData ( medMetaDataKeys::SeriesDescription.key() ) )
    {
        qDebug() << "Critical: data has no SeriesDescription, cannot save it";
        emit failure ( this );
        return;
    }*/

    // copied from medDatabaseImporter::run()

    if ( !d->data->hasMetaData ( medMetaDataKeys::PatientName.key() ) )
        d->data->addMetaData ( medMetaDataKeys::PatientName.key(), QStringList() << "John Doe" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::BirthDate.key() ) )
        d->data->addMetaData ( medMetaDataKeys::BirthDate.key(), QStringList() << "" );

    QString generatedPatientID = QUuid::createUuid().toString().replace ( "{","" ).replace ( "}","" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::PatientID.key() ) )
        d->data->addMetaData ( medMetaDataKeys::PatientID.key(), QStringList() << generatedPatientID );

    if ( !d->data->hasMetaData ( medMetaDataKeys::StudyDescription.key() ) )
        d->data->addMetaData ( medMetaDataKeys::StudyDescription.key(), QStringList() << "EmptyStudy" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::StudyID.key() ) )
        d->data->addMetaData ( medMetaDataKeys::StudyID.key(), QStringList() << "0" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::StudyDicomID.key() ) )
        d->data->addMetaData ( medMetaDataKeys::StudyDicomID.key(), QStringList() << "0" );

    QString generatedSeriesID = QUuid::createUuid().toString().replace ( "{","" ).replace ( "}","" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::SeriesID.key() ) )
        d->data->addMetaData ( medMetaDataKeys::SeriesID.key(), QStringList() << generatedSeriesID );

    if ( !d->data->hasMetaData ( medMetaDataKeys::SeriesDicomID.key() ) )
        d->data->addMetaData ( medMetaDataKeys::SeriesDicomID.key(), QStringList() << generatedSeriesID );

    if ( !d->data->hasMetaData ( medMetaDataKeys::Orientation.key() ) )
        d->data->addMetaData ( medMetaDataKeys::Orientation.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::SeriesNumber.key() ) )
        d->data->addMetaData ( medMetaDataKeys::SeriesNumber.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::SequenceName.key() ) )
        d->data->addMetaData ( medMetaDataKeys::SequenceName.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::SliceThickness.key() ) )
        d->data->addMetaData ( medMetaDataKeys::SliceThickness.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::Rows.key() ) )
        d->data->addMetaData ( medMetaDataKeys::Rows.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::Columns.key() ) )
        d->data->addMetaData ( medMetaDataKeys::Columns.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::Age.key() ) )
        d->data->addMetaData ( medMetaDataKeys::Age.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::BirthDate.key() ) )
        d->data->addMetaData ( medMetaDataKeys::BirthDate.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::Gender.key() ) )
        d->data->addMetaData ( medMetaDataKeys::Gender.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::Description.key() ) )
        d->data->addMetaData ( medMetaDataKeys::Description.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::Modality.key() ) )
        d->data->addMetaData ( medMetaDataKeys::Modality.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::Protocol.key() ) )
        d->data->addMetaData ( medMetaDataKeys::Protocol.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::Comments.key() ) )
        d->data->addMetaData ( medMetaDataKeys::Comments.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::Status.key() ) )
        d->data->addMetaData ( medMetaDataKeys::Status.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::AcquisitionDate.key() ) )
        d->data->addMetaData ( medMetaDataKeys::AcquisitionDate.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::ImportationDate.key() ) )
        d->data->addMetaData ( medMetaDataKeys::ImportationDate.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::Referee.key() ) )
        d->data->addMetaData ( medMetaDataKeys::Referee.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::Performer.key() ) )
        d->data->addMetaData ( medMetaDataKeys::Performer.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::Institution.key() ) )
        d->data->addMetaData ( medMetaDataKeys::Institution.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::Report.key() ) )
        d->data->addMetaData ( medMetaDataKeys::Report.key(), QStringList() << "" );

    if ( !d->data->hasMetaData ( medMetaDataKeys::FilePaths.key() ) )
        d->data->addMetaData ( medMetaDataKeys::FilePaths.key(), QStringList() << "generated with medInria" );

    QString size ="";
    if ( medAbstractDataImage *imagedata = dynamic_cast<medAbstractDataImage*> ( d->data.data() ) )
        size = QString::number ( imagedata->zDimension() );
    d->data->addMetaData ( medMetaDataKeys::Size.key(), size );

    QString patientName = medMetaDataKeys::PatientName.getFirstValue(d->data);
    QString patientId   = medMetaDataKeys::PatientID.getFirstValue(d->data);
    QString studyName   = medMetaDataKeys::StudyDescription.getFirstValue(d->data);
    QString seriesName  = medMetaDataKeys::SeriesDescription.getFirstValue(d->data);

    QString studyId        = medMetaDataKeys::StudyID.getFirstValue(d->data);
    QString studyUid       = medMetaDataKeys::StudyDicomID.getFirstValue(d->data);

    QString seriesId       = medMetaDataKeys::SeriesID.getFirstValue(d->data);
    QString seriesUid      = medMetaDataKeys::SeriesDicomID.getFirstValue(d->data);
    QString orientation    = medMetaDataKeys::Orientation.getFirstValue(d->data);
    QString seriesNumber   = medMetaDataKeys::SeriesNumber.getFirstValue(d->data);
    QString sequenceName   = medMetaDataKeys::SequenceName.getFirstValue(d->data);
    QString sliceThickness = medMetaDataKeys::SliceThickness.getFirstValue(d->data);
    QString rows           = medMetaDataKeys::Rows.getFirstValue(d->data);
    QString columns        = medMetaDataKeys::Columns.getFirstValue(d->data);
    QString age            = medMetaDataKeys::Age.getFirstValue(d->data);
    QString birthdate      = medMetaDataKeys::BirthDate.getFirstValue(d->data);
    QString gender         = medMetaDataKeys::Gender.getFirstValue(d->data);
    QString description    = medMetaDataKeys::Description.getFirstValue(d->data);
    QString modality       = medMetaDataKeys::Modality.getFirstValue(d->data);
    QString protocol       = medMetaDataKeys::Protocol.getFirstValue(d->data);
    QString comments       = medMetaDataKeys::Comments.getFirstValue(d->data);
    QString status         = medMetaDataKeys::Status.getFirstValue(d->data);
    QString acqdate        = medMetaDataKeys::AcquisitionDate.getFirstValue(d->data);
    QString importdate     = medMetaDataKeys::ImportationDate.getFirstValue(d->data);
    QString referee        = medMetaDataKeys::Referee.getFirstValue(d->data);
    QString performer      = medMetaDataKeys::Performer.getFirstValue(d->data);
    QString institution    = medMetaDataKeys::Institution.getFirstValue(d->data);
    QString report         = medMetaDataKeys::Report.getFirstValue(d->data);

    //foreach ( QString metadata, d->data->metaDataList() )
    //  qDebug() << d->data->metaDataValues ( metadata );


    medDatabaseControllerImpl * dbi = medDatabaseController::instance();
    QSqlQuery query ( * ( dbi->database() ) );

    QVariant id;

    // Check if PATIENT/STUDY/SERIES already exists in the database

    bool dataExists = false;

    do {
        dataExists = false;
        query.prepare ( "SELECT id, patientId FROM patient WHERE name = :name AND birthdate = :birthdate" );
        query.bindValue ( ":name", patientName );
        query.bindValue ( ":birthdate", birthdate );

        if ( !query.exec() )
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

        if ( query.first() )
        {
            id = query.value ( 0 );
            patientId = query.value ( 1 ).toString();

            query.prepare ( "SELECT id FROM study WHERE patient = :id AND name = :name AND uid = :uid" );
            query.bindValue ( ":id", id );
            query.bindValue ( ":name", studyName );
            query.bindValue ( ":uid", studyUid );
            if ( !query.exec() )
                qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

            if ( query.first() )
            {
                id = query.value ( 0 );

                query.prepare ( "SELECT id, seriesId FROM series WHERE study = :id AND name = :name AND uid = :uid AND orientation = :orientation AND seriesNumber = :seriesNumber AND sequenceName = :sequenceName AND sliceThickness = :sliceThickness AND rows = :rows AND columns = :columns" );
                query.bindValue ( ":id",             id );
                query.bindValue ( ":name",           seriesName );
                query.bindValue ( ":uid",            seriesUid );
                query.bindValue ( ":orientation",    orientation );
                query.bindValue ( ":seriesNumber",   seriesNumber );
                query.bindValue ( ":sequenceName",   sequenceName );
                query.bindValue ( ":sliceThickness", sliceThickness );
                query.bindValue ( ":rows",           rows );
                query.bindValue ( ":columns",        columns );

                if ( !query.exec() )
                    qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

                if ( query.first() )
                {
                    id = query.value ( 0 );
                    seriesId = query.value ( 1 ).toString();

                    qDebug() << "Series ID: " << seriesId;

                    dataExists = true;
                }
            }
        }

        if (dataExists)
        {
            seriesName += "-1";
            d->data->setMetaData(medMetaDataKeys::SeriesDescription.key(),seriesName);
            qDebug() << "Already exists, new series name" << seriesName;
        }
    }
    while (dataExists);

    if ( dataExists )
    {
        qDebug() << "data is already in the database, skipping";
        emit failure ( this );
        return;
    }

    // define a unique key string to identify which volume an image belongs to.
    // we use: patientName, studyID, seriesID, orientation, seriesNumber, sequenceName, sliceThickness, rows, columns. All images of the same volume should share similar values of these parameters
    QString key = patientName+studyId+seriesId+orientation+seriesNumber+sequenceName+sliceThickness+rows+columns;

    QString subDirName = "/" + patientId;
    QString imageFileNameBase =  subDirName + "/" +  seriesId;

    QDir dir ( medStorage::dataLocation() + subDirName );
    if ( !dir.exists() )
    {
        if ( !medStorage::mkpath ( medStorage::dataLocation() + subDirName ) )
        {
            dtkDebug() << "Unable to create directory for images";
            emit failure ( this );
            return ;
        }
    }

    QList<QString> writers = dtkAbstractDataFactory::instance()->writers();

    int writeSuccess = 0;

    // Trick similar to medDatabaseImporter
    QString identifier = d->data->identifier();
    QString extension = "";

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
        //.mha stored rgb data as vector data
        //we can't know whether it's a vector field or colred data if mha.
        if (identifier.contains("RGB")) {
            extension = ".nii";
        }
        else extension = ".mha";
    }

    for ( int i=0; i<writers.size(); i++ )
    {
        dtkSmartPointer<dtkAbstractDataWriter> dataWriter;
        dataWriter = dtkAbstractDataFactory::instance()->writerSmartPointer(writers[i]);
        qDebug() << "trying " << dataWriter->identifier();

        if (!dataWriter->handled().contains(d->data->identifier())) {
            qDebug() << "failed with " << dataWriter->identifier();
            continue;
        }

        qDebug() << "success with " << dataWriter->identifier();
        dataWriter->setData (d->data);

        // This is what should be done but for now writers come in a wrong order making analyze the default (and it doesn't handle well orientation matrices
        /*
        QStringList extensions = dataWriter->supportedFileExtensions();
        QString extension;
        if ( extensions.isEmpty() )
        {
            if (d->data->identifier().contains("Mesh"))
                extension = ".vtk";
            else
                extension = ".mha";
        }
        else
        {
            extension = extensions[0];
        }
        */

        QString imageFileName = imageFileNameBase + extension;
        qDebug() << "trying to write in file : "<< medStorage::dataLocation() + imageFileName;

        if ( dataWriter->canWrite ( medStorage::dataLocation() + imageFileName ) )
        {
            if ( dataWriter->write ( medStorage::dataLocation() + imageFileName ) )
            {
                writeSuccess = 1;
                d->data->addMetaData ( "FileName", imageFileName );
                break;
            }
        }
    }

    //test GPR
    /*if ( !writeSuccess )
    {
        emit failure ( this );
        return;
    }*/
    QStringList filePaths;
    QString     seriesPath = "";
    QString thumbPath = "";
    QStringList thumbPaths;

    if ( writeSuccess )
    {

    /*QStringList*/ filePaths  = d->data->metaDataValues ( medMetaDataKeys::FilePaths.key() );
    /*QString */    seriesPath = d->data->metaDataValues ( "FileName" ) [0];

    // generate and save the thumbnails
    QList<QImage> &thumbnails = d->data->thumbnails();

    QFileInfo   seriesInfo ( seriesPath );
    QString     thumb_dir = seriesInfo.dir().path() + "/" + seriesInfo.completeBaseName() + "/";
    /*QStringList thumbPaths;*/

    if ( thumbnails.count() )
        if ( !medStorage::mkpath ( medStorage::dataLocation() + thumb_dir ) )
            qDebug() << "Cannot create directory: " << thumb_dir;

    for ( int j=0; j<thumbnails.count(); j++ )
    {
        QString thumb_name = thumb_dir + QString().setNum ( j ) + ".jpg";
        thumbnails[j].save ( medStorage::dataLocation() + thumb_name, "JPG" );
        thumbPaths << thumb_name;
    }

    QImage  thumbnail = d->data->thumbnail(); // representative thumbnail for PATIENT/STUDY/SERIES
    /*QString*/ thumbPath = thumb_dir + "ref.jpg";
    thumbnail.save ( medStorage::dataLocation() + thumbPath, "JPG" );

    }

    // Now, populate the database
    medDataIndex index ( medDatabaseController::instance()->dataSourceId(), medDataIndex::NOT_VALID,
                         medDataIndex::NOT_VALID, medDataIndex::NOT_VALID, medDataIndex::NOT_VALID );

    ////////////////////////////////////////////////////////////////// PATIENT
    query.prepare ( "SELECT id FROM patient WHERE name = :name AND birthdate = :birthdate AND patientId = :patientId");
    query.bindValue ( ":name", patientName );
    query.bindValue ( ":birthdate", birthdate );
    query.bindValue ( ":patientId", patientId );

    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if ( query.first() )
    {
        id = query.value ( 0 );
        index.setPatientId ( id.toInt() );
    }
    else
    {
        query.prepare ( "INSERT INTO patient (name, thumbnail, birthdate, gender, patientId) VALUES (:name, :thumbnail, :birthdate, :gender, :patientId)" );
        query.bindValue ( ":name",      patientName );
        query.bindValue ( ":thumbnail", thumbPath );
        query.bindValue ( ":birthdate", birthdate );
        query.bindValue ( ":gender",    gender );
        query.bindValue ( ":patientId",  patientId );
        query.exec();
        id = query.lastInsertId();
        index.setPatientId ( id.toInt() );
    }

    ////////////////////////////////////////////////////////////////// STUDY

    query.prepare ( "SELECT id FROM study WHERE patient = :id AND name = :name AND uid = :uid" );
    query.bindValue ( ":id",      id );
    query.bindValue ( ":name",    studyName );
    query.bindValue ( ":uid",     studyUid );
    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if ( query.first() )
    {
        id = query.value ( 0 );
        index.setStudyId ( id.toInt() );
    }
    else
    {
        query.prepare ( "INSERT INTO study (patient, name, uid, thumbnail, studyId) VALUES (:patient, :study, :uid, :thumbnail, :studyId)" );
        query.bindValue ( ":patient",   id );
        query.bindValue ( ":study",     studyName );
        query.bindValue ( ":uid",       studyUid );
        query.bindValue ( ":thumbnail", thumbPath );
        query.bindValue ( ":studyId", studyId );
        query.exec();
        id = query.lastInsertId();
        index.setStudyId ( id.toInt() );
    }

    ///////////////////////////////////////////////////////////////// SERIES

    query.prepare ( "SELECT * FROM series WHERE study = :id AND name = :name AND uid = :uid AND orientation = :orientation AND seriesNumber = :seriesNumber AND sequenceName = :sequenceName AND sliceThickness = :sliceThickness AND rows = :rows AND columns = :columns" );
    query.bindValue ( ":id",             id );
    query.bindValue ( ":name",           seriesName );
    query.bindValue ( ":uid",            seriesUid );
    query.bindValue ( ":orientation",    orientation );
    query.bindValue ( ":seriesNumber",   seriesNumber );
    query.bindValue ( ":sequenceName",   sequenceName );
    query.bindValue ( ":sliceThickness", sliceThickness );
    query.bindValue ( ":rows",           rows );
    query.bindValue ( ":columns",        columns );

    if ( !query.exec() )
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if ( query.first() )
    {
        id = query.value ( 0 );
        index.setSeriesId ( id.toInt() );
    }
    else
    {
        query.prepare ( "INSERT INTO series (study, size, name, path, uid, seriesId, orientation, seriesNumber, sequenceName, sliceThickness, rows, columns, thumbnail, age, description, modality, protocol, comments, status, acquisitiondate, importationdate, referee, performer, institution, report) VALUES (:study, :size, :name, :path, :uid, :seriesId, :orientation, :seriesNumber, :sequenceName, :sliceThickness, :rows, :columns, :thumbnail, :age, :description, :modality, :protocol, :comments, :status, :acquisitiondate, :importationdate, :referee, :performer, :institution, :report)" );
        query.bindValue ( ":study",          id );
        query.bindValue ( ":size",           size );
        query.bindValue ( ":name",           seriesName );
        query.bindValue ( ":path",           seriesPath );
        query.bindValue ( ":uid",            seriesUid );
        query.bindValue ( ":seriesId",       seriesId );
        query.bindValue ( ":orientation",    orientation );
        query.bindValue ( ":seriesNumber",   seriesNumber );
        query.bindValue ( ":sequenceName",   sequenceName );
        query.bindValue ( ":sliceThickness", sliceThickness );
        query.bindValue ( ":rows",           rows );
        query.bindValue ( ":columns",        columns );
        query.bindValue ( ":thumbnail",      thumbPath );
        query.bindValue ( ":age",         age );
        query.bindValue ( ":description", description );
        query.bindValue ( ":modality",    modality );
        query.bindValue ( ":protocol",    protocol );
        query.bindValue ( ":comments",    comments );
        query.bindValue ( ":status",      status );
        query.bindValue ( ":acquisitiondate",     acqdate );
        query.bindValue ( ":importationdate",  importdate );
        query.bindValue ( ":referee",     referee );
        query.bindValue ( ":performer",   performer );
        query.bindValue ( ":institution", institution );
        query.bindValue ( ":report",      report );

        if ( !query.exec() )
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

        id = query.lastInsertId();
        index.setSeriesId ( id.toInt() );
    }

    ///////////////////////////////////////////////////////////////// IMAGE

    if ( filePaths.count() ==1 && thumbPaths.count() >1 ) // special case to 1 image and multiple thumbnails
    {
        QFileInfo fileInfo ( filePaths[0] );
        for ( int j=0; j<thumbPaths.count(); j++ )
        {
            query.prepare ( "SELECT id FROM image WHERE series = :id AND name = :name" );
            query.bindValue ( ":id", id );
            query.bindValue ( ":name", fileInfo.fileName() +QString().setNum ( j ) );

            if ( !query.exec() )
                qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

            if ( query.first() )
            {
                ; //qDebug() << "Image" << file << "already in database";
            }
            else
            {
                query.prepare ( "INSERT INTO image (series, name, path, instance_path, thumbnail) VALUES (:series, :name, :path, :instance_path, :thumbnail)" );
                query.bindValue ( ":series", id );
                query.bindValue ( ":name", fileInfo.fileName() +QString().setNum ( j ) );
                query.bindValue ( ":path", fileInfo.filePath() );
                query.bindValue ( ":instance_path", seriesPath );
                query.bindValue ( ":thumbnail", thumbPaths[j] );

                if ( !query.exec() )
                    qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
            }
        }
    }
    else
    {
        for ( int j=0; j<filePaths.count(); j++ )
        {
            QFileInfo fileInfo ( filePaths[j] );

            query.prepare ( "SELECT id FROM image WHERE series = :id AND name = :name" );
            query.bindValue ( ":id",   id );
            query.bindValue ( ":name", fileInfo.fileName() );

            if ( !query.exec() )
                qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

        if(query.first()) {
            ; //qDebug() << "Image" << file << "already in database";
        }
        else {
            query.prepare("INSERT INTO image (series, name, path, instance_path, thumbnail, isIndexed) VALUES (:series, :name, :path, :instance_path, :thumbnail, :isIndexed)");
        query.bindValue(":series", id);
        query.bindValue(":name", fileInfo.fileName());
        query.bindValue(":path", fileInfo.filePath());
        query.bindValue(":instance_path", seriesPath);
        if (j<thumbPaths.count())
            query.bindValue(":thumbnail", thumbPaths[j]);
        else
            query.bindValue(":thumbnail", "");

            query.bindValue(":isIndexed", false);

        if(!query.exec())
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        }
        }
    }

    emit progress(this, 100);
    emit success(this);

    if (d->callerUuid == "")
        emit addedIndex(index);
    else
        emit addedIndex(index,d->callerUuid);
}

void medDatabaseWriter::onCancel ( QObject* )
{
    d->isCancelled = true;
}

