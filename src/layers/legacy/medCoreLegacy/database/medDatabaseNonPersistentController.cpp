/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractDataReader.h>
#include <dtkCoreSupport/dtkAbstractDataWriter.h>

#include <QtCore/QHash>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medDatabaseNonPersistentController.h>
#include <medDatabaseNonPersistentItem.h>
#include <medDatabaseNonPersistentItem_p.h>
#include <medDatabaseNonPersistentImporter.h>
#include <medDataIndex.h>
#include <medMessageController.h>
#include <medMetaDataKeys.h>
#include <medJobManagerL.h>

// /////////////////////////////////////////////////////////////////
// medDatabaseNonPersitentControllerPrivate
// /////////////////////////////////////////////////////////////////

class medDatabaseNonPersistentControllerPrivate
{
public:
    int patientIndex;
    int studyIndex;
    int seriesIndex;
    typedef QMap<medDataIndex, medDatabaseNonPersistentItem *> DataHashMapType;
    DataHashMapType items;
};

// /////////////////////////////////////////////////////////////////
// medDatabaseNonPersistentController
// /////////////////////////////////////////////////////////////////

std::unique_ptr<medDatabaseNonPersistentController> medDatabaseNonPersistentController::s_instance = nullptr;

medDatabaseNonPersistentController &medDatabaseNonPersistentController::instance()
{
    if(!s_instance)
    {
        s_instance = std::unique_ptr<medDatabaseNonPersistentController>(new medDatabaseNonPersistentController());
    }
    return *s_instance.get();
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
        return d->seriesIndex++;
    else
        return d->seriesIndex;
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
    medMessageProgress * message = medMessageController::instance().showProgress(tr("Opening file item"));

    connect(importer, SIGNAL(progressed(int)),    message, SLOT(setProgress(int)));
    connect(importer, SIGNAL(dataImported(medDataIndex,QUuid)), this, SIGNAL(dataImported(medDataIndex,QUuid)));

    connect(importer, SIGNAL(success(QObject *)), message, SLOT(success()));
    connect(importer, SIGNAL(failure(QObject *)), message, SLOT(failure()));
    connect(importer, SIGNAL(showError(const QString&,unsigned int)),
            &medMessageController::instance(),SLOT(showError(const QString&,unsigned int)));

    medJobManagerL::instance().registerJobItem(importer);
    QThreadPool::globalInstance()->start(importer);
}

int medDatabaseNonPersistentController::nonPersistentDataStartingIndex() const
{
    return 100000000;
}

medDatabaseNonPersistentController::medDatabaseNonPersistentController(): d(new medDatabaseNonPersistentControllerPrivate)
{
    d->patientIndex = nonPersistentDataStartingIndex();
    d->studyIndex   = nonPersistentDataStartingIndex();
    d->seriesIndex  = nonPersistentDataStartingIndex();
}

medDatabaseNonPersistentController::~medDatabaseNonPersistentController()
{
    qDeleteAll(d->items);

    delete d;
    d = nullptr;
}

bool medDatabaseNonPersistentController::isConnected() const
{
    // always connected as there is no database to control
    return true;
}

void medDatabaseNonPersistentController::importData(medAbstractData *data,
                                                    const QUuid & callerUuid, bool allowDuplicateSeriesName)
{
    medDatabaseNonPersistentImporter * importer = new medDatabaseNonPersistentImporter(data,callerUuid);
    medMessageProgress * message = medMessageController::instance().showProgress("Importing data item");

    connect(importer, SIGNAL(progressed(int)),    message, SLOT(setProgress(int)));
    connect(importer, SIGNAL(dataImported(medDataIndex,QUuid)), this, SIGNAL(dataImported(medDataIndex,QUuid)));

    connect(importer, SIGNAL(success(QObject *)), message, SLOT(success()));
    connect(importer, SIGNAL(failure(QObject *)), message, SLOT(failure()));
    connect(importer, SIGNAL(showError(const QString&,unsigned int)),
            &medMessageController::instance(),SLOT(showError(const QString&,unsigned int)));

    medJobManagerL::instance().registerJobItem(importer);
    QThreadPool::globalInstance()->start(importer);
}

