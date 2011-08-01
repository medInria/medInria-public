/* medDatabaseReader.cpp --- 
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
#include "medDatabaseReader.h"
#include <medStorage.h>
#include <medAbstractDataImage.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>

class medDatabaseReaderPrivate
{
public:
    medDataIndex index;
};

medDatabaseReader::medDatabaseReader(const medDataIndex& index) : QObject(), d(new medDatabaseReaderPrivate)
{
    d->index = index;
}

medDatabaseReader::~medDatabaseReader(void)
{
    delete d;

    d = NULL;
}

dtkSmartPointer<dtkAbstractData> medDatabaseReader::run(void)
{
    QVariant patientId = d->index.patientId();
    QVariant   studyId = d->index.studyId();
    QVariant  seriesId = d->index.seriesId();
    QVariant   imageId = d->index.imageId();

    QSqlQuery query((*(medDatabaseController::instance()->database())));

    QString patientName;
    QString studyName;
    QString seriesName;

    query.prepare("SELECT name FROM patient WHERE id = :id");
    query.bindValue(":id", patientId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    if (query.first())
        patientName = query.value(0).toString();

    query.prepare("SELECT name FROM study WHERE id = :id");
    query.bindValue(":id", studyId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    if (query.first())
        studyName = query.value(0).toString();

    query.prepare("SELECT name FROM series WHERE id = :id");
    query.bindValue(":id", seriesId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    if (query.first())
        seriesName = query.value(0).toString();

    QStringList filenames;
    QString     filename;

    query.prepare("SELECT name, id, path, instance_path FROM image WHERE series = :series");
    query.bindValue(":series", seriesId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    while(query.next()) {
        filenames << query.value(2).toString();
        filename = medStorage::dataLocation() + query.value(3).toString();
    }

    dtkSmartPointer <dtkAbstractData> dtkdata =  this->readFile(filename);

    if ( dtkdata.data() && (!dtkdata.isNull()) ) {
        dtkdata->addMetaData("PatientName", patientName);
        dtkdata->addMetaData("StudyDescription",   studyName);
        dtkdata->addMetaData("SeriesDescription",  seriesName);
        emit success(this);
    } else {
        emit failure(this);
    }
    if (dtkdata.refCount() != 1)
        qWarning() << "(Run:Exit) RefCount should be 1 here: " << dtkdata.refCount();
    return dtkdata;

}

qint64 medDatabaseReader::getDataSize()
{
    QString filename = getFilePath();
    QFileInfo info(filename);
    return info.size();
}

QString medDatabaseReader::getFilePath()
{
    QVariant patientId = d->index.patientId();
    QVariant   studyId = d->index.studyId();
    QVariant  seriesId = d->index.seriesId();
    QVariant   imageId = d->index.imageId();

    QSqlQuery query((*(medDatabaseController::instance()->database())));

    QString     filename;

    query.prepare("SELECT instance_path FROM image WHERE series = :series");
    query.bindValue(":series", seriesId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    while(query.next()) {
        filename = medStorage::dataLocation() + query.value(0).toString();
    }

    return filename;
}

dtkSmartPointer<dtkAbstractData> medDatabaseReader::readFile( QString filename )
{
    dtkSmartPointer<dtkAbstractData> dtkdata;

    QList<QString> readers = dtkAbstractDataFactory::instance()->readers();

    for (int i = 0; i < readers.size(); i++) {

        dtkSmartPointer<dtkAbstractDataReader> dataReader;
        dataReader = dtkAbstractDataFactory::instance()->readerSmartPointer(readers[i]);

        connect(dataReader, SIGNAL(progressed(int)), this, SIGNAL(progressed(int)));
        if (dataReader->canRead(filename)) {
            dataReader->read(filename);
            dataReader->enableDeferredDeletion(false);
            dtkdata = dataReader->data();
            if (dtkdata.refCount() != 2)
                qWarning() << "(ReaderLoop) RefCount should be 2 here: " << dtkdata.refCount();
            break;
        }

    }

    return dtkdata;
}
