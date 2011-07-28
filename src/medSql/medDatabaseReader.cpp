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

    query.prepare("SELECT name, id, path, instance_path, isIndexed FROM image WHERE series = :series");
    query.bindValue(":series", seriesId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    // now we might have both indexed and imported images in the same series
    // so we will get the full path for indexed images and, for the imported
    // (and possibly aggregated) images we will get the path of the
    // aggregated file (which is in instance_path)

    QStringList filenames;
    while(query.next())
    {
        bool isIndexed = query.value(4).toBool();

        if (isIndexed)
        {
            QString filename = query.value(2).toString();


            // if the file is indexed the chanced that is not there anymore are higher
            // so we check for existence and return null if they are not there anymore
            QFileInfo fileinfo(filename);

            if(!fileinfo.exists())
            {
                emit failure(this);
                return NULL;
            }

            filenames << filename;
        }
        else
        {
            filenames << medStorage::dataLocation() + query.value(3).toString();
        }
    }
    // we remove all the duplicate entries, as imported files
    // might have introduced duplicates
    filenames.removeDuplicates();

    dtkAbstractData *data = NULL;

    QList<QString> readers = dtkAbstractDataFactory::instance()->readers();

    for (int i = 0; i < readers.size(); i++) {
        dtkAbstractDataReader* dataReader = dtkAbstractDataFactory::instance()->reader(readers[i]);

        connect(dataReader, SIGNAL(progressed(int)), this, SIGNAL(progressed(int)));

        if (dataReader->canRead(filenames)) {

            //qDebug() << "Reading using" << dataReader->description() << "reader";

            dataReader->read(filenames);
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

    QString filename;

    query.prepare("SELECT path, instance_path, isIndexed FROM image WHERE series = :series");
    query.bindValue(":series", seriesId);

    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    // indexed files have an empty string in 'instance_path' column
    // and imported files have the relative path of the (aggregated) file

    while(query.next()) {
        bool isIndexed = query.value(2).toBool();

        if (isIndexed) {
            filename = query.value(0).toString();
        } else {
            filename = medStorage::dataLocation() + query.value(2).toString();
        }
    }

    return filename;
}
