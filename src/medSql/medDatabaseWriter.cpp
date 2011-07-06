/* medDatabaseWriter.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jun 29 15:27:20 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 29 15:46:20 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 19
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabaseController.h"
#include "medDatabaseWriter.h"
#include <medCore/medStorage.h>
#include <medCore/medDataIndex.h>
#include <medCore/medAbstractDataImage.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>

class medDatabaseWriterPrivate
{
public:
    dtkAbstractData *data;
};

medDatabaseWriter::medDatabaseWriter(dtkAbstractData *data) : QObject(), d(new medDatabaseWriterPrivate)
{
    d->data = data;
}

medDatabaseWriter::~medDatabaseWriter(void)
{
    delete d;

    d = NULL;
}

medDataIndex medDatabaseWriter::run(void)
{
    if (!d->data) {
        emit failure (this);
	return medDataIndex();
    }

    if(!d->data->hasMetaData("SeriesDescription")) {
        qDebug() << "Critical: data has no SeriesDescription, cannot save it";
	emit failure (this);
	return medDataIndex();
    }
    
    // copied from medDatabaseImporter::run()
    
    if(!d->data->hasMetaData("PatientName"))
        d->data->addMetaData("PatientName", QStringList() << "John Doe");

    if(!d->data->hasMetaData("StudyDescription"))
        d->data->addMetaData("StudyDescription", QStringList() << "EmptyStudy");

    // if(!d->data->hasMetaData("SeriesDescription"))
    // d->data->addMetaData("SeriesDescription", QStringList() << fileInfo.baseName());

    if(!d->data->hasMetaData("StudyID"))
        d->data->addMetaData("StudyID", QStringList() << "");
    
    if(!d->data->hasMetaData("SeriesID"))
        d->data->addMetaData("SeriesID", QStringList() << "");

    if(!d->data->hasMetaData("Orientation"))
        d->data->addMetaData("Orientation", QStringList() << "");

    if(!d->data->hasMetaData("SeriesNumber"))
        d->data->addMetaData("SeriesNumber", QStringList() << "");

    if(!d->data->hasMetaData("SequenceName"))
        d->data->addMetaData("SequenceName", QStringList() << "");

    if(!d->data->hasMetaData("SliceThickness"))
        d->data->addMetaData("SliceThickness", QStringList() << "");

    if(!d->data->hasMetaData("Rows"))
        d->data->addMetaData("Rows", QStringList() << "");

    if(!d->data->hasMetaData("Columns"))
        d->data->addMetaData("Columns", QStringList() << "");

    if(!d->data->hasMetaData("Age"))
        d->data->addMetaData("Age", QStringList() << "");

    if(!d->data->hasMetaData("BirthDate"))
        d->data->addMetaData("BirthDate", QStringList() << "");

    if(!d->data->hasMetaData("Gender"))
        d->data->addMetaData("Gender", QStringList() << "");

    if(!d->data->hasMetaData("Description"))
        d->data->addMetaData("Description", QStringList() << "");

    if(!d->data->hasMetaData("Modality"))
        d->data->addMetaData("Modality", QStringList() << "");

    if(!d->data->hasMetaData("Protocol"))
        d->data->addMetaData("Protocol", QStringList() << "");

    if(!d->data->hasMetaData("Comments"))
        d->data->addMetaData("Comments", QStringList() << "");

    if(!d->data->hasMetaData("Status"))
        d->data->addMetaData("Status", QStringList() << "");

    if(!d->data->hasMetaData("AcquisitionDate"))
        d->data->addMetaData("AcquisitionDate", QStringList() << "");

    if(!d->data->hasMetaData("ImportationDate"))
        d->data->addMetaData("ImportationDate", QStringList() << "");

    if(!d->data->hasMetaData("Referee"))
        d->data->addMetaData("Referee", QStringList() << "");

    if(!d->data->hasMetaData("Performer"))
        d->data->addMetaData("Performer", QStringList() << "");

    if(!d->data->hasMetaData("Institution"))
        d->data->addMetaData("Institution", QStringList() << "");

    if(!d->data->hasMetaData("Report"))
        d->data->addMetaData("Report", QStringList() << "");

    if(!d->data->hasMetaData("FilePaths"))
        d->data->addMetaData("FilePaths", QStringList() << "generated with medinria");

    QString size ="";
    if (medAbstractDataImage *imagedata = dynamic_cast<medAbstractDataImage*> (d->data) )
        size = QString::number (imagedata->zDimension() );
    d->data->addMetaData ("Size", size);


    QString patientName = d->data->metaDataValues("PatientName")[0];
    QString studyName   = d->data->metaDataValues("StudyDescription")[0];
    QString seriesName  = d->data->metaDataValues("SeriesDescription")[0];

    QString studyId        = d->data->metaDataValues("StudyID")[0];
    QString seriesId       = d->data->metaDataValues("SeriesID")[0];
    QString orientation    = d->data->metaDataValues("Orientation")[0];
    QString seriesNumber   = d->data->metaDataValues("SeriesNumber")[0];
    QString sequenceName   = d->data->metaDataValues("SequenceName")[0];
    QString sliceThickness = d->data->metaDataValues("SliceThickness")[0];
    QString rows           = d->data->metaDataValues("Rows")[0];
    QString columns        = d->data->metaDataValues("Columns")[0];
    QString age            = d->data->metaDataValues("Age")[0];
    QString birthdate      = d->data->metaDataValues("BirthDate")[0];
    QString gender         = d->data->metaDataValues("Gender")[0];
    QString description    = d->data->metaDataValues("Description")[0];
    QString modality       = d->data->metaDataValues("Modality")[0];
    QString protocol       = d->data->metaDataValues("Protocol")[0];
    QString comments       = d->data->metaDataValues("Comments")[0];
    QString status         = d->data->metaDataValues("Status")[0];
    QString acqdate        = d->data->metaDataValues("AcquisitionDate")[0];
    QString importdate     = d->data->metaDataValues("ImportationDate")[0];
    QString referee        = d->data->metaDataValues("Referee")[0];
    QString performer      = d->data->metaDataValues("Performer")[0];
    QString institution    = d->data->metaDataValues("Institution")[0];
    QString report         = d->data->metaDataValues("Report")[0];


    medDatabaseControllerImpl * dbi = medDatabaseController::instance();
    QSqlQuery query(*(dbi->database()));
    QVariant id;
    
    // Check if PATIENT/STUDY/SERIES already exists in the database

    bool dataExists = false;
    
    query.prepare("SELECT id FROM patient WHERE name = :name");
    query.bindValue(":name", patientName);
    
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    
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
	    query.bindValue(":id",             id);
	    query.bindValue(":name",           seriesName);
	    query.bindValue(":seriesID",       seriesId);
	    query.bindValue(":orientation",    orientation);
	    query.bindValue(":seriesNumber",   seriesNumber);
	    query.bindValue(":sequenceName",   sequenceName);
	    query.bindValue(":sliceThickness", sliceThickness);
	    query.bindValue(":rows",           rows);
	    query.bindValue(":columns",        columns);

	    if(!query.exec())
	        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

	    if(query.first()) {
	        id = query.value(0);

		dataExists = true;
	    }
	}
    }
    
    if (dataExists) {
        qDebug() << "data is already in the database, skipping";
	emit failure (this);
	return medDataIndex();
    }


    // define a unique key string to identify which volume an image belongs to.
    // we use: patientName, studyID, seriesID, orientation, seriesNumber, sequenceName, sliceThickness, rows, columns. All images of the same volume should share similar values of these parameters
    QString key = patientName+studyId+seriesId+orientation+seriesNumber+sequenceName+sliceThickness+rows+columns;

    QString s_patientName = dbi->stringForPath( patientName );
    QString s_studyName   = dbi->stringForPath( studyName );
    QString s_seriesName  = dbi->stringForPath( seriesName );

    QString subDirName = "/" +
        s_patientName + "/" +
        s_studyName;
    QString imageFileNameBase =  subDirName + "/" +
                            s_seriesName; //  + ".mha";

    QDir dir( medStorage::dataLocation() + subDirName );
    if (!dir.exists()) {
        if ( !medStorage::mkpath(medStorage::dataLocation() + subDirName) ) {
            dtkDebug() << "Unable to create directory for images";
            emit failure (this);
            return medDataIndex();
        }
    }

    QList<QString> writers = dtkAbstractDataFactory::instance()->writers();

    int writeSuccess = 0;
    
    for (int i=0; i<writers.size(); i++)
    {
        dtkSmartPointer<dtkAbstractDataWriter> dataWriter;
        dataWriter = dtkAbstractDataFactory::instance()->writerSmartPointer(writers[i]);
        qDebug() << "trying " << dataWriter->description();
        
        if (!dataWriter->handled().contains(d->data->description()))
        {
            qDebug() << "failed with " << dataWriter->description();
	    continue;
        }
        
        qDebug() << "success with " << dataWriter->description();
	dataWriter->setData (d->data);

        QStringList extensions = dataWriter->supportedFileExtensions();
        QString extension;
        if ( extensions.isEmpty() ) {
            extension = ".mha";
        } else {
            extension = extensions[0];
        }
        QString imageFileName = imageFileNameBase + extension;
        qDebug() << "trying to write in file : "<< medStorage::dataLocation() + imageFileName;

        if (dataWriter->canWrite (medStorage::dataLocation() + imageFileName)) {
            if (dataWriter->write( medStorage::dataLocation() + imageFileName )) {
                writeSuccess = 1;
                d->data->addMetaData ("FileName", imageFileName);
                break;
            }
        }
    }

    if (!writeSuccess) {
	emit failure (this);
	return medDataIndex();
    }




    QStringList filePaths  = d->data->metaDataValues ("FilePaths");
    QString     seriesPath = d->data->metaDataValues ("FileName")[0];


    // generate and save the thumbnails
    QList<QImage> &thumbnails = d->data->thumbnails();
    
    QFileInfo   seriesInfo (seriesPath);
    QString     thumb_dir = seriesInfo.dir().path() + "/" + seriesInfo.completeBaseName() + "/";
    QStringList thumbPaths;

    if (thumbnails.count())
        if (!medStorage::mkpath (medStorage::dataLocation() + thumb_dir))
	    qDebug() << "Cannot create directory: " << thumb_dir;
    
    for (int j=0; j<thumbnails.count(); j++) {
        QString thumb_name = thumb_dir + QString().setNum (j) + ".jpg";
        thumbnails[j].save(medStorage::dataLocation() + thumb_name, "JPG");
        thumbPaths << thumb_name;
    }

    QImage  thumbnail = d->data->thumbnail(); // representative thumbnail for PATIENT/STUDY/SERIES
    QString thumbPath = thumb_dir + "ref.jpg";
    thumbnail.save (medStorage::dataLocation() + thumbPath, "JPG");
    

    // Now, populate the database

    medDataIndex index(medDatabaseController::instance()->dataSourceId());

    ////////////////////////////////////////////////////////////////// PATIENT

    query.prepare("SELECT id FROM patient WHERE name = :name");
    query.bindValue(":name", patientName);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first()) {
        id = query.value(0);
        index.setPatientId (id.toInt());
    }
    else {
        query.prepare("INSERT INTO patient (name, thumbnail, birthdate, gender) VALUES (:name, :thumbnail, :birthdate, :gender)");
	query.bindValue(":name",      patientName);
        query.bindValue(":thumbnail", thumbPath );
        query.bindValue(":birthdate", birthdate );
        query.bindValue(":gender",    gender );
	query.exec();
	id = query.lastInsertId();
	index.setPatientId (id.toInt());
    }

    ////////////////////////////////////////////////////////////////// STUDY

    query.prepare("SELECT id FROM study WHERE patient = :id AND name = :name AND uid = :studyID");
    query.bindValue(":id",      id);
    query.bindValue(":name",    studyName);
    query.bindValue(":studyID", studyId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    
    if(query.first()) {
        id = query.value(0);
        index.setStudyId (id.toInt());
    }
    else {
        query.prepare("INSERT INTO study (patient, name, uid, thumbnail) VALUES (:patient, :study, :studyID, :thumbnail)");
	query.bindValue(":patient",   id);
	query.bindValue(":study",     studyName);
        query.bindValue(":studyID",   studyId);
        query.bindValue(":thumbnail", thumbPath );
	query.exec();
	id = query.lastInsertId();
	index.setStudyId (id.toInt());
    }

    ///////////////////////////////////////////////////////////////// SERIES
    
    query.prepare("SELECT * FROM series WHERE study = :id AND name = :name AND uid = :seriesID AND orientation = :orientation AND seriesNumber = :seriesNumber AND sequenceName = :sequenceName AND sliceThickness = :sliceThickness AND rows = :rows AND columns = :columns");
    query.bindValue(":id",             id);
    query.bindValue(":name",           seriesName);
    query.bindValue(":seriesID",       seriesId);
    query.bindValue(":orientation",    orientation);
    query.bindValue(":seriesNumber",   seriesNumber);
    query.bindValue(":sequenceName",   sequenceName);
    query.bindValue(":sliceThickness", sliceThickness);
    query.bindValue(":rows",           rows);
    query.bindValue(":columns",        columns);

    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    
    if(query.first()) {
        id = query.value(0);
        index.setSeriesId (id.toInt());
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
        index.setSeriesId (id.toInt());
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

	    query.prepare  ("SELECT id FROM image WHERE series = :id AND name = :name");
	    query.bindValue(":id",   id);
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

    emit progressed(100);
    emit success(this);

    return index;
}
