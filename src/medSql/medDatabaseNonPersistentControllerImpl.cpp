/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseNonPersistentItem.h"
#include "medDatabaseNonPersistentItem_p.h"
#include "medDatabaseNonPersistentControllerImpl.h"
#include "medDatabaseNonPersistentImporter.h"

#include <medDataIndex.h>
#include <medMessageController.h>
#include <medJobManager.h>

#include <medMetaDataKeys.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>

#include <QtCore/QHash>

// /////////////////////////////////////////////////////////////////
// medDatabaseNonPersitentControllerPrivate
// /////////////////////////////////////////////////////////////////

class medDatabaseNonPersistentControllerImplPrivate
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

int medDatabaseNonPersistentControllerImpl::patientId(bool increment)
{
    if (increment)
        return d->patientIndex++;
    else
        return d->patientIndex;
}

int medDatabaseNonPersistentControllerImpl::studyId(bool increment)
{
    if (increment)
        return d->studyIndex++;
    else
        return d->studyIndex;
}

int medDatabaseNonPersistentControllerImpl::seriesId(bool increment)
{
    if (increment)
        return d->serieIndex++;
    else
        return d->serieIndex;
}

int medDatabaseNonPersistentControllerImpl::imageId(bool increment)
{
    if (increment)
        return d->imageIndex++;
    else
        return d->imageIndex;
}

QList<medDatabaseNonPersistentItem *> medDatabaseNonPersistentControllerImpl::items(void)
{
    return d->items.values();
}

void medDatabaseNonPersistentControllerImpl::insert(medDataIndex index, medDatabaseNonPersistentItem *item)
{
    d->items.insert(index, item);
}

void medDatabaseNonPersistentControllerImpl::import(const QString& file,QString importUuid)
{
    medDatabaseNonPersistentImporter *reader =
            new medDatabaseNonPersistentImporter(file,importUuid);
	medMessageProgress *message = medMessageController::instance()->showProgress(tr("Opening file item"));

    connect(reader, SIGNAL(progressed(int)),
            message, SLOT(setProgress(int)));
    connect(reader, SIGNAL(addedIndex(const medDataIndex &,const QString&)),
            this, SIGNAL(updated(const medDataIndex &, QString)));
    connect(reader, SIGNAL(success(QObject *)),
            message, SLOT(success()));
    connect(reader, SIGNAL(failure(QObject *)),
             message, SLOT(failure()));

    medJobManager::instance()->registerJobItem(reader);
    QThreadPool::globalInstance()->start(reader);
}

dtkSmartPointer<dtkAbstractData> medDatabaseNonPersistentControllerImpl::read( const medDataIndex& index ) const
{
    // Lookup item in hash table.
    medDatabaseNonPersistentControllerImplPrivate::DataHashMapType::const_iterator it( d->items.find(index) );

    // Is item in our table ? if not, return null.
    if ( it == d->items.end() || it.value() == NULL )
        return dtkSmartPointer<dtkAbstractData> ();

    dtkSmartPointer<dtkAbstractData> ret(it.value()->data());

    return ret;
}

int medDatabaseNonPersistentControllerImpl::nonPersistentDataStartingIndex(void) const
{
    return 100000000;
}

medDatabaseNonPersistentControllerImpl::medDatabaseNonPersistentControllerImpl(void): d(new medDatabaseNonPersistentControllerImplPrivate)
{
    d->patientIndex = nonPersistentDataStartingIndex();
    d->studyIndex = nonPersistentDataStartingIndex();
    d->serieIndex = nonPersistentDataStartingIndex();
    d->imageIndex = nonPersistentDataStartingIndex();
}

medDatabaseNonPersistentControllerImpl::~medDatabaseNonPersistentControllerImpl(void)
{
    foreach(medDatabaseNonPersistentItem *item, d->items)
        delete item;

    delete d;

    d = NULL;
}

bool medDatabaseNonPersistentControllerImpl::isConnected() const
{
    // always connected as there is no database to control
    return true;
}

