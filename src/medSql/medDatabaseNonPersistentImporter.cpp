/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseNonPersistentController.h>
#include <medDatabaseController.h>
#include <medDatabaseNonPersistentItem.h>
#include <medDatabaseNonPersistentItem_p.h>
#include <medDatabaseNonPersistentImporter.h>

#include <medAbstractDataImage.h>
#include <medMetaDataKeys.h>

#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <medAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>



medDatabaseNonPersistentImporter::medDatabaseNonPersistentImporter ( const QString& file, const QString& callerUuid )
: medAbstractDatabaseImporter(file, true, callerUuid)
{
    qDebug() << "medDatabaseNonPersistentImporter created with uuid:" << this->callerUuid();
}

//-----------------------------------------------------------------------------------------------------------

medDatabaseNonPersistentImporter::medDatabaseNonPersistentImporter ( medAbstractData* medData, const QString& callerUuid )
: medAbstractDatabaseImporter(medData, true, callerUuid)
{
    qDebug() << "medDatabaseNonPersistentImporter created with uuid:" << this->callerUuid();
}

//-----------------------------------------------------------------------------------------------------------

medDatabaseNonPersistentImporter::~medDatabaseNonPersistentImporter ()
{
    
}

//-----------------------------------------------------------------------------------------------------------

QString medDatabaseNonPersistentImporter::getPatientID(QString patientName, QString birthDate)
{
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

medDataIndex medDatabaseNonPersistentImporter::populateDatabaseAndGenerateThumbnails ( medAbstractData* data, QString pathToStoreThumbnails )
{
    QPointer<medDatabaseNonPersistentControllerImpl> npdc =
            medDatabaseNonPersistentController::instance();
            
    QList<medDatabaseNonPersistentItem*> items = npdc->items();

    int     patientDbId   = -1;
    QString patientName = medMetaDataKeys::PatientName.getFirstValue(data);
    QString patientId = medMetaDataKeys::PatientID.getFirstValue(data);
    QString birthdate = medMetaDataKeys::BirthDate.getFirstValue(data);

    // check if patient is already in the persistent database
    medDataIndex databaseIndex = medDatabaseController::instance()->indexForPatient ( patientName );
    medDatabaseNonPersistentItem *patientItem = NULL;
    
    if ( databaseIndex.isValid() )
    {
        qDebug() << "Patient exists in the database, I reuse his Id";
        patientDbId = databaseIndex.patientId();
    }
    else
    {
        // check if patient is already in the non persistent database
        for ( int i=0; i<items.count(); i++ )
            //if ( items[i]->name() ==patientName )
            if ( medMetaDataKeys::PatientName.getFirstValue(items[i]->data()) == patientName )
            {
                patientDbId = items[i]->index().patientId();
                patientItem = items[i];
                break;
            }
    }

    if ( patientDbId==-1 )
    {
        patientDbId = npdc->patientId ( true );
    }

    medDataIndex index;
    
    if ( patientItem == NULL )
    {
        // create an item for patient
        patientItem = new medDatabaseNonPersistentItem;
        index = medDataIndex ( npdc->dataSourceId(), patientDbId, -1, -1, -1 );

        dtkSmartPointer<medAbstractData> medData = new medAbstractData();
        medData->addMetaData ( medMetaDataKeys::PatientName.key(), QStringList() <<  patientName );
        medData->addMetaData ( medMetaDataKeys::BirthDate.key(), birthdate );

        patientItem->d->name = patientName;
        patientItem->d->patientId = patientId;
        patientItem->d->index = index;
        patientItem->d->birthdate = birthdate;
        patientItem->d->data = medData;

        npdc->insert ( index, patientItem );
    }
    
    
    int     studyDbId   = -1;
    QString studyName = medMetaDataKeys::StudyDescription.getFirstValue(data);
    QString studyId = medMetaDataKeys::StudyID.getFirstValue(data);
    QString studyUid = medMetaDataKeys::StudyDicomID.getFirstValue(data);
    
    QString seriesName = medMetaDataKeys::SeriesDescription.getFirstValue(data);
    

    if( studyName!="EmptyStudy" || seriesName!="EmptySerie" )
    {
        // check if study is already in the persistent database
        databaseIndex = medDatabaseController::instance()->indexForStudy ( patientName, studyName );
        medDatabaseNonPersistentItem *studyItem = NULL;

        if ( databaseIndex.isValid() )
        {
            qDebug() << "Study exists in the database, I reuse its Id";
            studyDbId = databaseIndex.studyId();
        }
        else
        {
            for ( int i=0; i<items.count(); i++ )
                if ( items[i]->name()==patientName && items[i]->studyName()==studyName )
                {
                    studyDbId = items[i]->index().studyId();
                    studyItem = items[i];
                    break;
                }
        }

        if ( studyDbId==-1 )
        {
            studyDbId = npdc->studyId ( true );
        }
        if ( studyItem == NULL )
        {
            // create an item for study
            studyItem = new medDatabaseNonPersistentItem;
            index = medDataIndex ( npdc->dataSourceId(), patientDbId, studyDbId, -1, -1 );

            dtkSmartPointer<medAbstractData> medData = new medAbstractData();

            medData->addMetaData ( medMetaDataKeys::PatientName.key(), QStringList() << patientName );
            medData->addMetaData ( medMetaDataKeys::BirthDate.key(), birthdate );
            medData->addMetaData ( medMetaDataKeys::StudyDescription.key(), QStringList() << studyName );
            medData->addMetaData ( medMetaDataKeys::StudyID.key(), QStringList() << studyId );
            medData->addMetaData ( medMetaDataKeys::StudyDicomID.key(), QStringList() << studyUid );

            studyItem->d->name = patientName;
            studyItem->d->patientId = patientId;
            studyItem->d->birthdate = birthdate;
            studyItem->d->studyName = studyName;
            studyItem->d->index = index;
            studyItem->d->data = medData;
            studyItem->d->studyId = studyId;
            studyItem->d->studyUid = studyUid;

            npdc->insert ( index, studyItem );
        }
    }
        
        
    if(seriesName != "EmptySerie")
    {
        index = medDataIndex ( npdc->dataSourceId(), patientDbId, studyDbId, npdc->seriesId ( true ), -1 );

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
    }
    
    return index;
}


//-----------------------------------------------------------------------------------------------------------

bool medDatabaseNonPersistentImporter::checkIfExists ( medAbstractData* medData, QString imageName )
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

QString medDatabaseNonPersistentImporter::ensureUniqueSeriesName ( const QString seriesName )
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
        newSeriesName = originalSeriesName + " (copy " + QString::number(suffix) + ")";
    }

    return newSeriesName;
}

