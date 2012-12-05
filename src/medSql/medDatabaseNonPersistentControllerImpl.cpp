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
#include "medDatabaseNonPersistentReader.h"
#include "medDatabaseNonPersistentImporter.h"

#include <medDataIndex.h>
#include <medMessageController.h>
#include <medJobManager.h>

#include "medAttachedData.h"
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
    qDebug() << "DEBUG : entering medDatabaseNonPersistentControllerImpl::import(const QString& file,const QString& importUuid)";
    medDatabaseNonPersistentReader *reader =
            new medDatabaseNonPersistentReader(file,importUuid);

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
    qDebug() << "DEBUG : entering medDatabaseNonPersistentControllerImpl::import";

    medDatabaseNonPersistentImporter *importer = new medDatabaseNonPersistentImporter(data,callerUuid);
    medMessageProgress *message = medMessageController::instance()->showProgress("Importing data item");

    connect(importer, SIGNAL(progressed(int)),    message, SLOT(setProgress(int)));
    connect(importer, SIGNAL(nonPersistentImported(const medDataIndex &,const QString&)), this, SIGNAL(updated(const medDataIndex &, QString)));
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
            qDebug() << "DEBUG : indexesToRemove.size() = " << indexesToRemove.size();
            qDebug() << "it.key() = " << it.key();
        }
    }

    if (indexesToRemove.size() == 0)
        return;

    for (medDataIndexList::const_iterator it(indexesToRemove.begin()); it != indexesToRemove.end(); ++it)
    {
        DataHashMapType::iterator itemIt(d->items.find(*it));
        delete itemIt.value();
        d->items.erase(itemIt);
    }

    emit medAbstractDbController::updated(index);
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


 const QList<medDataIndex> medDatabaseNonPersistentControllerImpl::moveStudy(const medDataIndex& indexStudy, const medDataIndex& toPatient)
{
    QList<medDataIndex> newIndexList;
    medDataIndex newIndex(indexStudy);
    newIndex.setPatientId(toPatient.patientId());
    
    insert(newIndex, d->items[indexStudy]);
    
    //retrieve destination patient information
    dtkAbstractData *dataPatient = read(toPatient);

    if(dataPatient==NULL)
    {
        // let's try to get patient information from its series
        QList<medDataIndex> studiesIndexList = studies(toPatient);
        if(!studiesIndexList.isEmpty())
        {
            QList<medDataIndex> seriesIndexList = series(studiesIndexList[0]);

            if(!seriesIndexList.isEmpty())
                dataPatient = read(seriesIndexList[0]);
            else return newIndexList;
        }
        else return newIndexList;
    }
        
    dtkAbstractData *dataStudy = read(indexStudy); 

    if(dataStudy==NULL)
    {
	newIndexList << newIndex;
	
        // there is nothing to update directly in the study, we have to update the series of the study
        QList<medDataIndex> seriesIndexList = series(indexStudy);

        foreach(medDataIndex serie, seriesIndexList)
        {
            dataStudy = read(serie);
	    
            if(dataStudy!=NULL)
            {
                dataStudy->setMetaData ( medMetaDataKeys::PatientName.key(),
                                         QStringList() <<  dataPatient->metadata( medMetaDataKeys::PatientName.key()) );
                dataStudy->setMetaData ( medMetaDataKeys::PatientID.key(),
                                         QStringList() <<  dataPatient->metadata( medMetaDataKeys::PatientID.key()) );

                medDataIndex newSerieIndex = moveSerie(serie, newIndex);

                if(newSerieIndex.isValid())
                    newIndexList << newSerieIndex;
            }
        }
    }
    
    d->items.remove(indexStudy);

    return newIndexList;
}

const medDataIndex medDatabaseNonPersistentControllerImpl::moveSerie(const medDataIndex& indexSerie, const medDataIndex& toStudy)
{
    medDataIndex newIndex(indexSerie);
    newIndex.setStudyId(toStudy.studyId());
    newIndex.setPatientId(toStudy.patientId());

    // we need to update metadatas (patient, study) of the serie to move
    dtkAbstractData *dataSerie = read(indexSerie);

    //retrieve destination study information
    dtkAbstractData *dataStudy = read(toStudy);
    
    if(dataStudy == NULL)
    {
        // let's try to get study information from its series
        QList<medDataIndex> seriesIndexList = series(toStudy);
        if(!seriesIndexList.isEmpty())
            dataStudy = read(seriesIndexList[0]);
        else return newIndex;
    }

    dataSerie->setMetaData ( medMetaDataKeys::PatientName.key(),
                             QStringList() <<  dataStudy->metadata( medMetaDataKeys::PatientName.key()) );
    dataSerie->setMetaData ( medMetaDataKeys::PatientID.key(),
                             QStringList() <<  dataStudy->metadata( medMetaDataKeys::PatientID.key()) );
    dataSerie->setMetaData ( medMetaDataKeys::StudyDescription.key(),
                             QStringList() <<  dataStudy->metadata( medMetaDataKeys::StudyDescription.key()) );
    dataSerie->setMetaData ( medMetaDataKeys::StudyID.key(),
                             QStringList() <<  dataStudy->metadata( medMetaDataKeys::StudyID.key()) );
    dataSerie->setMetaData ( medMetaDataKeys::StudyDicomID.key(),
                             QStringList() <<  dataStudy->metadata( medMetaDataKeys::StudyDicomID.key()) );

    insert(newIndex, d->items[indexSerie]);
    d->items.remove(indexSerie);

    return newIndex;
}

