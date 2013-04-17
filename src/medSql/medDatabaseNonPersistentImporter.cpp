/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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
#include <dtkLog/dtkLog.h>

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
    qDebug() << "DEBUG : entering medDatabaseNonPersistentImporter constructor";
    d->data = data;
    d->isCancelled = false;
    qDebug() << d->data;
}

medDatabaseNonPersistentImporter::~medDatabaseNonPersistentImporter()
{
    delete d;

    d = NULL;
}

void medDatabaseNonPersistentImporter::run()
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
    QString patientName = medMetaDataKeys::PatientName.getFirstValue(data);

    // These metadata may be missing, in which case you get an empty QString.
    QString birthdate = medMetaDataKeys::BirthDate.getFirstValue(data);
    QString patientId = medMetaDataKeys::PatientID.getFirstValue(data);

    qDebug() << "in database non persistent importer: " << patientId;

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

    // Optional, will use empty string otherwise.
    QString studyName = medMetaDataKeys::StudyDescription.getFirstValue(data);
    QString studyUid = medMetaDataKeys::StudyDicomID.getFirstValue(data);
    QString studyId = medMetaDataKeys::StudyID.getFirstValue(data);

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

    QString seriesName = medMetaDataKeys::SeriesDescription.getFirstValue(data);
    QString seriesUid = medMetaDataKeys::SeriesDicomID.getFirstValue(data);
    QString seriesId = medMetaDataKeys::SeriesID.getFirstValue(data);

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
