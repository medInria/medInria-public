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
    dtkSmartPointer<dtkAbstractData> data;
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

    // when creating empty patient or studies, we need to continue
    /*if ( !data->hasMetaData ( medMetaDataKeys::PatientName.key() ) ||
         !data->hasMetaData ( medMetaDataKeys::StudyDescription.key() ) ||
         !data->hasMetaData ( medMetaDataKeys::SeriesDescription.key() ) )
    {
        qDebug() << "metaData PatientName or StudyDescription or SeriesDescription are missing, cannot proceed";
        emit failure ( this );
        return;
    }*/

    QList<medDatabaseNonPersistentItem*> items = medDatabaseNonPersistentController::instance()->items();

    int patientDbId = -1;
    QString patientName = medMetaDataKeys::PatientName.getFirstValue(data);

    // These metadata may be missing, in which case you get an empty QString.
    QString birthdate = medMetaDataKeys::BirthDate.getFirstValue(data);
    QString patientId = medMetaDataKeys::PatientID.getFirstValue(data);
    QString gender = medMetaDataKeys::Gender.getFirstValue(data);

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
        {
            QString itemPatientName = medDatabaseNonPersistentController::instance()->metaData(items[i]->index(),medMetaDataKeys::PatientName.key() );

            if ( itemPatientName == patientName )
            {
                patientDbId = items[i]->index().patientId();
                break;
            }
        }
    }

    if ( patientDbId==-1 )
    {
        patientDbId = medDatabaseNonPersistentController::instance()->patientId ( true );

        // create an item for patient
        medDatabaseNonPersistentItem *item = new medDatabaseNonPersistentItem;
        medDataIndex index = medDataIndex ( medDatabaseNonPersistentController::instance()->dataSourceId(), patientDbId, -1, -1, -1 );

        if ( !patientName.isEmpty() )
            item->d->name = patientName;
        else
            item->d->name = "John Doe";

        //dtkSmartPointer<dtkAbstractData> dtkData = dtkAbstractDataFactory::instance()->createSmartPointer("itkDataImageUChar3");
        dtkSmartPointer<medAbstractData> dtkData = new medAbstractData();

        dtkData->addMetaData ( medMetaDataKeys::PatientName.key(), QStringList() <<  item->d->name );
        dtkData->addMetaData ( medMetaDataKeys::BirthDate.key(), birthdate );
        dtkData->addMetaData ( medMetaDataKeys::Gender.key(), gender );
       
        /*dtkAbstractData dtkData;
        dtkData.addMetaData ( medMetaDataKeys::PatientName.key(), QStringList() <<  item->d->name );
        dtkData.addMetaData ( medMetaDataKeys::BirthDate.key(), birthdate );
        dtkData.addMetaData ( medMetaDataKeys::Gender.key(), gender );*/

        item->d->patientId = patientId;
        item->d->index = index;
        item->d->data = dtkData;

        medDatabaseNonPersistentController::instance()->insert ( index, item );
        
        emit nonPersistentImported ( index, d->callerUuid );
    }

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
        {
            QString itemPatientName = medDatabaseNonPersistentController::instance()->metaData(items[i]->index(),medMetaDataKeys::PatientName.key() );
            QString itemStudyName = medDatabaseNonPersistentController::instance()->metaData(items[i]->index(),medMetaDataKeys::StudyDescription.key() );

            if ( itemPatientName == patientName && itemStudyName == studyName )
            {
                studyDbId = items[i]->index().studyId();
                break;
            }
        }
    }

    int     serieDbId   = -1;
    QString seriesName = medMetaDataKeys::SeriesDescription.getFirstValue(data);
    QString seriesUid = medMetaDataKeys::SeriesDicomID.getFirstValue(data);
    QString seriesId = medMetaDataKeys::SeriesID.getFirstValue(data);

    if ( studyDbId==-1 && ( !studyName.isEmpty() ) )
    {
        //the study is defined and is not in the db, let's generate a new id
        studyDbId = medDatabaseNonPersistentController::instance()->studyId ( true );

        // create an item for study
        medDatabaseNonPersistentItem *item = new medDatabaseNonPersistentItem;
        medDataIndex index = medDataIndex ( medDatabaseNonPersistentController::instance()->dataSourceId(), patientDbId, studyDbId, -1, -1 );

        if ( !patientName.isEmpty() )
            item->d->name = patientName;
        else
            item->d->name = "John Doe";

        item->d->patientId = patientId;
        item->d->index = index;
        item->d->studyName = studyName;

        //dtkSmartPointer<dtkAbstractData> dtkData = dtkAbstractDataFactory::instance()->createSmartPointer("itkDataImageUChar3");
        dtkSmartPointer<medAbstractData> dtkData = new medAbstractData();

        dtkData->addMetaData ( medMetaDataKeys::PatientName.key(), QStringList() << patientName );
        dtkData->addMetaData ( medMetaDataKeys::BirthDate.key(), birthdate );
        dtkData->addMetaData ( medMetaDataKeys::Gender.key(), gender );
        dtkData->addMetaData ( medMetaDataKeys::StudyDescription.key(), QStringList() << studyName );
        dtkData->addMetaData ( medMetaDataKeys::StudyID.key(), QStringList() << "0" );
        dtkData->addMetaData ( medMetaDataKeys::StudyDicomID.key(), QStringList() << "0" ); 

        item->d->data = dtkData;

        medDatabaseNonPersistentController::instance()->insert ( index, item );
        
        emit nonPersistentImported ( index, d->callerUuid );
    }

    // the study is defined
    if ( studyDbId!=-1 && !seriesName.isEmpty() )
    {
        // and the serie is defined, let's generate a new id
        serieDbId = medDatabaseNonPersistentController::instance()->seriesId ( true );
    }

    index = medDataIndex ( medDatabaseNonPersistentController::instance()->dataSourceId(), patientDbId, studyDbId, serieDbId, -1 );


    if( serieDbId!=-1)
    {
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
        
        emit nonPersistentImported ( index, d->callerUuid );
    }

    emit progress ( this, 100 );
    emit success ( this );
    
}

void medDatabaseNonPersistentImporter::onCancel ( QObject* )
{
    d->isCancelled = true;
}