//-----------------------------------------------------------------------------------------------------------

bool medDatabaseNonPersistentImporter::isPartialImportAttempt ( medAbstractData* medData )
{
    // here we check if the series we try to import is already in the database

    // if the image didn't provide basic information like patient, study or series
    // we cannot tell whether we are importing the same file twice or
    // if we want to import now a file with a path that was previously imported before
    // see http://pm-med.inria.fr/issues/292 for more details
    QString containsBasicInfo = medMetaDataKeys::ContainsBasicInfo.getFirstValue(medData);
    if (containsBasicInfo.compare("true", Qt::CaseInsensitive) != 0)
        return false;

    QPointer<medDatabaseNonPersistentControllerImpl> npdc =
        medDatabaseNonPersistentController::instance();

    QList<medDatabaseNonPersistentItem*> items = npdc->items();
    
    

            
    bool isPartialImport = false;
    foreach(medDatabaseNonPersistentItem* item, items)
    {
        isPartialImport = item->Match(medData);
        if(isPartialImport)
        {
            QString patientName = medMetaDataKeys::PatientName.getFirstValue(item->data()).simplified();
            QString studyName = medMetaDataKeys::StudyDescription.getFirstValue(item->data()).simplified();
            QString seriesName = medMetaDataKeys::SeriesDescription.getFirstValue(item->data()).simplified();
            QStringList filePaths = item->data()->metaDataValues ( medMetaDataKeys::FilePaths.key() );
            (*partialAttemptsInfo()) << ( QStringList() << patientName << studyName << seriesName << filePaths[0] );
            break;
        }
    }
    if (isPartialImport)
    {
        QString seriesName = medMetaDataKeys::SeriesDescription.getFirstValue(medData).simplified();
        QString newSeriesDescription = ensureUniqueSeriesName(seriesName);
        medData->setMetaData(medMetaDataKeys::SeriesDescription.key(), QStringList()<< newSeriesDescription );
    }
   
    return isPartialImport;
}








