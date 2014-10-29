/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseNonPersistentItem.h>
#include <medDatabaseNonPersistentItem_p.h>
#include <medDatabaseNonPersistentController.h>
#include <medDatabaseNonPersistentImporter.h>

#include <medDataIndex.h>
#include <medMessageController.h>
#include <medJobManager.h>

#include <medMetaDataKeys.h>

#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <medAbstractData.h>

#include <QtCore/QHash>

// /////////////////////////////////////////////////////////////////
// medDatabaseNonPersitentControllerPrivate
// /////////////////////////////////////////////////////////////////

class medDatabaseNonPersistentControllerPrivate
{
public:
    int patientIndex;
    int studyIndex;
    int serieIndex;
    int imageIndex;
    typedef QMap<medDataIndex, medDatabaseNonPersistentItem *> DataHashMapType;
    DataHashMapType items;
};

// /////////////////////////////////////////////////////////////////
// medDatabaseNonPersitentController
// /////////////////////////////////////////////////////////////////

medDatabaseNonPersistentController*  medDatabaseNonPersistentController::s_instance = NULL;

medDatabaseNonPersistentController* medDatabaseNonPersistentController::instance() {
    if ( ! s_instance) {
        s_instance = new medDatabaseNonPersistentController();
    }
    return s_instance;
}

int medDatabaseNonPersistentController::patientId(bool increment)
{
    if (increment)
        return d->patientIndex++;
    else
        return d->patientIndex;
}

int medDatabaseNonPersistentController::studyId(bool increment)
{
    if (increment)
        return d->studyIndex++;
    else
        return d->studyIndex;
}

int medDatabaseNonPersistentController::seriesId(bool increment)
{
    if (increment)
        return d->serieIndex++;
    else
        return d->serieIndex;
}

int medDatabaseNonPersistentController::imageId(bool increment)
{
    if (increment)
        return d->imageIndex++;
    else
        return d->imageIndex;
}

QList<medDatabaseNonPersistentItem *> medDatabaseNonPersistentController::items(void)
{
    return d->items.values();
}

void medDatabaseNonPersistentController::insert(medDataIndex index, medDatabaseNonPersistentItem *item)
{
    d->items.insert(index, item);
}

void medDatabaseNonPersistentController::importPath(const QString& file,const QUuid & importUuid, bool /*indexWithoutCopying*/)
{
    medDatabaseNonPersistentImporter * importer = new medDatabaseNonPersistentImporter(file,importUuid);
    connect(importer, SIGNAL(dataImported(medDataIndex,QUuid)), this, SIGNAL(dataImported(medDataIndex,QUuid)));
    importer->start();
}

int medDatabaseNonPersistentController::nonPersistentDataStartingIndex(void) const
{
    return 100000000;
}

medDatabaseNonPersistentController::medDatabaseNonPersistentController(void): d(new medDatabaseNonPersistentControllerPrivate)
{
    d->patientIndex = nonPersistentDataStartingIndex();
    d->studyIndex = nonPersistentDataStartingIndex();
    d->serieIndex = nonPersistentDataStartingIndex();
    d->imageIndex = nonPersistentDataStartingIndex();
}

medDatabaseNonPersistentController::~medDatabaseNonPersistentController(void)
{
    qDeleteAll(d->items);

    delete d;
    d = NULL;
}

bool medDatabaseNonPersistentController::isConnected() const
{
    // always connected as there is no database to control
    return true;
}

void medDatabaseNonPersistentController::importData(medAbstractData *data,
                                                    const QUuid & callerUuid)
{
    medDatabaseNonPersistentImporter * importer = new medDatabaseNonPersistentImporter(data,callerUuid);
    connect(importer, SIGNAL(dataImported(medDataIndex,QUuid)), this, SIGNAL(dataImported(medDataIndex,QUuid)));
    importer->start();
}

void medDatabaseNonPersistentController::removeAll()
{
    // objects are reference counted.
    // We could check if the item is still in use... but we just remove our reference here.
    qDeleteAll(d->items);

    d->items.clear();
    d->patientIndex = nonPersistentDataStartingIndex();
    d->studyIndex = nonPersistentDataStartingIndex();
    d->serieIndex = nonPersistentDataStartingIndex();
    d->imageIndex = nonPersistentDataStartingIndex();
}