void medDatabaseNonPersistentControllerImpl::import(dtkAbstractData *data,
                                                    QString callerUuid)
{
    medDatabaseNonPersistentImporter *importer = new medDatabaseNonPersistentImporter(data,callerUuid);
    medMessageProgress *message = medMessageController::instance()->showProgress("Importing data item");

    connect(importer, SIGNAL(progressed(int)),    message, SLOT(setProgress(int)));
    connect(importer, SIGNAL(addedIndex(const medDataIndex &,const QString&)), this, SIGNAL(updated(const medDataIndex &, QString)));
    connect(importer, SIGNAL(success(QObject *)), message, SLOT(success()));
    connect(importer, SIGNAL(failure(QObject *)), message, SLOT(failure()));

    medJobManager::instance()->registerJobItem(importer);
    QThreadPool::globalInstance()->start(importer);
}

void medDatabaseNonPersistentControllerImpl::clear(void)
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


void medDatabaseNonPersistentControllerImpl::remove(const medDataIndex &index)
{
    typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType DataHashMapType;
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

    emit medAbstractDbController::updated(index);

    if( index.isValidForSeries() && series(index).isEmpty() )
        remove(medDataIndex(index.dataSourceId(), index.patientId(), index.isValidForStudy(), -1, -1));
    else if( index.isValidForStudy() && series(index).isEmpty() )
        remove(medDataIndex(index.dataSourceId(), index.patientId(),-1, -1, -1));
}

qint64 medDatabaseNonPersistentControllerImpl::getEstimatedSize( const medDataIndex& index ) const
{
    Q_UNUSED(index);
    return 0;
}

QList<medDataIndex> medDatabaseNonPersistentControllerImpl::availableItems() const
{
    return d->items.keys();
}

