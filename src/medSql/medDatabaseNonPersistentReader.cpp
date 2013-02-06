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
#include "medDatabaseNonPersistentReader.h"

#include <medAbstractDataImage.h>
#include <medMetaDataKeys.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>



medDatabaseNonPersistentReader::medDatabaseNonPersistentReader ( const QString& file,
                                                                 const QString& callerUuid )
: medAbstractDatabaseImporter(file, true, callerUuid)
{
    qDebug() << "medDatabaseNonPersistentReader created with uuid:" << this->callerUuid();
}

//-----------------------------------------------------------------------------------------------------------

medDatabaseNonPersistentReader::~medDatabaseNonPersistentReader ( void )
{
    
}

//-----------------------------------------------------------------------------------------------------------

QString medDatabaseNonPersistentReader::getPatientID(QString patientName, QString birthDate)
{
    //TODO GPR : à reprendre 
    QPointer<medDatabaseNonPersistentControllerImpl> npdc =
            medDatabaseNonPersistentController::instance();
            
    QList<medDatabaseNonPersistentItem*> items = npdc->items();
    
    bool patientExists = false;
    QString patientID;
    
    foreach(medDatabaseNonPersistentItem* item, items)
    {
        if(item->name() == patientName && item->birthdate() == birthDate)
        {
            patientExists = true;
            patientID = item->patientId();
            break;
        }
    }
    
    if(!patientExists)
    {
        patientID = QUuid::createUuid().toString().replace("{","").replace("}",""); 
    }
    
    return patientID;
}

//-----------------------------------------------------------------------------------------------------------

medDataIndex medDatabaseNonPersistentReader::populateDatabaseAndGenerateThumbnails ( dtkAbstractData* data, QString pathToStoreThumbnails )
{
    QPointer<medDatabaseNonPersistentControllerImpl> npdc =
            medDatabaseNonPersistentController::instance();
            
    QList<medDatabaseNonPersistentItem*> items = npdc->items();

    int     patientDbId   = -1;
    QString patientName = medMetaDataKeys::PatientName.getFirstValue(data);
    QString patientId = medMetaDataKeys::PatientID.getFirstValue(data);

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
        patientDbId = npdc->patientId ( true );

    int     studyDbId   = -1;
    QString studyName = medMetaDataKeys::StudyDescription.getFirstValue(data);
    QString studyId = medMetaDataKeys::StudyID.getFirstValue(data);
    QString studyUid = medMetaDataKeys::StudyDicomID.getFirstValue(data);

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
        studyDbId = npdc->studyId ( true );

    medDataIndex index = medDataIndex ( npdc->dataSourceId(), patientDbId, studyDbId, npdc->seriesId ( true ), -1 );

    QString seriesName = medMetaDataKeys::SeriesDescription.getFirstValue(data);
    QString seriesId = medMetaDataKeys::SeriesID.getFirstValue(data);
    QString seriesUid = medMetaDataKeys::SeriesDicomID.getFirstValue(data);
    QString orientation = medMetaDataKeys::Orientation.getFirstValue(data);
    QString seriesNumber = medMetaDataKeys::SeriesNumber.getFirstValue(data);
    QString sequenceName = medMetaDataKeys::SequenceName.getFirstValue(data);
    QString sliceThickness = medMetaDataKeys::SliceThickness.getFirstValue(data);
    QString rows = medMetaDataKeys::Rows.getFirstValue(data);
    QString columns = medMetaDataKeys::Columns.getFirstValue(data);

    QFileInfo info (file() );

    medDatabaseNonPersistentItem *item = new medDatabaseNonPersistentItem;

    if ( !patientName.isEmpty() )
        item->d->name = patientName;
    else
        item->d->name = info.baseName();

    item->d->patientId = patientId;
    item->d->studyName = studyName;
    item->d->seriesName = seriesName;
    item->d->seriesId = seriesId;
    item->d->file = file();
    item->d->thumb = data->thumbnail();
    item->d->index = index;
    item->d->data = data;
    item->d->orientation = orientation;
    item->d->seriesNumber = seriesNumber;
    item->d->sequenceName = sequenceName;
    item->d->sliceThickness = sliceThickness;
    item->d->rows = rows;
    item->d->columns = columns;
    item->d->seriesUid = seriesUid;
    item->d->studyUid = studyUid;

    npdc->insert ( index, item );
    return index;
}


//-----------------------------------------------------------------------------------------------------------

bool medDatabaseNonPersistentReader::checkIfExists ( dtkAbstractData* dtkdata, QString imageName )
{
    bool imageExists = false;

    QPointer<medDatabaseNonPersistentControllerImpl> npdc =
            medDatabaseNonPersistentController::instance();
            
    QList<medDatabaseNonPersistentItem*> items = npdc->items();
    
    foreach(medDatabaseNonPersistentItem* item, items)
    {
        imageExists = item->file() == imageName;
        if (imageExists)
            break;
    }
    
    return imageExists;
}

//-----------------------------------------------------------------------------------------------------------

QString medDatabaseNonPersistentReader::ensureUniqueSeriesName ( const QString seriesName )
{
    QPointer<medDatabaseNonPersistentControllerImpl> npdc =
        medDatabaseNonPersistentController::instance();

    QList<medDatabaseNonPersistentItem*> items = npdc->items();

    QStringList seriesNames;
    foreach(medDatabaseNonPersistentItem* item, items)
    {
        QString sname = item->seriesName();
        if(sname.startsWith(seriesName) )
            seriesNames << sname;
    }

    QString originalSeriesName = seriesName;
    QString newSeriesName = seriesName;
    int suffix = 0;

    while (seriesNames.contains(newSeriesName))
    {
        // it exist
        suffix++;
        newSeriesName = originalSeriesName + "_" + QString::number(suffix);
    }

    return newSeriesName;
}

//-----------------------------------------------------------------------------------------------------------

bool medDatabaseNonPersistentReader::isPartialImportAttempt ( dtkAbstractData* dtkData )
{
    // here we check if the series we try to import is already in the database

    // if the image didn't provide basic information like patient, study or series
    // we cannot tell whether we are importing the same file twice or
    // if we want to import now a file with a path that was previously imported before
    // see http://pm-med.inria.fr/issues/292 for more details
    QString containsBasicInfo = medMetaDataKeys::ContainsBasicInfo.getFirstValue(dtkData);
    if (containsBasicInfo.compare("true", Qt::CaseInsensitive) != 0)
        return false;

    QPointer<medDatabaseNonPersistentControllerImpl> npdc =
        medDatabaseNonPersistentController::instance();

    QList<medDatabaseNonPersistentItem*> items = npdc->items();
    
    QString patientName = medMetaDataKeys::PatientName.getFirstValue(dtkData).simplified();
    QString studyName = medMetaDataKeys::StudyDescription.getFirstValue(dtkData).simplified();
    QString seriesName = medMetaDataKeys::SeriesDescription.getFirstValue(dtkData).simplified();
    QStringList filePaths = dtkData->metaDataValues ( medMetaDataKeys::FilePaths.key() );

    bool isPartialImport;
    foreach(medDatabaseNonPersistentItem* item, items)
    {
        isPartialImport = item->Match(dtkData);
        
        if(isPartialImport)
        {
            (*partialAttemptsInfo()) << ( QStringList() << patientName << studyName << seriesName << filePaths[0] );
            break;
        }
    }
   
    return isPartialImport;
}