void medDatabaseNonPersistentController::remove(const medDataIndex &index)
{
    typedef medDatabaseNonPersistentControllerPrivate::DataHashMapType DataHashMapType;
    typedef QList<medDataIndex> medDataIndexList;
    medDataIndexList indexesToRemove;

    for (DataHashMapType::const_iterator it(d->items.begin()); it != d->items.end(); ++it ) {
        if (medDataIndex::isMatch( it.key(), index)) {
            indexesToRemove.push_back(it.key());
        }
    }

    for (medDataIndexList::const_iterator it(indexesToRemove.begin()); it != indexesToRemove.end(); ++it)
    {
        DataHashMapType::iterator itemIt(d->items.find(*it));
        delete itemIt.value();
        d->items.erase(itemIt);
    }

    if( index.isValidForSeries() && series(index).isEmpty() )
        remove(medDataIndex(index.dataSourceId(), index.patientId(), index.isValidForStudy(), -1, -1));
    else if( index.isValidForStudy() && series(index).isEmpty() )
        remove(medDataIndex(index.dataSourceId(), index.patientId(), -1, -1, -1));

    emit dataRemoved(index);
}

QList<medDataIndex> medDatabaseNonPersistentController::availableItems() const
{
    return d->items.keys();
}

bool medDatabaseNonPersistentController::contains( const medDataIndex& index ) const
{
    //does not work since we can share patient Id with the persistent one.
    //return index.patientId() >= this->nonPersistentDataStartingIndex();
    if (d->items.contains(index))
        return true;
    //we may not have a complete match, but we may contain it
    if (index.isValid() && index.dataSourceId() == dataSourceId())
    {
        //index is valid and comes from this dataSource
        typedef medDatabaseNonPersistentControllerPrivate::DataHashMapType MapType;
        // Cannot find an exact match for the given index.
        // Find first data that may match
        // using ordered map, and scan while index matches.
        MapType::const_iterator it = d->items.lowerBound( index );
        if (it != d->items.end() && medDataIndex::isMatch(it.key() , index ) ) {
            return true;
        }
    }
    return false;
}

int medDatabaseNonPersistentController::dataSourceId() const
{
    return 2;
}

QList<medDataIndex> medDatabaseNonPersistentController::patients() const
{
    QList<medDataIndex> ret;

    typedef medDatabaseNonPersistentControllerPrivate::DataHashMapType MapType;
    int prevId = -1;
    for (MapType::const_iterator it(d->items.begin()); it != d->items.end(); ++it)
    {
        int currId = it.key().patientId();
        if ( currId != prevId ) {
            ret.push_back(medDataIndex::makePatientIndex(dataSourceId(),currId));
            prevId = currId;
        }
    }
    return ret;
}

QList<medDataIndex> medDatabaseNonPersistentController::studies( const medDataIndex& index ) const
{
    QList<medDataIndex> ret;

    if ( !index.isValidForPatient() )
    {
        qWarning() << "invalid index passed";
        return ret;
    }

    typedef medDatabaseNonPersistentControllerPrivate::DataHashMapType MapType;
    // First which does not compare less then given index -> first study for this patient.
    MapType::const_iterator it(d->items.lowerBound(medDataIndex::makePatientIndex(this->dataSourceId(), index.patientId())));
    int prevId = -1;
    for ( ; it != d->items.end() ; ++it)
    {
        if ( it.key().patientId() != index.patientId() )
            break;
        int currId = it.key().studyId();
        if ( currId != prevId ) {
            ret.push_back(medDataIndex::makeStudyIndex(this->dataSourceId(), index.patientId(), currId));
            prevId = currId;
        }
    }
    return ret;
}

QList<medDataIndex> medDatabaseNonPersistentController::series( const medDataIndex& index ) const
{
    QList<medDataIndex> ret;

    if ( !index.isValidForStudy() )
    {
        qWarning() << "invalid index passed";
        return ret;
    }

    typedef medDatabaseNonPersistentControllerPrivate::DataHashMapType MapType;
    // First which does not compare less then given index -> first series for this patient.
    MapType::const_iterator it(d->items.lowerBound(medDataIndex::makeStudyIndex(this->dataSourceId(), index.patientId(), index.studyId())));
    int prevId = -1;
    for ( ; it != d->items.end() ; ++it)
    {
        if ( it.key().patientId() != index.patientId() || it.key().studyId() != index.studyId())
            break;

        int currId = it.key().seriesId();
        if ( currId != prevId ) {
            ret.push_back(medDataIndex::makeSeriesIndex(this->dataSourceId(), index.patientId(), index.studyId(), currId));
            prevId = currId;
        }
    }
    return ret;
}

