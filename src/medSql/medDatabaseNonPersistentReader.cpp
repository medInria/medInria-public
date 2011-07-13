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

#include <medAbstractDataImage.h>
#include <medMetaDataHelper.h>

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

        dtkSmartPointer<dtkAbstractData> dtkdata;

        for (int i=0; i<readers.size(); i++) {
            dtkSmartPointer<dtkAbstractDataReader> dataReader;
            dataReader = dtkAbstractDataFactory::instance()->readerSmartPointer(readers[i]);
            if (dataReader->canRead( fileInfo.filePath() )) {
                dataReader->readInformation( fileInfo.filePath() );
                dtkdata = dataReader->data();
                break;
            }
        }

        if (!dtkdata)
            continue;

        if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_PatientName()))
            dtkdata->addMetaData(medMetaDataHelper::KEY_PatientName(), QStringList() << fileInfo.baseName());

        if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_StudyDescription()))
            dtkdata->addMetaData(medMetaDataHelper::KEY_StudyDescription(), QStringList() << "EmptyStudy");

        if(!dtkdata->hasMetaData(medMetaDataHelper::KEY_SeriesDescription()))
            dtkdata->addMetaData(medMetaDataHelper::KEY_SeriesDescription(), QStringList() << fileInfo.baseName());

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


    QString patientName = dtkdata->metaDataValues(medMetaDataHelper::KEY_PatientName())[0];
    QString studyName   = dtkdata->metaDataValues(medMetaDataHelper::KEY_StudyDescription())[0];
    QString seriesName  = dtkdata->metaDataValues(medMetaDataHelper::KEY_SeriesDescription())[0];

    QString studyId = dtkdata->metaDataValues(medMetaDataHelper::KEY_StudyID())[0];
    QString seriesId = dtkdata->metaDataValues(medMetaDataHelper::KEY_SeriesID())[0];
    QString orientation = dtkdata->metaDataValues(medMetaDataHelper::KEY_Orientation())[0];
    QString seriesNumber = dtkdata->metaDataValues(medMetaDataHelper::KEY_SeriesNumber())[0];
    QString sequenceName = dtkdata->metaDataValues(medMetaDataHelper::KEY_SequenceName())[0];
    QString sliceThickness = dtkdata->metaDataValues(medMetaDataHelper::KEY_SliceThickness())[0];
    QString rows = dtkdata->metaDataValues(medMetaDataHelper::KEY_Rows())[0];
    QString columns = dtkdata->metaDataValues(medMetaDataHelper::KEY_Columns())[0];

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

        dtkSmartPointer<dtkAbstractData> imData;

        for (int i=0; i<readers.size(); i++) {
            dtkSmartPointer<dtkAbstractDataReader> dataReader;
            dataReader = dtkAbstractDataFactory::instance()->readerSmartPointer(readers[i]);

            if (dataReader->canRead( it.value() )) {

                //connect (dataReader, SIGNAL (progressed (int)), this, SLOT (setImportProgress(int)));

                if (dataReader->read( it.value() )) {
                    imData = dataReader->data();

                    if (imData) {
                        if (!imData->hasMetaData(medMetaDataHelper::KEY_FilePaths()))
                            imData->addMetaData(medMetaDataHelper::KEY_FilePaths(), it.value());

                        if (!imData->hasMetaData(medMetaDataHelper::KEY_PatientName()))
                            imData->addMetaData(medMetaDataHelper::KEY_PatientName(), QStringList() << QFileInfo (it.value()[0]).baseName());

                        if (!imData->hasMetaData(medMetaDataHelper::KEY_StudyDescription()))
                            imData->addMetaData(medMetaDataHelper::KEY_StudyDescription(), QStringList() << "EmptyStudy");

                        if (!imData->hasMetaData(medMetaDataHelper::KEY_SeriesDescription()))
                            imData->addMetaData(medMetaDataHelper::KEY_SeriesDescription(), QStringList() << QFileInfo (it.value()[0]).baseName());

                        if(!imData->hasMetaData(medMetaDataHelper::KEY_StudyID()))
                            imData->addMetaData(medMetaDataHelper::KEY_StudyID(), QStringList() << "");

                        if(!imData->hasMetaData(medMetaDataHelper::KEY_SeriesID()))
                            imData->addMetaData(medMetaDataHelper::KEY_SeriesID(), QStringList() << "");

                        if(!imData->hasMetaData(medMetaDataHelper::KEY_Orientation()))
                            imData->addMetaData(medMetaDataHelper::KEY_Orientation(), QStringList() << "");

                        if(!imData->hasMetaData(medMetaDataHelper::KEY_SeriesNumber()))
                            imData->addMetaData(medMetaDataHelper::KEY_SeriesNumber(), QStringList() << "");

                        if(!imData->hasMetaData(medMetaDataHelper::KEY_SequenceName()))
                            imData->addMetaData(medMetaDataHelper::KEY_SequenceName(), QStringList() << "");

                        if(!imData->hasMetaData(medMetaDataHelper::KEY_SliceThickness()))
                            imData->addMetaData(medMetaDataHelper::KEY_SliceThickness(), QStringList() << "");

                        if(!imData->hasMetaData(medMetaDataHelper::KEY_Rows()))
                            imData->addMetaData(medMetaDataHelper::KEY_Rows(), QStringList() << "");

                        if(!imData->hasMetaData(medMetaDataHelper::KEY_Columns()))
                            imData->addMetaData(medMetaDataHelper::KEY_Columns(), QStringList() << "");

                        imData->addMetaData ("FileName", it.key() );

                        break;
                    }
                }

                //disconnect (dataReader, SIGNAL (progressed (int)), this, SLOT (setImportProgress(int)));
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
	QString patientName = data->metaDataValues(medMetaDataHelper::KEY_PatientName())[0];
	
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
	QString studyName = data->metaDataValues(medMetaDataHelper::KEY_StudyDescription())[0];

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
    
	index = medDataIndex (medDatabaseNonPersistentController::instance()->dataSourceId(), patientId, studyId, medDatabaseNonPersistentController::instance()->seriesId(true), -1);

	QString seriesName = data->metaDataValues(medMetaDataHelper::KEY_SeriesDescription())[0];
	
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
