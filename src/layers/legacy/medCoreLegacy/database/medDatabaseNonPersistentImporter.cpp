/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medAbstractImageData.h>
#include <medDatabaseController.h>
#include <medDatabaseNonPersistentController.h>
#include <medDatabaseNonPersistentItem.h>
#include <medDatabaseNonPersistentItem_p.h>
#include <medDatabaseNonPersistentImporter.h>
#include <medGlobalDefs.h>
#include <medMetaDataKeys.h>

medDatabaseNonPersistentImporter::medDatabaseNonPersistentImporter (const QString& file, const QUuid& uuid )
: medAbstractDatabaseImporter(file, uuid, true)
{
    qDebug() << "medDatabaseNonPersistentImporter created with uuid:" << this->callerUuid();
}

//-----------------------------------------------------------------------------------------------------------

medDatabaseNonPersistentImporter::medDatabaseNonPersistentImporter (medAbstractData* medData, const QUuid &uuid )
: medAbstractDatabaseImporter(medData, uuid, true)
{
    qDebug() << "medDatabaseNonPersistentImporter created with uuid:" << this->callerUuid();
}

//-----------------------------------------------------------------------------------------------------------
/**
* Retrieves patientID. Checks if patient is already in the database
* if so, reurns is Id, otherwise creates a new guid
*/
QString medDatabaseNonPersistentImporter::getPatientID(QString patientName, QString birthDate)
{
    QPointer<medDatabaseNonPersistentController> npdc =
            medDatabaseNonPersistentController::instance();

    QList<medDatabaseNonPersistentItem*> items = npdc->items();

    bool patientExists = false;
    QString patientID;

    for(medDatabaseNonPersistentItem* item : items)
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
/**
* Populates database tables and generates thumbnails.
* @param medData - a @medAbstractData object created from the original image
* @param pathToStoreThumbnails - path where the thumbnails will be stored
* @return medDataIndex the new medDataIndex associated with this imported series.
**/
medDataIndex medDatabaseNonPersistentImporter::populateDatabaseAndGenerateThumbnails ( medAbstractData* data, QString pathToStoreThumbnails )
{
    Q_UNUSED(pathToStoreThumbnails);
    QPointer<medDatabaseNonPersistentController> npdc =
            medDatabaseNonPersistentController::instance();

    QList<medDatabaseNonPersistentItem*> items = npdc->items();

    int     patientDbId   = -1;
    QString patientName = medMetaDataKeys::PatientName.getFirstValue(data);
    QString patientId = medMetaDataKeys::PatientID.getFirstValue(data);
    QString birthdate = medMetaDataKeys::BirthDate.getFirstValue(data);

    // check if patient is already in the persistent database
    medDataIndex databaseIndex = medDatabaseController::instance()->indexForPatient ( patientName );
    medDatabaseNonPersistentItem *patientItem = nullptr;

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

    if ( patientItem == nullptr )
    {
        // create an item for patient
        patientItem = new medDatabaseNonPersistentItem;
        index = medDataIndex ( npdc->dataSourceId(), patientDbId, -1, -1 );

        medAbstractData *medData = new medAbstractData();
        medData->setMetaData ( medMetaDataKeys::PatientName.key(), QStringList() <<  patientName );
        medData->setMetaData ( medMetaDataKeys::BirthDate.key(), birthdate );

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
    QString studyUid = medMetaDataKeys::StudyInstanceUID.getFirstValue(data);

    QString seriesName = medMetaDataKeys::SeriesDescription.getFirstValue(data);

    if( studyName!="EmptyStudy" || seriesName!="EmptySeries" )
    {
        // check if study is already in the persistent database
        databaseIndex = medDatabaseController::instance()->indexForStudy ( patientName, studyName );
        medDatabaseNonPersistentItem *studyItem = nullptr;

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
        if ( studyItem == nullptr )
        {
            // create an item for study
            studyItem = new medDatabaseNonPersistentItem;
            index = medDataIndex ( npdc->dataSourceId(), patientDbId, studyDbId, -1 );

            medAbstractData *medData = new medAbstractData();

            medData->setMetaData ( medMetaDataKeys::PatientName.key(), QStringList() << patientName );
            medData->setMetaData ( medMetaDataKeys::BirthDate.key(), birthdate );
            medData->setMetaData ( medMetaDataKeys::StudyDescription.key(), QStringList() << studyName );
            medData->setMetaData ( medMetaDataKeys::StudyID.key(), QStringList() << studyId );
            medData->setMetaData ( medMetaDataKeys::StudyInstanceUID.key(), QStringList() << studyUid );

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

    if(seriesName != "EmptySeries")
    {
        index = medDataIndex ( npdc->dataSourceId(), patientDbId, studyDbId, npdc->seriesId ( true ) );

        QString seriesId = medMetaDataKeys::SeriesID.getFirstValue(data);
        QString seriesUid = medMetaDataKeys::SeriesInstanceUID.getFirstValue(data);
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
        item->d->thumb = data->generateThumbnail(med::defaultThumbnailSize);
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

/**
* Finds if parameter @seriesName is already being used in the database
* if is not, it returns @seriesName unchanged
* otherwise, it returns an unused new series name (created by adding a suffix)
* @param seriesName - the series name
* @return newSeriesName - a new, unused, series name
**/
QString medDatabaseNonPersistentImporter::ensureUniqueSeriesName ( const QString seriesName, const QString studyId )
{
    QPointer<medDatabaseNonPersistentController> npdc =
        medDatabaseNonPersistentController::instance();

    QList<medDatabaseNonPersistentItem*> items = npdc->items();

    QStringList seriesNames;
    for(medDatabaseNonPersistentItem* item : items)
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
