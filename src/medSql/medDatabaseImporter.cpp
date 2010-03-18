/* medDatabaseImporter.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jan 19 13:42:32 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Mar 18 23:17:30 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 32
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
#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>

#include <medSql/medDatabaseController.h>

class medDatabaseImporterPrivate
{
public:
    QString file;
};

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
    dir.setFilter(QDir::Files | QDir::Hidden);

    QStringList fileList;
    if (dir.exists()) {
        QDirIterator directory_walker(file, QDir::Files | QDir::Hidden, QDirIterator::Subdirectories);
	while (directory_walker.hasNext()) {
	    fileList << directory_walker.next();
	}
    }
    else
        fileList << file;

    QMap<QString, QStringList> imagesToWriteMap;

    typedef dtkAbstractDataFactory::dtkAbstractDataTypeHandler dtkAbstractDataTypeHandler;
    
    QList<dtkAbstractDataTypeHandler> readers = dtkAbstractDataFactory::instance()->readers();

    int fileCount = fileList.count();
    int fileIndex = 0;

    foreach (QString file, fileList) {

        emit progressed(((qreal)fileIndex/(qreal)fileCount)*50.0);

        fileIndex++;

        QFileInfo fileInfo( file );

        dtkAbstractData* dtkdata = 0;

        for (int i=0; i<readers.size(); i++) {            
            dtkAbstractDataReader* dataReader = dtkAbstractDataFactory::instance()->reader(readers[i].first, readers[i].second);
            if (dataReader->canRead( fileInfo.filePath() )) {
                dataReader->readInformation( fileInfo.filePath() );
                dtkdata = dataReader->data();
                delete dataReader;
                break;
            }
        }

        if (!dtkdata)
            continue;

        if(!dtkdata->hasMetaData("PatientName"))
            dtkdata->addMetaData("PatientName", QStringList() << "John Doe");

        if(!dtkdata->hasMetaData("StudyDescription"))
            dtkdata->addMetaData("StudyDescription", QStringList() << "EmptyStudy");

        if(!dtkdata->hasMetaData("SeriesDescription"))
            dtkdata->addMetaData("SeriesDescription", QStringList() << fileInfo.baseName());
	
	QString patientName = dtkdata->metaDataValues(tr("PatientName"))[0];
	QString studyName   = dtkdata->metaDataValues(tr("StudyDescription"))[0];
	QString seriesName  = dtkdata->metaDataValues(tr("SeriesDescription"))[0];

	QSqlQuery query(*(medDatabaseController::instance()->database()));
	QVariant id;

	QString imageFileName = medDatabaseController::instance()->dataLocation() + "/" + patientName.simplified() + "/" + studyName.simplified() + "/" + seriesName.simplified() + ".mhd";

	// Check if PATIENT/STUDY/SERIES/IMAGE already exists in the database

	bool imageExists = false;
	query.prepare("SELECT id FROM patient WHERE name = :name");
	query.bindValue(":name", patientName);
	if(!query.exec())
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
	
	if(query.first()) {
            id = query.value(0);

            query.prepare("SELECT id FROM study WHERE patient = :id AND name = :name");
            query.bindValue(":id", id);
            query.bindValue(":name", studyName);
            if(!query.exec())
                qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

            if(query.first()) {
                id = query.value(0);

                query.prepare("SELECT id FROM series WHERE study = :id AND name = :name");
                query.bindValue(":id", id);
                query.bindValue(":name", seriesName);
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
	    imagesToWriteMap[ imageFileName ] << fileInfo.filePath();
    }
    
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
                            imData->addMetaData("FilePaths", it.value());

                        if (!imData->hasMetaData ("PatientName"))
                            imData->addMetaData ("PatientName", QStringList() << "John Doe");
                        if (!imData->hasMetaData ("StudyDescription"))
                            imData->addMetaData ("StudyDescription", QStringList() << "EmptyStudy");
                        if (!imData->hasMetaData ("SeriesDescription"))
                            imData->addMetaData ("SeriesDescription", QStringList() << QFileInfo (it.value()[0]).baseName());

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
            qDebug() << "Could not read data: " << it.value();
            continue;
        }

        QFileInfo fileInfo (it.key());
        if (!fileInfo.dir().exists() && !medDatabaseController::instance()->mkpath (fileInfo.dir().path())) {
            qDebug() << "Cannot create directory: " << fileInfo.dir().path();
            continue;
        }

        for (int i=0; i<writers.size(); i++) {
            dtkAbstractDataWriter *dataWriter = dtkAbstractDataFactory::instance()->writer(writers[i].first, writers[i].second);
            dataWriter->setData (imData);

            if (dataWriter->canWrite( it.key() )) {
                if (dataWriter->write( it.key() )) {
                    dtkDataList.push_back (imData);
                    delete dataWriter;
                    break;
                }
            }
        }
    }

    // Now, populate the database
    
    for (int i=0; i<dtkDataList.count(); i++) {

        dtkAbstractData *dtkdata = dtkDataList[i];

      	QString patientName = dtkdata->metaDataValues(tr("PatientName"))[0].simplified();
	QString studyName   = dtkdata->metaDataValues(tr("StudyDescription"))[0].simplified();
	QString seriesName  = dtkdata->metaDataValues(tr("SeriesDescription"))[0].simplified();
	QStringList filePaths = dtkdata->metaDataValues (tr("FilePaths"));
	QString s_age;
	if (dtkdata->hasMetaData(tr("(0010,1010)")))
	    s_age=dtkdata->metaDataValues(tr("(0010,1010)"))[0];
	
	//QString patientPath;
	//QString studyPath;
	QString seriesPath = dtkdata->metaDataValues (tr("FileName"))[0];

	QSqlQuery query(*(medDatabaseController::instance()->database()));
	QVariant id;

	// generate and save the thumbnails
	QList<QImage> &thumbnails = dtkdata->thumbnails();
	QFileInfo seriesInfo( seriesPath );
	QString thumb_dir = seriesInfo.dir().path() + "/" + seriesName.simplified() + "/";
	QStringList thumbPaths;

        if (thumbnails.count())
	    if (!medDatabaseController::instance()->mkpath (thumb_dir))
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
            query.prepare("INSERT INTO patient (name, thumbnail) VALUES (:name, :thumbnail)");
            query.bindValue(":name", patientName);
	    query.bindValue(":thumbnail", thumbPath );
            query.exec(); id = query.lastInsertId();

            //patientPath = this->dataLocation() + "/" + QString().setNum (id.toInt());
            /*
	  if (!QDir (patientPath).exists() && !this->mkpath (patientPath))
	    qDebug() << "Cannot create directory: " << patientPath;
	  */
	}
	
	
	////////////////////////////////////////////////////////////////// STUDY

	query.prepare("SELECT id FROM study WHERE patient = :id AND name = :name");
	query.bindValue(":id", id);
	query.bindValue(":name", studyName);
	if(!query.exec())
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
	
	if(query.first()) {
            id = query.value(0);
            //studyPath = patientPath + "/" + QString().setNum (id.toInt());
	}
	else {
            query.prepare("INSERT INTO study (patient, name, thumbnail) VALUES (:patient, :study, :thumbnail)");
            query.bindValue(":patient", id);
            query.bindValue(":study", studyName);
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
	
	query.prepare("SELECT * FROM series WHERE study = :id AND name = :name");
	query.bindValue(":id", id);
	query.bindValue(":name", seriesName);
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

            query.prepare("INSERT INTO series (study, size, name, path, thumbnail, age) VALUES (:study, :size, :name, :path, :thumbnail, :age)");
            query.bindValue(":study", id);
            query.bindValue(":size", 1);
            query.bindValue(":name", seriesName);
            query.bindValue(":path", seriesPath);
	    query.bindValue(":thumbnail", thumbPath );
	    query.bindValue(":age", s_age),
            query.exec(); id = query.lastInsertId();

            //seriesPath = studyPath + "/" + QString().setNum (id.toInt()) + ".mhd";
	}
	
	
	///////////////////////////////////////////////////////////////// IMAGE

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

        delete dtkdata;
    }

    emit progressed(100);
    emit done();
}
