/* medDatabaseNonPersitentImporter.cpp --- 
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
#include "medDatabaseNonPersistentImporter.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>

class medDatabaseNonPersistentImporterPrivate
{
public:
    dtkAbstractData *data;
};

medDatabaseNonPersistentImporter::medDatabaseNonPersistentImporter(dtkAbstractData *data) : QObject(), d(new medDatabaseNonPersistentImporterPrivate)
{
    d->data = data;
    qDebug() << d->data;
    
}

medDatabaseNonPersistentImporter::~medDatabaseNonPersistentImporter(void)
{
    delete d;

    d = NULL;
}

medDataIndex medDatabaseNonPersistentImporter::run(void)
{
    medDataIndex index;
    
    dtkAbstractData *data = d->data;
    if (!data) {
        emit failure (this);
	return medDataIndex();
    }

    if (!data->hasMetaData ("PatientName") ||
	!data->hasMetaData ("StudyDescription") ||
	!data->hasMetaData ("SeriesDescription") ) {
        qDebug() << "metaData PatientName or StudyDescription or SeriesDescription are missing, cannot proceed";
	emit failure (this);
	return medDataIndex();
    }
    
    QList<medDatabaseNonPersistentItem*> items = medDatabaseNonPersistentController::instance()->items();

    int patientId = -1;
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

    medDatabaseNonPersistentItem *item = new medDatabaseNonPersistentItem;

    if(!patientName.isEmpty())
        item->d->name = patientName;
    else
        item->d->name = "John Doe";

    item->d->studyName  = studyName;
    item->d->seriesName = seriesName;
    item->d->file       = "";
    item->d->thumb      = data->thumbnail();
    item->d->index      = index;
    item->d->data       = data;

    medDatabaseNonPersistentController::instance()->insert(index, item);

    emit progressed(100);
    emit success(this);

    return index;
}