QList<medDataIndex> medDatabaseNonPersistentController::images( const medDataIndex& index ) const
{
    QList<medDataIndex> ret;

    if ( !index.isValidForSeries() )
    {
        qWarning() << "invalid index passed";
        return ret;
    }

    typedef medDatabaseNonPersistentControllerPrivate::DataHashMapType MapType;
    // First which does not compare less then given index -> first series for this patient.
    MapType::const_iterator it(d->items.lowerBound(medDataIndex::makeSeriesIndex(this->dataSourceId(), index.patientId(), index.studyId(), index.seriesId())));
    int prevId = -1;
    for ( ; it != d->items.end(); ++it)
    {
        if ( it.key().patientId() != index.patientId() || it.key().studyId() != index.studyId() || it.key().seriesId() != index.seriesId())
            break;

        int currId = it.key().seriesId();
        if ( currId != prevId ) {
            ret.push_back(medDataIndex(this->dataSourceId(), index.patientId(), index.studyId(), index.seriesId(), currId));
            prevId = currId;
        }
    }
    return ret;
}

QPixmap medDatabaseNonPersistentController::thumbnail(const medDataIndex &index) const
{
    if (d->items.contains(index)) {
        return QPixmap::fromImage(d->items.value(index)->thumb());
    }
    return QPixmap();
}

QString medDatabaseNonPersistentController::metaData( const medDataIndex& index, const QString& key ) const
{
    typedef medDatabaseNonPersistentControllerPrivate::DataHashMapType MapType;

    MapType::const_iterator it(d->items.find(index));
    if (it != d->items.end()  && it.value()!=NULL ) {
        medAbstractData *data = it.value()->data();
        if (data &&  data->hasMetaData(key) )
            return data->metadata(key);
    } else {
        // Cannot find an exact match for the given index. Find first data that may match
        // using ordered map.
        it = d->items.lowerBound( index );
        if (it != d->items.end() && medDataIndex::isMatch( it.key(), index) && it.value()!=NULL ) {
            medAbstractData *data = it.value()->data();
            if (data &&  data->hasMetaData(key) )
                return data->metadata(key);
        }
    }
    return QString();
}

bool medDatabaseNonPersistentController::setMetaData( const medDataIndex& index, const QString& key, const QString& value )
{
    typedef medDatabaseNonPersistentControllerPrivate::DataHashMapType MapType;

    MapType::const_iterator it(d->items.find(index));
    if (it != d->items.end() ) {
        medAbstractData *data = it.value()->data();
        if (data) {
            data->setMetaData(key, value);
            return true;
        }
    } else {
        // Cannot find an exact match for the given index. Find first data that may match
        // using ordered map, and scan while index matches.
        it = d->items.lowerBound( index );
        int numSet(0);
        for ( ; it != d->items.end() && medDataIndex::isMatch(it.key() , index ); ++it ) {
            medAbstractData *data = it.value()->data();
            if (data) {
                data->setMetaData(key, value);
                ++numSet;
            }
        }
        if (numSet)
            return true;
    }
    return false;
}

bool medDatabaseNonPersistentController::isPersistent( ) const
{
    return false;
}


QList<medDataIndex> medDatabaseNonPersistentController::moveStudy(const medDataIndex& indexStudy, const medDataIndex& toPatient)
{
    QList<medDataIndex> newIndexList;
    medDataIndex newIndex(indexStudy);
    newIndex.setPatientId(toPatient.patientId());

    if(indexStudy == newIndex)
    {
        //the study is being moved to the same patient, nothing to do
        return newIndexList;
    }

    medDatabaseNonPersistentItem * studyItem = NULL;

    studyItem = d->items.find(indexStudy).value();

    insert(newIndex, studyItem);
    newIndexList << newIndex;

    //retrieve destination patient information
    medAbstractData *dataPatient = retrieve(toPatient);

    if(dataPatient==NULL)
    {
        // let's try to get patient information from its series
        QList<medDataIndex> studiesIndexList = studies(toPatient);
        foreach(medDataIndex tempStudy, studiesIndexList)
        {
            QList<medDataIndex> seriesIndexList = series(tempStudy);

            if(!seriesIndexList.isEmpty())
            {
                dataPatient = retrieve(seriesIndexList[0]);
                break;
            }
        }
        if( dataPatient == NULL )
            return newIndexList;
    }

    medAbstractData *dataStudy = retrieve(indexStudy);

    if(dataStudy!=NULL)
    {
        //update metadata
        dataStudy->setMetaData ( medMetaDataKeys::PatientName.key(),
                                 QStringList() <<  dataPatient->metadata( medMetaDataKeys::PatientName.key()) );
        dataStudy->setMetaData ( medMetaDataKeys::PatientID.key(),
                                 QStringList() <<  dataPatient->metadata( medMetaDataKeys::PatientID.key()) );
        dataStudy->setMetaData ( medMetaDataKeys::BirthDate.key(),
                                 QStringList() <<  dataPatient->metadata( medMetaDataKeys::BirthDate.key()) );

        if(studyItem)
        {
            //update item properties
            studyItem->setName(dataPatient->metadata( medMetaDataKeys::PatientName.key()));
            studyItem->setPatientId(dataPatient->metadata( medMetaDataKeys::PatientID.key()));
            studyItem->setBirthdate(dataPatient->metadata( medMetaDataKeys::BirthDate.key()));
        }
    }

    // we also have to update the series of the study
    QList<medDataIndex> seriesIndexList = series(indexStudy);

    foreach(medDataIndex serie, seriesIndexList)
    {
        dataStudy = retrieve(serie);

        if(dataStudy!=NULL)
        {
            medDataIndex newSerieIndex = moveSerie(serie, newIndex);

            if(newSerieIndex.isValid())
                newIndexList << newSerieIndex;
        }
    }

    typedef medDatabaseNonPersistentControllerPrivate::DataHashMapType DataHashMapType;
    DataHashMapType::iterator itemIt(d->items.find(indexStudy));
    d->items.erase(itemIt);

    if(!newIndexList.isEmpty())
    {
        emit metadataModified(indexStudy); // to signal the study has been removed
        emit metadataModified(newIndexList[0]); // to signal the study has been added
    }

    return newIndexList;
}

