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

#include <medAbstractDataImage.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>

class medDatabaseNonPersistentImporterPrivate
{
public:
    medDatabaseNonPersistentImporterPrivate ( const QString& uuid ) :
            callerUuid ( uuid ) {}
    dtkAbstractData *data;
    QString callerUuid;
    bool isCancelled;
};

medDatabaseNonPersistentImporter::medDatabaseNonPersistentImporter ( dtkAbstractData *data,
        const QString& callerUuid )
        : medJobItem(), d ( new medDatabaseNonPersistentImporterPrivate ( callerUuid ) )
{
    d->data = data;
    d->isCancelled = false;
    qDebug() << d->data;
}

medDatabaseNonPersistentImporter::~medDatabaseNonPersistentImporter ( void )
{
    delete d;

    d = NULL;
}

void medDatabaseNonPersistentImporter::run ( void )
{
    medDataIndex index;

    dtkAbstractData *data = d->data;
    if ( !data )
    {
        emit failure ( this );
        return;
    }

    if ( !data->hasMetaData ( medMetaDataKeys::PatientName.key() ) ||
            !data->hasMetaData ( medMetaDataKeys::StudyDescription.key() ) ||
            !data->hasMetaData ( medMetaDataKeys::SeriesDescription.key() ) )
    {
        qDebug() << "metaData PatientName or StudyDescription or SeriesDescription are missing, cannot proceed";
        emit failure ( this );
        return;
    }

    QList<medDatabaseNonPersistentItem*> items = medDatabaseNonPersistentController::instance()->items();

    int patientDbId = -1;
    QString patientName = data->metaDataValues ( medMetaDataKeys::PatientName.key() ) [0];
    QString birthdate = data->metaDataValues ( medMetaDataKeys::BirthDate.key() ) [0];
    QString patientId = data->metaDataValues ( medMetaDataKeys::PatientID.key() ) [0];

    // check if patient is already in the persistent database
    medDataIndex databaseIndex = medDatabaseController::instance()->indexForPatient ( patientName );
    if ( databaseIndex.isValid() )
    {
        qDebug() << "Patient exists in the database, I reuse his Id";
        patientDbId = databaseIndex.patientId();
    }
    else
    {
        for ( int i=0; i<items.count(); i++ )
            if ( items[i]->name() ==patientName )
            {
                patientDbId = items[i]->index().patientId();
                break;
            }
    }

    if ( patientDbId==-1 )
        patientDbId = medDatabaseNonPersistentController::instance()->patientId ( true );

    int     studyDbId   = -1;
    QString studyName = data->metaDataValues ( medMetaDataKeys::StudyDescription.key() ) [0];
    QString studyUid = data->metaDataValues ( medMetaDataKeys::StudyDicomID.key() ) [0];
    QString studyId = data->metaDataValues ( medMetaDataKeys::StudyID.key() ) [0];

    databaseIndex = medDatabaseController::instance()->indexForStudy ( patientName, studyName );
    if ( databaseIndex.isValid() )
    {
        qDebug() << "Study exists in the database, I reuse its Id";
        studyDbId = databaseIndex.studyId();
    }
    else
    {
        for ( int i=0; i<items.count(); i++ )
            if ( items[i]->name() ==patientName && items[i]->studyName() ==studyName )
            {
                studyDbId = items[i]->index().studyId();
                break;
            }
    }

    if ( studyDbId==-1 )
        studyDbId = medDatabaseNonPersistentController::instance()->studyId ( true );

    index = medDataIndex ( medDatabaseNonPersistentController::instance()->dataSourceId(), patientDbId, studyDbId, medDatabaseNonPersistentController::instance()->seriesId ( true ), -1 );

    QString seriesName = data->metaDataValues ( medMetaDataKeys::SeriesDescription.key() ) [0];
    QString seriesUid = data->metaDataValues ( medMetaDataKeys::SeriesDicomID.key() ) [0];
    QString seriesId = data->metaDataValues ( medMetaDataKeys::SeriesID.key() ) [0];

    medDatabaseNonPersistentItem *item = new medDatabaseNonPersistentItem;

    if ( !patientName.isEmpty() )
        item->d->name = patientName;
    else
        item->d->name = "John Doe";

    item->d->birthdate  = birthdate;
    item->d->patientId  = patientId;
    item->d->studyName  = studyName;
    item->d->studyId    = studyId;
    item->d->studyUid    = studyUid;
    item->d->seriesName = seriesName;
    item->d->seriesId   = seriesId;
    item->d->seriesUid   = seriesUid;
    item->d->file       = "";
    item->d->thumb      = data->thumbnail();
    item->d->index      = index;
    item->d->data       = data;

    medDatabaseNonPersistentController::instance()->insert ( index, item );

    emit progress ( this, 100 );
    emit success ( this );
    emit nonPersistentImported ( index, d->callerUuid );
}

void medDatabaseNonPersistentImporter::onCancel ( QObject* )
{
    d->isCancelled = true;
}
