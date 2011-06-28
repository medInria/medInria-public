/* medDatabaseNonPersitentReader.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jun 29 15:53:52 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 29 16:30:13 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 35
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabaseNonPersistentController.h"
#include "medDatabaseController.h"
#include "medDatabaseNonPersistentItem.h"
#include "medDatabaseNonPersistentItem_p.h"
#include "medDatabaseNonPersistentReader.h"

#include <medData/medAbstractDataImage.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>

class medDatabaseNonPersistentReaderPrivate
{
public:
    QString file;
};

medDatabaseNonPersistentReader::medDatabaseNonPersistentReader(const QString& file) : QObject(), d(new medDatabaseNonPersistentReaderPrivate)
{
    d->file = file;
}

medDatabaseNonPersistentReader::~medDatabaseNonPersistentReader(void)
{
    delete d;

    d = NULL;
}

medDataIndex medDatabaseNonPersistentReader::run(void)
{
    qDebug() << DTK_PRETTY_FUNCTION;

    if (d->file.isEmpty())
        return medDataIndex();

    QDir dir(d->file);
    dir.setFilter(QDir::Files);

    QStringList fileList;
    if (dir.exists()) {
        QDirIterator directory_walker(d->file, QDir::Files, QDirIterator::Subdirectories);
    while (directory_walker.hasNext()) {
        fileList << directory_walker.next();
    }
    }
    else
        fileList << d->file;

    fileList.sort();

    QMap<QString, QStringList> imagesToWriteMap;

    QList<QString> readers = dtkAbstractDataFactory::instance()->readers();

    int fileCount = fileList.count();
    int fileIndex = 0;

    QMap<QString, int> keyToInt;
    int currentIndex = 1;

    foreach (QString file, fileList) {

        emit progressed((int)(((qreal)fileIndex/(qreal)fileCount)*50.0));

        fileIndex++;

        QFileInfo fileInfo( file );

        dtkAbstractData* dtkdata = 0;

        for (int i=0; i<readers.size(); i++) {
            dtkAbstractDataReader* dataReader = dtkAbstractDataFactory::instance()->reader(readers[i]);
            if (dataReader->canRead( fileInfo.filePath() )) {
                dataReader->readInformation( fileInfo.filePath() );
                dtkdata = dataReader->data();
                delete dataReader;
                break;
            }
            delete dataReader;
        }

        if (!dtkdata)
            continue;

        if(!dtkdata->hasMetaData("PatientName"))
            dtkdata->addMetaData("PatientName", QStringList() << fileInfo.baseName());

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

    imagesToWriteMap[ key ] << fileInfo.filePath();
    }

    QMap<QString, int>::const_iterator itk = keyToInt.begin();
    

    // read and write images in mhd format

    QList<dtkAbstractData*> dtkDataList;

    QMap<QString, QStringList>::const_iterator it = imagesToWriteMap.begin();
    
    int imagesCount = imagesToWriteMap.count();
    int imageIndex = 0;

    for (it; it!=imagesToWriteMap.end(); it++) {

        emit progressed((int)(((qreal)imageIndex/(qreal)imagesCount)*50.0 + 50.0));

        imageIndex++;

        dtkAbstractData *imData = NULL;

        for (int i=0; i<readers.size(); i++) {
            dtkAbstractDataReader* dataReader = dtkAbstractDataFactory::instance()->reader(readers[i]);

            if (dataReader->canRead( it.value() )) {

                //connect (dataReader, SIGNAL (progressed (int)), this, SLOT (setImportProgress(int)));

                if (dataReader->read( it.value() )) {
                    imData = dataReader->data();

                    if (imData) {
                        if (!imData->hasMetaData ("FilePaths"))
                            imData->addMetaData("FilePaths", it.value());

                        if (!imData->hasMetaData ("PatientName"))
                            imData->addMetaData ("PatientName", QStringList() << QFileInfo (it.value()[0]).baseName());
            
                        if (!imData->hasMetaData ("StudyDescription"))
                            imData->addMetaData ("StudyDescription", QStringList() << "EmptyStudy");
            
                        if (!imData->hasMetaData ("SeriesDescription"))
                            imData->addMetaData ("SeriesDescription", QStringList() << QFileInfo (it.value()[0]).baseName());

            if(!imData->hasMetaData("StudyID"))
                        imData->addMetaData("StudyID", QStringList() << "");
    
            if(!imData->hasMetaData("SeriesID"))
                imData->addMetaData("SeriesID", QStringList() << "");

            if(!imData->hasMetaData("Orientation"))
                imData->addMetaData("Orientation", QStringList() << "");

            if(!imData->hasMetaData("SeriesNumber"))
                imData->addMetaData("SeriesNumber", QStringList() << "");

            if(!imData->hasMetaData("SequenceName"))
                imData->addMetaData("SequenceName", QStringList() << "");

            if(!imData->hasMetaData("SliceThickness"))
                imData->addMetaData("SliceThickness", QStringList() << "");

            if(!imData->hasMetaData("Rows"))
                imData->addMetaData("Rows", QStringList() << "");
    
            if(!imData->hasMetaData("Columns"))
                imData->addMetaData("Columns", QStringList() << "");

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

    dtkDataList.push_back (imData);

    }

    medDataIndex index;
    
    for (int i=0; i<dtkDataList.count(); i++) {

        dtkAbstractData *data = dtkDataList[i];

	QList<medDatabaseNonPersistentItem*> items = medDatabaseNonPersistentController::instance()->items();
	
	int     patientId   = -1;
	QString patientName = data->metaDataValues(tr("PatientName"))[0];
	
	// check if patient is already in the persistent database
	medDataIndex databaseIndex = medDatabaseController::instance()->indexForPatient (patientName);
	if (databaseIndex.isValid()) {
	    qDebug() << "Patient exists in the database, I reuse his Id";
	    patientId = databaseIndex.patientId();
	}
	else {
	    for (int i=0; i<items.count(); i++)
	        if (items[i]->name()==patientName) {
		    patientId = items[i]->index().patientId();
		    break;
		}
	}
	
	if (patientId==-1)
	    patientId = medDatabaseNonPersistentController::instance()->patientId(true);
	
	int     studyId   = -1;
	QString studyName = data->metaDataValues(tr("StudyDescription"))[0];

	databaseIndex = medDatabaseController::instance()->indexForStudy (patientName, studyName);
	if (databaseIndex.isValid()) {
	    qDebug() << "Study exists in the database, I reuse his Id";
	    studyId = databaseIndex.studyId();
	}
	else {
	    for (int i=0; i<items.count(); i++)
	        if (items[i]->name()==patientName && items[i]->studyName()==studyName) {
		    studyId = items[i]->index().studyId();
		    break;
		}
	}
	
	if (studyId==-1)
	    studyId = medDatabaseNonPersistentController::instance()->studyId(true);
    
	index = medDataIndex (patientId, studyId, medDatabaseNonPersistentController::instance()->seriesId(true), -1);

	QString seriesName = data->metaDataValues(tr("SeriesDescription"))[0];
	
        QFileInfo info(d->file);

	medDatabaseNonPersistentItem *item = new medDatabaseNonPersistentItem;

        if(!patientName.isEmpty())
            item->d->name = patientName;
        else
            item->d->name = info.baseName();

	item->d->studyName = studyName;
	item->d->seriesName = seriesName;
	item->d->file = d->file;
	item->d->thumb = data->thumbnail();
	item->d->index = index;
	item->d->data = data;

        medDatabaseNonPersistentController::instance()->insert(index, item);
    }

    emit success(this);

    return index;
}
