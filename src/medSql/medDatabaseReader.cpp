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
#include <medCore/medStorage.h>
#include <medCore/medAbstractDataImage.h>

#include <medCore/medMetaDataHelper.h>
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

dtkAbstractData *medDatabaseReader::run(void)
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


    dtkAbstractData *data = NULL;

    QList<QString> readers = dtkAbstractDataFactory::instance()->readers();

    for (int i = 0; i < readers.size(); i++) {
        dtkAbstractDataReader* dataReader = dtkAbstractDataFactory::instance()->reader(readers[i]);

        connect(dataReader, SIGNAL(progressed(int)), this, SIGNAL(progressed(int)));

        if (dataReader->canRead(filename)) {

            //qDebug() << "Reading using" << dataReader->description() << "reader";

            dataReader->read(filename);
            data = dataReader->data();
            delete dataReader;
            break;
        }

        delete dataReader;
    }

    if (data) {

       QSqlQuery seriesQuery(*(medDatabaseController::instance()->database()));
       QVariant seriesThumbnail;

        seriesQuery.prepare("SELECT thumbnail FROM series WHERE id = :seriesId");
        seriesQuery.bindValue(":seriesId", seriesId);
        if(!seriesQuery.exec())
            qDebug() << DTK_COLOR_FG_RED << seriesQuery.lastError() << DTK_NO_COLOR;

        if(seriesQuery.first()) {
            seriesThumbnail = seriesQuery.value(0);

            QString thumbPath = medStorage::dataLocation() + seriesThumbnail.toString();
            medMetaDataHelper::addSeriesThumbnail(data, thumbPath);

        }
        else {
            qWarning() << "Thumbnailpath not found";
        }



        medMetaDataHelper::addPatientName(data, patientName);
        medMetaDataHelper::addStudyDescription(data, studyName);
        medMetaDataHelper::addSeriesDescription(data, seriesName);
        medMetaDataHelper::addPatientID(data, patientId.toString());
        medMetaDataHelper::addStudyID(data, studyId.toString());
        medMetaDataHelper::addSeriesID(data, seriesId.toString());
        //medMetaDataHelper::addImageID(data, imageId.toString());
        emit success(this);
    } else {
        emit failure(this);
    }

    return data;
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
