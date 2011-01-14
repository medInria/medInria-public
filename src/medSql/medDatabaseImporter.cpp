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
};

QMutex medDatabaseImporterPrivate::mutex;

medDatabaseImporter::medDatabaseImporter(const QString& file) : QRunnable(), d(new medDatabaseImporterPrivate)
{
    d->file = file;
}

medDatabaseImporter::~medDatabaseImporter(void)
{
    delete d;

    d = NULL;
}

void medDatabaseImporter::run(void)
{

    QString file = d->file;

    QDir dir(file);
    dir.setFilter(QDir::Files);

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

    typedef dtkAbstractDataFactory::dtkAbstractDataTypeHandler dtkAbstractDataTypeHandler;
    
    QList<dtkAbstractDataTypeHandler> readers = dtkAbstractDataFactory::instance()->readers();

    int fileCount = fileList.count();
    int fileIndex = 0;

    QMap<QString, int> keyToInt;
    int currentIndex = 1;

    foreach (QString file, fileList) {

        emit progressed(((qreal)fileIndex/(qreal)fileCount)*50.0);

        fileIndex++;

        QFileInfo fileInfo( file );

        dtkAbstractData* dtkdata = 0;

        QMutexLocker locker(&d->mutex);

        for (int i=0; i<readers.size(); i++) {
            dtkAbstractDataReader* dataReader = dtkAbstractDataFactory::instance()->reader(readers[i].first, readers[i].second);
            if (dataReader->canRead( fileInfo.filePath() )) {
	      qDebug() << "datareader is able to read : " << fileInfo.filePath() << "\n";
	      
                dataReader->readInformation( fileInfo.filePath() );
                dtkdata = dataReader->data();
		if (dtkdata)
		    qDebug() << dtkdata << "\n";
                delete dataReader;
                break;
            }
        }
        locker.unlock();

        if (!dtkdata)
            continue;

        if(!dtkdata->hasMetaData("PatientName"))
            dtkdata->addMetaData("PatientName", QStringList() << "John Doe");

        if(!dtkdata->hasMetaData("StudyDescription"))
            dtkdata->addMetaData("StudyDescription", QStringList() << "EmptyStudy");

        if(!dtkdata->hasMetaData("SeriesDescription"))
            dtkdata->addMetaData("SeriesDescription", QStringList() << fileInfo.baseName());

	if(!dtkdata->hasMetaData("StudyID"))
            dtkdata->addMetaData("StudyID", QStringList() << "");
	
	if(!dtkdata->hasMetaData("SeriesID"))
            dtkdata->addMetaData("SeriesID", QStringList() << "");

	if(!dtkdata->hasMetaData("Orientation"))
            dtkdata->addMetaData("Orientation", QStringList() << "");

	if(!dtkdata->hasMetaData("SeriesNumber"))
            dtkdata->addMetaData("SeriesNumber", QStringList() << "");

	if(!dtkdata->hasMetaData("SequenceName"))
            dtkdata->addMetaData("SequenceName", QStringList() << "");

	if(!dtkdata->hasMetaData("SliceThickness"))
            dtkdata->addMetaData("SliceThickness", QStringList() << "");

	if(!dtkdata->hasMetaData("Rows"))
            dtkdata->addMetaData("Rows", QStringList() << "");
		
	if(!dtkdata->hasMetaData("Columns"))
            dtkdata->addMetaData("Columns", QStringList() << "");

	if(!dtkdata->hasMetaData("Age"))
            dtkdata->addMetaData("Age", QStringList() << "");

	if(!dtkdata->hasMetaData("BirthDate"))
            dtkdata->addMetaData("BirthDate", QStringList() << "");

	if(!dtkdata->hasMetaData("Gender"))
            dtkdata->addMetaData("Gender", QStringList() << "");

	if(!dtkdata->hasMetaData("Description"))
            dtkdata->addMetaData("Description", QStringList() << "");

	if(!dtkdata->hasMetaData("Modality"))
            dtkdata->addMetaData("Modality", QStringList() << "");

	if(!dtkdata->hasMetaData("Protocol"))
            dtkdata->addMetaData("Protocol", QStringList() << "");

	if(!dtkdata->hasMetaData("Comments"))
            dtkdata->addMetaData("Comments", QStringList() << "");

	if(!dtkdata->hasMetaData("Status"))
            dtkdata->addMetaData("Status", QStringList() << "");

	if(!dtkdata->hasMetaData("AcquisitionDate"))
            dtkdata->addMetaData("AcquisitionDate", QStringList() << "");

	if(!dtkdata->hasMetaData("ImportationDate"))
            dtkdata->addMetaData("ImportationDate", QStringList() << "");

	if(!dtkdata->hasMetaData("Referee"))
            dtkdata->addMetaData("Referee", QStringList() << "");

	if(!dtkdata->hasMetaData("Performer"))
            dtkdata->addMetaData("Performer", QStringList() << "");

	if(!dtkdata->hasMetaData("Institution"))
            dtkdata->addMetaData("Institution", QStringList() << "");

	if(!dtkdata->hasMetaData("Report"))
            dtkdata->addMetaData("Report", QStringList() << "");


	QString patientName = dtkdata->metaDataValues(tr("PatientName"))[0];
	QString studyName   = dtkdata->metaDataValues(tr("StudyDescription"))[0];
	QString seriesName  = dtkdata->metaDataValues(tr("SeriesDescription"))[0];

	QString studyId = dtkdata->metaDataValues(tr("StudyID"))[0];
	QString seriesId = dtkdata->metaDataValues(tr("SeriesID"))[0];
	QString orientation = dtkdata->metaDataValues(tr("Orientation"))[0];
	QString seriesNumber = dtkdata->metaDataValues(tr("SeriesNumber"))[0];
	QString sequenceName = dtkdata->metaDataValues(tr("SequenceName"))[0];
	QString sliceThickness = dtkdata->metaDataValues(tr("SliceThickness"))[0];
	QString rows = dtkdata->metaDataValues(tr("Rows"))[0];
	QString columns = dtkdata->metaDataValues(tr("Columns"))[0];

	// define a unique key string to identify which volume an image belongs to.
	// we use: patientName, studyID, seriesID, orientation, seriesNumber, sequenceName, sliceThickness, rows, columns. All images of the same volume should share similar values of these parameters
	QString key = patientName+studyId+seriesId+orientation+seriesNumber+sequenceName+sliceThickness+rows+columns;
	if (!keyToInt.contains(key)) {
	    keyToInt[key] = currentIndex;
	    currentIndex++;
	}

	QSqlQuery query(*(medDatabaseController::instance()->database()));
	QVariant id;

	// we append the uniqueID at the end of the filename to have unique filenames for each volume
	QString uniqueSeriesId;
	uniqueSeriesId.setNum(keyToInt[key]);

	QString s_patientName = patientName.simplified();
	QString s_studyName   = studyName.simplified();
	QString s_seriesName  = seriesName.simplified();
		
	s_patientName.replace (0x00EA, 'e');
	s_studyName.replace   (0x00EA, 'e');
	s_seriesName.replace  (0x00EA, 'e');
	s_patientName.replace (0x00E4, 'a');
	s_studyName.replace   (0x00E4, 'a');
	s_seriesName.replace  (0x00E4, 'a');	
		
        QString imageFileName = medStorage::dataLocation() + "/" +
		s_patientName + "/" +
		s_studyName   + "/" +
		s_seriesName  + uniqueSeriesId;
	
	if (dtkdata->description() == "vtkDataMesh")
            imageFileName = imageFileName + ".vtk";
	else
            imageFileName = imageFileName + ".mha";

	// Check if PATIENT/STUDY/SERIES/IMAGE already exists in the database

	bool imageExists = false;
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
                    query.bindValue(":name", fileInfo.fileName());

                    if(!query.exec())
                        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

                    if(query.first()) {
                        imageExists = true;
                    }
                }
            }
	}
	
	if (!imageExists)
	    imagesToWriteMap[imageFileName] << fileInfo.filePath();

	delete dtkdata;
	
    }


    if (imagesToWriteMap.count()==0)
    {
      emit showError(this, tr("No compatible or new image was found"),5000);
      emit failure();
      return;
    }

    
    QMap<QString, int>::const_iterator itk = keyToInt.begin();    
    
    // read and write images in mhd format

    QList<dtkAbstractData*> dtkDataList;
    QList<dtkAbstractDataTypeHandler> writers = dtkAbstractDataFactory::instance()->writers();

    QMap<QString, QStringList>::const_iterator it = imagesToWriteMap.begin();
    
    int imagesCount = imagesToWriteMap.count();
    int imageIndex = 0;

    for (it; it!=imagesToWriteMap.end(); it++) {

        emit progressed(((qreal)imageIndex/(qreal)imagesCount)*50.0 + 50.0);

        imageIndex++;

        dtkAbstractData *imData = NULL;

        for (int i=0; i<readers.size(); i++) {
            dtkAbstractDataReader* dataReader = dtkAbstractDataFactory::instance()->reader(readers[i].first, readers[i].second);

            if (dataReader->canRead( it.value() )) {

                //connect (dataReader, SIGNAL (progressed (int)), this, SLOT (setImportProgress(int)));

                if (dataReader->read( it.value() )) {
                    imData = dataReader->data();					

                    if (imData) {
                        if (!imData->hasMetaData ("FilePaths"))
                            imData->addMetaData  ("FilePaths", it.value());

                        if (!imData->hasMetaData ("PatientName"))
                            imData->addMetaData  ("PatientName", QStringList() << "John Doe");
			
                        if (!imData->hasMetaData ("StudyDescription"))
                            imData->addMetaData  ("StudyDescription", QStringList() << "EmptyStudy");
			
                        if (!imData->hasMetaData ("SeriesDescription"))
                            imData->addMetaData  ("SeriesDescription", QStringList() << QFileInfo (it.value()[0]).baseName());

			QStringList size;
			if (dtkAbstractDataImage *imagedata = dynamic_cast<dtkAbstractDataImage*> (imData) ) {
			    size << QString::number (imagedata->zDimension() );
			}
			else {
			    size << "";
			}
			imData->setMetaData ("Size", size);

			if(!imData->hasMetaData ("StudyID"))
            		    imData->addMetaData ("StudyID", QStringList() << "");
	
			if(!imData->hasMetaData ("SeriesID"))
			    imData->addMetaData ("SeriesID", QStringList() << "");

			if(!imData->hasMetaData ("Orientation"))
			    imData->addMetaData ("Orientation", QStringList() << "");

			if(!imData->hasMetaData ("SeriesNumber"))
			    imData->addMetaData ("SeriesNumber", QStringList() << "");

			if(!imData->hasMetaData ("SequenceName"))
			    imData->addMetaData ("SequenceName", QStringList() << "");

			if(!imData->hasMetaData ("SliceThickness"))
			    imData->addMetaData ("SliceThickness", QStringList() << "");

			if(!imData->hasMetaData ("Rows"))
			    imData->addMetaData ("Rows", QStringList() << "");
	
			if(!imData->hasMetaData ("Columns"))
			    imData->addMetaData ("Columns", QStringList() << "");

			if(!imData->hasMetaData("Age"))
			    imData->addMetaData("Age", QStringList() << "");

			if(!imData->hasMetaData("BirthDate"))
			    imData->addMetaData("BirthDate", QStringList() << "");

			if(!imData->hasMetaData("Gender"))
			    imData->addMetaData("Gender", QStringList() << "");
			
			if(!imData->hasMetaData("Description"))
			    imData->addMetaData("Description", QStringList() << "");
			
			if(!imData->hasMetaData("Modality"))
			    imData->addMetaData("Modality", QStringList() << "");

			if(!imData->hasMetaData("Protocol"))
			    imData->addMetaData("Protocol", QStringList() << "");

			if(!imData->hasMetaData("Comments"))
			    imData->addMetaData("Comments", QStringList() << "");

			if(!imData->hasMetaData("Status"))
			    imData->addMetaData("Status", QStringList() << "");
			
			if(!imData->hasMetaData("AcquisitionDate"))
			    imData->addMetaData("AcquisitionDate", QStringList() << "");
			
			if(!imData->hasMetaData("ImportationDate"))
			    imData->addMetaData("ImportationDate", QStringList() << "");
			
			if(!imData->hasMetaData("Referee"))
			    imData->addMetaData("Referee", QStringList() << "");
			
			if(!imData->hasMetaData("Performer"))
			    imData->addMetaData("Performer", QStringList() << "");
			
			if(!imData->hasMetaData("Institution"))
			    imData->addMetaData("Institution", QStringList() << "");
			
			if(!imData->hasMetaData("Report"))
			    imData->addMetaData("Report", QStringList() << "");
			
                        imData->addMetaData ("FileName", it.key() );
			
                        delete dataReader;
			
                        break;
                    }
                }

                //disconnect (dataReader, SIGNAL (progressed (int)), this, SLOT (setImportProgress(int)));
                delete dataReader;
            }
        }

        if (!imData) {
             emit showError(this, tr ("Could not read data: ") + it.value()[0],5000);
            continue;
        }

        QFileInfo fileInfo (it.key());
        if (!fileInfo.dir().exists() && !medStorage::mkpath (fileInfo.dir().path())) {
	    qDebug() << "Cannot create directory: " << fileInfo.dir().path();
            continue;
        }

	int writeSuccess = 0;
	
        for (int i=0; i<writers.size(); i++)
	{
            dtkAbstractDataWriter *dataWriter = dtkAbstractDataFactory::instance()->writer(writers[i].first, writers[i].second);
	    qDebug() << "trying " << dataWriter->description();
	    
	    if (! dataWriter->handled().contains(imData->description()))
	    {
	      qDebug() << "failed with " << dataWriter->description();
	      continue;
	    }
	    
	    qDebug() << "success with " << dataWriter->description();
            dataWriter->setData (imData);

	    qDebug() << "trying to write in file : "<<it.key();
	    
            if (dataWriter->canWrite( it.key() )) {
                if (dataWriter->write( it.key() )) {
                    dtkDataList.push_back (imData);
		    writeSuccess = 1;
                    delete dataWriter;
                    break;
                }
            }
        }


	if (!writeSuccess) {
            emit showError(this, tr ("Could not save data file: ") + it.value()[0],5000);
	    continue;
	}
	

	// Now, populate the database
	if (imData) {
	  
	  dtkAbstractData *dtkdata = imData; //dtkDataList[i];

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

	/*
	QString s_age;
	if (dtkdata->hasMetaData(tr("(0010,1010)")))
	    s_age=dtkdata->metaDataValues(tr("(0010,1010)"))[0];
	*/
	
	//QString patientPath;
	//QString studyPath;
	QString seriesPath = dtkdata->metaDataValues (tr("FileName"))[0];

	QSqlQuery query(*(medDatabaseController::instance()->database()));
	QVariant id;

	// generate and save the thumbnails
	QList<QImage> &thumbnails = dtkdata->thumbnails();
	QFileInfo seriesInfo( seriesPath );
	QString thumb_dir = seriesInfo.dir().path() + "/" + seriesInfo.baseName() /*seriesName.simplified()*/ + "/";
	QStringList thumbPaths;

        if (thumbnails.count())
            if (!medStorage::mkpath (thumb_dir))
	        qDebug() << "Cannot create directory: " << thumb_dir;

	for (int j=0; j<thumbnails.count(); j++) {
	    QString thumb_name = thumb_dir + QString().setNum (j) + ".jpg";
	    thumbnails[j].save(thumb_name, "JPG");
	    thumbPaths << thumb_name;
	}

	QImage thumbnail = dtkdata->thumbnail(); // representative thumbnail for PATIENT/STUDY/SERIES
	QString thumbPath = thumb_dir + "ref.jpg";
	thumbnail.save (thumbPath, "JPG");
	
	
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
            /*
	      if (!QDir (patientPath).exists() && !this->mkpath (patientPath))
	      qDebug() << "Cannot create directory: " << patientPath;
	    */
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
	    /*else
	      query.bindValue(":thumbnail", "");
	    */
            query.exec(); id = query.lastInsertId();
	    
            //studyPath = patientPath + "/" + QString().setNum (id.toInt());
            /*
	      if (!QDir (studyPath).exists() && !this->mkpath (studyPath))
	      qDebug() << "Cannot create directory: " << studyPath;
	    */
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
	
	
	delete imData;
	imData = NULL;
	
	}
    }
    
    
    emit progressed(100);
    emit success();

}