medDataIndex medDatabaseNonPersistentController::moveSerie(const medDataIndex& indexSerie, const medDataIndex& toStudy)
{
    medDataIndex newIndex(indexSerie);
    newIndex.setStudyId(toStudy.studyId());
    newIndex.setPatientId(toStudy.patientId());

    if(indexSerie == newIndex)
    {
        //the serie is being moved to the same study, nothing to do
        return indexSerie;
    }

    // we need to update metadatas (patient, study) of the serie to move
    medAbstractData *dataSerie = retrieve(indexSerie);

    //retrieve destination study information
    medAbstractData *dataStudy = retrieve(toStudy);

    medDatabaseNonPersistentItem * serieItem = NULL;
    serieItem = d->items.find(indexSerie).value();

    if(dataStudy == NULL)
    {
        // let's try to get study information from its series
        QList<medDataIndex> seriesIndexList = series(toStudy);
        if(!seriesIndexList.isEmpty())
            dataStudy = retrieve(seriesIndexList[0]);
        else return newIndex;
    }

    if(dataSerie && serieItem)
    {
        dataSerie->setMetaData ( medMetaDataKeys::PatientName.key(),
                                 QStringList() <<  dataStudy->metadata( medMetaDataKeys::PatientName.key()) );
        dataSerie->setMetaData ( medMetaDataKeys::PatientID.key(),
                                 QStringList() <<  dataStudy->metadata( medMetaDataKeys::PatientID.key()) );
        dataSerie->setMetaData ( medMetaDataKeys::BirthDate.key(),
                                 QStringList() <<  dataStudy->metadata( medMetaDataKeys::BirthDate.key()) );
        dataSerie->setMetaData ( medMetaDataKeys::StudyDescription.key(),
                                 QStringList() <<  dataStudy->metadata( medMetaDataKeys::StudyDescription.key()) );
        dataSerie->setMetaData ( medMetaDataKeys::StudyID.key(),
                                 QStringList() <<  dataStudy->metadata( medMetaDataKeys::StudyID.key()) );
        dataSerie->setMetaData ( medMetaDataKeys::StudyDicomID.key(),
                                 QStringList() <<  dataStudy->metadata( medMetaDataKeys::StudyDicomID.key()) );

        serieItem->setName(dataStudy->metadata( medMetaDataKeys::PatientName.key()));
        serieItem->setPatientId(dataStudy->metadata( medMetaDataKeys::PatientID.key()));
        serieItem->setBirthdate(dataStudy->metadata( medMetaDataKeys::BirthDate.key()));
        serieItem->setStudyName(dataStudy->metadata( medMetaDataKeys::StudyDescription.key()));
        serieItem->setStudyId(dataStudy->metadata( medMetaDataKeys::StudyID.key()));
    }

    insert(newIndex, d->items[indexSerie]);

    typedef medDatabaseNonPersistentControllerPrivate::DataHashMapType DataHashMapType;
    DataHashMapType::iterator itemIt(d->items.find(indexSerie));
    d->items.erase(itemIt);

    emit metadataModified(indexSerie); // to signal the serie has been removed
    emit metadataModified(newIndex); // to signal the serie has been added

    return newIndex;
}

medAbstractData* medDatabaseNonPersistentController::retrieve(const medDataIndex& index) const
{
    // Lookup item in hash table.
    medDatabaseNonPersistentControllerPrivate::DataHashMapType::const_iterator it( d->items.find(index) );

    // Is item in our table ? if not, return null.
    if ( it == d->items.end() || it.value() == NULL )
        return NULL;

    medAbstractData* retreivedData(it.value()->data());

    return retreivedData;
}