bool medDatabaseNonPersistentControllerImpl::contains( const medDataIndex& index ) const
{
    //does not work since we can share patient Id with the persistent one.
    //return index.patientId() >= this->nonPersistentDataStartingIndex();
    if (d->items.contains(index))
        return true;
    //we may not have a complete match, but we may contain it
    if (index.isValid() && index.dataSourceId() == dataSourceId())
    {
        //index is valid and comes from this dataSource
        typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType MapType;
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

QImage medDatabaseNonPersistentControllerImpl::thumbnail( const medDataIndex &index ) const
{
    medDatabaseNonPersistentItem * item = NULL;
    if ( d->items.contains(index) ) {
        item = d->items.find(index).value();
    } else {
        typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType MapType;
        // Cannot find an exact match for the given index. Find first data that may match
        // using ordered map, and scan while index matches.
        MapType::const_iterator it = d->items.lowerBound( index );
        if (it != d->items.end() && medDataIndex::isMatch(it.key() , index ) ) {
            item = it.value();
        }
        // Since this does not contain real images, but series, search on the series index too.
        if ( !item && index.isValidForImage() ) {
            medDataIndex seriesIndex = medDataIndex::makeSeriesIndex(index.dataSourceId(), index.patientId(), index.studyId(), index.seriesId() );
            MapType::const_iterator it = d->items.lowerBound( seriesIndex );
            if ( it != d->items.end() &&  medDataIndex::isMatch(it.key() , seriesIndex ) ) {
                item = it.value();
            }
        }
    }
    if ( item && item->data()) {
        return item->data()->thumbnail();
    }
    else
    {
        return QImage();
    }
}

int medDatabaseNonPersistentControllerImpl::dataSourceId() const
{
    return 2;
}

QList<medDataIndex> medDatabaseNonPersistentControllerImpl::patients() const
{
    QList<medDataIndex> ret;

    typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType MapType;
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

QList<medDataIndex> medDatabaseNonPersistentControllerImpl::studies( const medDataIndex& index ) const
{
    QList<medDataIndex> ret;

    if ( !index.isValidForPatient() )
    {
        qWarning() << "invalid index passed";
        return ret;
    }

    typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType MapType;
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

QList<medDataIndex> medDatabaseNonPersistentControllerImpl::series( const medDataIndex& index ) const
{
    QList<medDataIndex> ret;

    if ( !index.isValidForStudy() )
    {
        qWarning() << "invalid index passed";
        return ret;
    }

    typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType MapType;
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

QList<medDataIndex> medDatabaseNonPersistentControllerImpl::images( const medDataIndex& index ) const
{
    QList<medDataIndex> ret;

    if ( !index.isValidForSeries() )
    {
        qWarning() << "invalid index passed";
        return ret;
    }

    typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType MapType;
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

QString medDatabaseNonPersistentControllerImpl::metaData( const medDataIndex& index, const QString& key ) const
{
    typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType MapType;

    MapType::const_iterator it(d->items.find(index));
    if (it != d->items.end()  && it.value()!=NULL ) {
        dtkAbstractData *data = it.value()->data();
        if (data &&  data->hasMetaData(key) )
            return data->metadata(key);
    } else {
        // Cannot find an exact match for the given index. Find first data that may match
        // using ordered map.
        it = d->items.lowerBound( index );
        if (it != d->items.end() && medDataIndex::isMatch( it.key(), index) && it.value()!=NULL ) {
            dtkAbstractData *data = it.value()->data();
            if (data &&  data->hasMetaData(key) )
                return data->metadata(key);
        }
    }
    return QString();
}

bool medDatabaseNonPersistentControllerImpl::setMetaData( const medDataIndex& index, const QString& key, const QString& value )
{
    typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType MapType;

    MapType::const_iterator it(d->items.find(index));
    if (it != d->items.end() ) {
        dtkAbstractData *data = it.value()->data();
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
            dtkAbstractData *data = it.value()->data();
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

bool medDatabaseNonPersistentControllerImpl::isPersistent( ) const
{
    return false;
}


QList<medDataIndex> medDatabaseNonPersistentControllerImpl::moveStudy(const medDataIndex& indexStudy, const medDataIndex& toPatient)
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
    dtkAbstractData *dataPatient = read(toPatient);

    if(dataPatient==NULL)
    {
        // let's try to get patient information from its series  
        QList<medDataIndex> studiesIndexList = studies(toPatient);
        foreach(medDataIndex tempStudy, studiesIndexList)
        {
            QList<medDataIndex> seriesIndexList = series(tempStudy);

            if(!seriesIndexList.isEmpty())
            {
                dataPatient = read(seriesIndexList[0]);
                break;
            }      
        }
        if( dataPatient == NULL )
            return newIndexList;     
    }

    dtkAbstractData *dataStudy = read(indexStudy);

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
        dataStudy = read(serie);

        if(dataStudy!=NULL)
        {
            medDataIndex newSerieIndex = moveSerie(serie, newIndex);

            if(newSerieIndex.isValid())
                newIndexList << newSerieIndex;
        }
    }

    typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType DataHashMapType;
    DataHashMapType::iterator itemIt(d->items.find(indexStudy));
    d->items.erase(itemIt);
    
    if(!newIndexList.isEmpty())
    {
        emit updated(indexStudy); // to signal the study has been removed
        emit updated(newIndexList[0]); // to signal the study has been added
    }
        
    return newIndexList;
}

medDataIndex medDatabaseNonPersistentControllerImpl::moveSerie(const medDataIndex& indexSerie, const medDataIndex& toStudy)
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
    dtkAbstractData *dataSerie = read(indexSerie);

    //retrieve destination study information
    dtkAbstractData *dataStudy = read(toStudy);
    
    medDatabaseNonPersistentItem * serieItem = NULL;
    serieItem = d->items.find(indexSerie).value();
    
    if(dataStudy == NULL)
    {
        // let's try to get study information from its series
        QList<medDataIndex> seriesIndexList = series(toStudy);
        if(!seriesIndexList.isEmpty())
            dataStudy = read(seriesIndexList[0]);
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

    typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType DataHashMapType;
    DataHashMapType::iterator itemIt(d->items.find(indexSerie));
    d->items.erase(itemIt);
    
    emit updated(indexSerie); // to signal the serie has been removed
    emit updated(newIndex); // to signal the serie has been added
    
    return newIndex;
}