void medDatabaseNonPersistentController::removeAll()
{
    // objects are reference counted.
    // We could check if the item is still in use... but we just remove our reference here.
    qDeleteAll(d->items);

    d->items.clear();
    d->patientIndex = nonPersistentDataStartingIndex();
    d->studyIndex   = nonPersistentDataStartingIndex();
    d->seriesIndex  = nonPersistentDataStartingIndex();
}

void medDatabaseNonPersistentController::remove(const medDataIndex &index)
{
    typedef medDatabaseNonPersistentControllerPrivate::DataHashMapType DataHashMapType;
    typedef QList<medDataIndex> medDataIndexList;
    medDataIndexList indexesToRemove;

    // Main index to remove
    if (d->items.contains(index))
    {
        indexesToRemove.push_back(index);
    }

    // If the index to remove is not a series, it's needed to remove series/study children also
    if (!index.isValidForSeries())
    {
        if (index.isValidForStudy())
        {
            for (DataHashMapType::const_iterator it(d->items.begin()); it != d->items.end(); ++it )
            {
                // Add series children indexes to the list to remove
                medDataIndex currentTemporaryIndex = it.key();
                if (currentTemporaryIndex.patientId() == index.patientId()
                        && currentTemporaryIndex.studyId()   == index.studyId()
                        && !indexesToRemove.contains(it.key()))
                {
                    indexesToRemove.push_back(it.key());
                }
            }
        }
        else if (index.isValidForPatient())
        {
            for (DataHashMapType::const_iterator it(d->items.begin()); it != d->items.end(); ++it )
            {
                // Add studies children indexes to the list to remove
                medDataIndex currentTemporaryIndex = it.key();
                if (currentTemporaryIndex.patientId() == index.patientId()
                        && !indexesToRemove.contains(it.key()))
                {
                    indexesToRemove.push_back(it.key());
                }
            }
        }
    }

    if (!indexesToRemove.isEmpty())
    {
        // Remove each index from the list
        for (medDataIndexList::const_iterator it(indexesToRemove.begin()); it != indexesToRemove.end(); ++it)
        {
            DataHashMapType::iterator itemIt(d->items.find(*it));
            delete itemIt.value();
            d->items.erase(itemIt);
        }

        // Patients are only valid for patients, Studies for patients and studies, and Series for patients, studies and series
        if( index.isValidForSeries() && series(index).isEmpty() )
        {
            // If the study is empty, remove it
            emit dataRemoved(index);
            remove(medDataIndex(index.dataSourceId(),
                                index.patientId(),
                                index.studyId(),
                                medDataIndex::NOT_VALID));
        }
        else if( index.isValidForStudy() && studies(index).isEmpty() )
        {
            emit dataRemoved(index);
            // If there are no studies in this patient anymore, remove it
            remove(medDataIndex(index.dataSourceId(),
                                index.patientId(),
                                medDataIndex::NOT_VALID,
                                medDataIndex::NOT_VALID));
        }
        else
        {
            emit dataRemoved(index);
        }
    }
}

QList<medDataIndex> medDatabaseNonPersistentController::availableItems() const
{
    return d->items.keys();
}

bool medDatabaseNonPersistentController::contains( const medDataIndex& index ) const
{
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

/**
 * @brief medDatabaseNonPersistentController::patients
 * @param none
 * @return each temporary patient index
 */
QList<medDataIndex> medDatabaseNonPersistentController::patients() const
{
    QList<medDataIndex> ret;

    typedef medDatabaseNonPersistentControllerPrivate::DataHashMapType MapType;
    int prevId = -1;
    for (MapType::const_iterator it(d->items.begin()); it != d->items.end(); ++it)
    {
        int currId = it.key().patientId();
        if ( currId != prevId )
        {
            ret.push_back(medDataIndex::makePatientIndex(dataSourceId(),currId));
            prevId = currId;
        }
    }
    return ret;
}

/**
 * @brief medDatabaseNonPersistentController::studies
 * @param index of a series, study or patient
 * @return each temporary study of the index's patient
 */
QList<medDataIndex> medDatabaseNonPersistentController::studies( const medDataIndex& index ) const
{
    QList<medDataIndex> ret;

    if ( !index.isValidForPatient() )
    {
        qWarning() << "invalid index passed (not patient)";
        return ret;
    }

    typedef medDatabaseNonPersistentControllerPrivate::DataHashMapType MapType;
    QList<int> studyIdFoundList;

    // For each temporary index
    for (MapType::const_iterator it(d->items.begin()); it != d->items.end(); ++it)
    {
        // If we're in the right index's patient
        if ( it.key().patientId() == index.patientId() )
        {
            int currentStudyId = it.key().studyId();

            // Get each study of this patient
            if (currentStudyId != medDataIndex::NOT_VALID
                    && !studyIdFoundList.contains(currentStudyId))
            {
                studyIdFoundList.push_back(currentStudyId);
                ret.push_back(medDataIndex::makeStudyIndex(this->dataSourceId(), index.patientId(), currentStudyId));
            }
        }
    }

    return ret;
}

/**
 * @brief medDatabaseNonPersistentController::series
 * @param index of a series or study
 * @return each series of the study of the index
 */
QList<medDataIndex> medDatabaseNonPersistentController::series( const medDataIndex& index ) const
{
    QList<medDataIndex> ret;

    if ( !index.isValidForStudy() )
    {
        qWarning() << "invalid index passed (not study)";
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

QStringList medDatabaseNonPersistentController::series(const QString &seriesName, const QString &studyId) const
{
    QStringList seriesNames;
    medDatabaseNonPersistentItem *item;
    for (medDataIndex index : d->items.keys())
    {
        if (index.isValidForSeries())
        {
            item = d->items.value(index);
            if (studyId.isEmpty())
            {
                if (item->seriesName().startsWith(seriesName))
                    seriesNames << item->seriesName();
            }
            else
            {
                if (item->studyId()==studyId && item->seriesName().startsWith(seriesName))
                {
                    seriesNames << item->seriesName();
                }
            }
        }
    }
    return seriesNames;
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
    if (it != d->items.end()  && it.value()!=nullptr ) {
        medAbstractData *data = it.value()->data();
        if (data &&  data->hasMetaData(key) )
            return data->metadata(key);
    } else {
        // Cannot find an exact match for the given index. Find first data that may match
        // using ordered map.
        it = d->items.lowerBound( index );
        if (it != d->items.end() && medDataIndex::isMatch( it.key(), index) && it.value()!=nullptr ) {
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

    medDatabaseNonPersistentItem * studyItem = nullptr;

    studyItem = d->items.find(indexStudy).value();

    insert(newIndex, studyItem);
    newIndexList << newIndex;

    //retrieve destination patient information
    medAbstractData *dataPatient = retrieve(toPatient);

    if(dataPatient==nullptr)
    {
        // let's try to get patient information from its series
        QList<medDataIndex> studiesIndexList = studies(toPatient);
        for(medDataIndex tempStudy : studiesIndexList)
        {
            QList<medDataIndex> seriesIndexList = series(tempStudy);

            if(!seriesIndexList.isEmpty())
            {
                dataPatient = retrieve(seriesIndexList[0]);
                break;
            }
        }
        if( dataPatient == nullptr )
            return newIndexList;
    }

    medAbstractData *dataStudy = retrieve(indexStudy);

    if(dataStudy!=nullptr)
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

    for(medDataIndex series : seriesIndexList)
    {
        dataStudy = retrieve(series);

        if(dataStudy!=nullptr)
        {
            medDataIndex newSeriesIndex = moveSeries(series, newIndex);

            if(newSeriesIndex.isValid())
                newIndexList << newSeriesIndex;
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

medDataIndex medDatabaseNonPersistentController::moveSeries(const medDataIndex& indexSeries, const medDataIndex& toStudy)
{
    medDataIndex newIndex(indexSeries);
    newIndex.setStudyId(toStudy.studyId());
    newIndex.setPatientId(toStudy.patientId());

    if(indexSeries == newIndex)
    {
        //the series is being moved to the same study, nothing to do
        return indexSeries;
    }

    // we need to update metadatas (patient, study) of the series to move
    medAbstractData *dataSeries = retrieve(indexSeries);

    //retrieve destination study information
    medAbstractData *dataStudy = retrieve(toStudy);

    medDatabaseNonPersistentItem * seriesItem = nullptr;
    seriesItem = d->items.find(indexSeries).value();

    if(dataStudy == nullptr)
    {
        // let's try to get study information from its series
        QList<medDataIndex> seriesIndexList = series(toStudy);
        if(!seriesIndexList.isEmpty())
            dataStudy = retrieve(seriesIndexList[0]);
        else return newIndex;
    }

    if(dataSeries && seriesItem)
    {
        dataSeries->setMetaData ( medMetaDataKeys::PatientName.key(),
                                 QStringList() <<  dataStudy->metadata( medMetaDataKeys::PatientName.key()) );
        dataSeries->setMetaData ( medMetaDataKeys::PatientID.key(),
                                 QStringList() <<  dataStudy->metadata( medMetaDataKeys::PatientID.key()) );
        dataSeries->setMetaData ( medMetaDataKeys::BirthDate.key(),
                                 QStringList() <<  dataStudy->metadata( medMetaDataKeys::BirthDate.key()) );
        dataSeries->setMetaData ( medMetaDataKeys::StudyDescription.key(),
                                 QStringList() <<  dataStudy->metadata( medMetaDataKeys::StudyDescription.key()) );
        dataSeries->setMetaData ( medMetaDataKeys::StudyID.key(),
                                 QStringList() <<  dataStudy->metadata( medMetaDataKeys::StudyID.key()) );
        dataSeries->setMetaData ( medMetaDataKeys::StudyInstanceUID.key(),
                                 QStringList() <<  dataStudy->metadata( medMetaDataKeys::StudyInstanceUID.key()) );

        seriesItem->setName(dataStudy->metadata( medMetaDataKeys::PatientName.key()));
        seriesItem->setPatientId(dataStudy->metadata( medMetaDataKeys::PatientID.key()));
        seriesItem->setBirthdate(dataStudy->metadata( medMetaDataKeys::BirthDate.key()));
        seriesItem->setStudyName(dataStudy->metadata( medMetaDataKeys::StudyDescription.key()));
        seriesItem->setStudyId(dataStudy->metadata( medMetaDataKeys::StudyID.key()));
    }

    insert(newIndex, d->items[indexSeries]);

    typedef medDatabaseNonPersistentControllerPrivate::DataHashMapType DataHashMapType;
    DataHashMapType::iterator itemIt(d->items.find(indexSeries));
    d->items.erase(itemIt);

    emit metadataModified(indexSeries); // to signal the series has been removed
    emit metadataModified(newIndex); // to signal the series has been added

    return newIndex;
}

medAbstractData* medDatabaseNonPersistentController::retrieve(const medDataIndex& index, bool readFullData) const
{
    // Lookup item in hash table.
    medDatabaseNonPersistentControllerPrivate::DataHashMapType::const_iterator it( d->items.find(index) );

    // Is item in our table ? if not, return null.
    if ( it == d->items.end() || it.value() == nullptr )
        return nullptr;

    medAbstractData* retreivedData(it.value()->data());

    return retreivedData;
}
