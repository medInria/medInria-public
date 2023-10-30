/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDataPacsController.h"

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medDatabaseNonPersistentItem.h>
#include <medDatabaseNonPersistentItem_p.h>
#include <medDataManager.h>
#include <medDataPacsImporter.h>
#include <medMessageController.h>
#include <medJobManagerL.h>

// /////////////////////////////////////////////////////////////////
// medDataPacsControllerPrivate
// /////////////////////////////////////////////////////////////////

class medDataPacsControllerPrivate
{
public:
    medDataPacsControllerPrivate() : startingIndex(200000000)
    {
        patientIndex = startingIndex;
        studyIndex = startingIndex;
        seriesIndex = startingIndex;
        timeout = 60000;
        numberOfFiles = 1;
    };
    int startingIndex;
    int patientIndex;
    int studyIndex;
    int seriesIndex;
    int timeout;
    int numberOfFiles;
    QHash<QString, QHash<QString, QVariant> > patientData;
    QHash<QString, QHash<QString, QVariant> > seriesData;
    typedef QMap<medDataIndex, medDatabaseNonPersistentItem *> DataHashMapType;
    DataHashMapType items;
    QMap<medDataIndex, QUuid> uuids;
};

// /////////////////////////////////////////////////////////////////
// medDataPacsController
// /////////////////////////////////////////////////////////////////

std::unique_ptr<medDataPacsController> medDataPacsController::s_instance = nullptr;

medDataPacsController &medDataPacsController::instance()
{
    if(!s_instance)
    {
        s_instance = std::unique_ptr<medDataPacsController>(new medDataPacsController());
    }
    return *s_instance.get();
}

QList<medDatabaseNonPersistentItem *> medDataPacsController::items(void)
{
    return d->items.values();
}

medDatabaseNonPersistentItem *medDataPacsController::getStudyItem(const QString &patientId,
                                                                  const QString &studyInstanceUID)
{
    medDatabaseNonPersistentItem *item = nullptr;
    medDataIndex index;
    for (auto it : d->items.toStdMap())
    {
        index = it.first;
        if (index.isValidForStudy() && !index.isValidForSeries())
        {
            if (it.second->patientId() == patientId &&
                it.second->studyUid() == studyInstanceUID)
            {
                item = it.second;
            }
        }
    }
    return item;
}

medDatabaseNonPersistentItem *medDataPacsController::getSeriesItem(const QString &patientId,
                                                                   const QString &studyInstanceUID,
                                                                   const QString &seriesInstanceUID)
{
    medDatabaseNonPersistentItem *item = nullptr;
    medDataIndex index;
    for (auto it : d->items.toStdMap())
    {
        index = it.first;
        if (index.isValidForSeries())
        {
            if (it.second->patientId() == patientId &&
                it.second->studyUid() == studyInstanceUID &&
                it.second->seriesUid() == seriesInstanceUID)
            {
                item = it.second;
            }
        }
    }
    return item;
}

medDataIndex medDataPacsController::index(medDatabaseNonPersistentItem *item)
{
    return d->items.key(item);
}

void medDataPacsController::insert(medDataIndex index, medDatabaseNonPersistentItem *item)
{
    d->items.insert(index, item);
}

bool medDataPacsController::importMetaDataFromPacs(const QHash<QString, QHash<QString, QVariant> > &pData,
                                                   const QHash<QString, QHash<QString, QVariant> > &sData)
{
    d->patientData = pData;
    d->seriesData = sData;
    QString patientName, patientId, birthdate;
    QString studyInstanceUID, studyDescription;
    QString seriesInstanceUID, seriesDescription, modality;
    for (QHash<QString, QVariant> &patientEntry : d->patientData)
    {
        patientId = d->patientData.key(patientEntry);
        patientName = patientEntry[medMetaDataKeys::PatientName.key()].toString();
        birthdate = patientEntry[medMetaDataKeys::BirthDate.key()].toString();
        medDataIndex ptIndex = storePatientMetaDataFromPacs(patientName, patientId, birthdate);
        if (ptIndex.isValid())
        {
            QUuid uuid = QUuid::createUuid();
            d->uuids[ptIndex] = uuid;
            QHash<QString, QVariant> studies = patientEntry["studies"].toHash();
            QList<QString> keys = studies.keys();
            for (QString sKey : keys)
            {
                studyInstanceUID = sKey;
                studyDescription = studies.value(sKey).toString();
                medDataIndex stIndex = storeStudyMetaDataFromPacs(ptIndex, studyInstanceUID, studyDescription);
                if (stIndex.isValid())
                {
                    uuid = QUuid::createUuid();
                    d->uuids[stIndex] = uuid;
                    for (QHash<QString, QVariant> &seriesEntry : d->seriesData)
                    {
                        if (seriesEntry[medMetaDataKeys::StudyInstanceUID.key()].toString() == sKey)
                        {
                            seriesInstanceUID = d->seriesData.key(seriesEntry);
                            seriesDescription = seriesEntry[medMetaDataKeys::SeriesDescription.key()].toString();
                            modality = seriesEntry[medMetaDataKeys::Modality.key()].toString();
                            medDataIndex seIndex = storeSeriesMetaDataFromPacs(ptIndex, stIndex, seriesInstanceUID, seriesDescription, modality);
                            if (seIndex.isValid())
                            {
                                uuid = QUuid::createUuid();
                                d->uuids[seIndex] = uuid;
                                emit dataImported(seIndex, uuid);
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

medDataIndex medDataPacsController::storeSeriesMetaDataFromPacs(medDataIndex &ptIndex,
                                                                medDataIndex &stIndex,
                                                                QString &seriesInstanceUID,
                                                                QString &seriesDescription,
                                                                QString &modality)
{
    int patientDbId = stIndex.patientId();
    int studyDbId = stIndex.studyId();
    medDatabaseNonPersistentItem *stItem = d->items[stIndex];
    QString patientName = stItem->name();
    QString patientId = stItem->patientId();
    QString birthdate = stItem->birthdate();
    QString studyDescription = stItem->studyName();
    QString studyInstanceUID = stItem->studyUid();
    medDataIndex index;

    medDataIndex databaseIndex = medDataManager::instance().controller()->indexForSeriesUID(patientName, studyInstanceUID, seriesInstanceUID);
    if (databaseIndex.isValid())
    {
        qDebug() << "Series exists in the database, We do not want to fetch this data";
        return medDataIndex();
    }
    else
    {
        for (auto it : d->items.toStdMap())
        {
            medDataIndex index = it.first;
            medDatabaseNonPersistentItem *item = it.second;
            if (item->name() == patientName &&
                item->studyUid() == studyInstanceUID &&
                item->seriesUid() == seriesInstanceUID &&
                item->seriesName() == seriesDescription)
            {
                qDebug() << "Series exists in the non permanent data structure (pacs), We do not want to fetch this data";
                return medDataIndex();
            }
        }
    }

    if (seriesDescription != "EmptySeries")
    {
        index = medDataIndex(dataSourceId(), patientDbId, studyDbId, seriesId(true));

        medDatabaseNonPersistentItem *seriesItem = new medDatabaseNonPersistentItem;

        seriesItem->setName(patientName);
        seriesItem->setPatientId(patientId);
        seriesItem->setBirthdate(birthdate);
        seriesItem->setStudyName(studyDescription);
        seriesItem->setSeriesName(seriesDescription);
        seriesItem->setIndex(index);
        seriesItem->setSeriesUid(seriesInstanceUID);
        seriesItem->setStudyUid(studyInstanceUID);

        insert(index, seriesItem);
    }

    return index;
}

medDataIndex medDataPacsController::storeStudyMetaDataFromPacs(medDataIndex &ptIndex,
                                                               QString &studyInstanceUID,
                                                               QString &studyDescription)
{
    int studyDbId = -1;
    medDataIndex stIndex;
    medDatabaseNonPersistentItem *ptItem = d->items[ptIndex];
    if (!ptItem)
    {
        qDebug() << "[ERROR] Impossible case !";
        return medDataIndex();
    }

    QString patientName = ptItem->name();
    QString patientId = ptItem->patientId();
    QString birthdate = ptItem->birthdate();

    int patientDbId = ptIndex.patientId();
    if (patientDbId == -1)
    {
        return medDataIndex();
    }

    if (studyDescription != "EmptyStudy")
    {
        // check if study is already in the persistent database
        medDataIndex databaseIndex = medDataManager::instance().controller()->indexForStudyUID(patientName, studyInstanceUID);
        medDatabaseNonPersistentItem *studyItem = nullptr;

        if (databaseIndex.isValid())
        {
            qDebug() << "Study exists in the database, I reuse its Id";
            studyDbId = databaseIndex.studyId();
            stIndex = medDataIndex(dataSourceId(), patientDbId, studyDbId, -1);
            studyItem = d->items.value(stIndex);
        }
        else
        {
            for (auto it : d->items.toStdMap())
            {
                medDataIndex index = it.first;
                medDatabaseNonPersistentItem *item = it.second;
                if (item->patientId() == patientId &&
                    item->studyUid() == studyInstanceUID &&
                    item->studyName() == studyDescription)
                {
                    qDebug() << "Study exists in the non permanent data structure (pacs), I reuse its Id";
                    studyDbId = index.studyId();
                    studyItem = item;
                    stIndex = index;
                    break;
                }
            }
        }

        if (studyDbId == -1)
        {
            studyDbId = this->studyId(true);
        }

        if (studyItem == nullptr)
        {
            // create an item for study
            studyItem = new medDatabaseNonPersistentItem;
            stIndex = medDataIndex(dataSourceId(), patientDbId, studyDbId, -1);

            medAbstractData *medData = new medAbstractData();
            medData->setMetaData(medMetaDataKeys::PatientName.key(), QStringList() << patientName);
            medData->setMetaData(medMetaDataKeys::BirthDate.key(), birthdate);
            medData->setMetaData(medMetaDataKeys::StudyDescription.key(), QStringList() << studyDescription);
            medData->setMetaData(medMetaDataKeys::StudyInstanceUID.key(), QStringList() << studyInstanceUID);

            studyItem->setName(patientName);
            studyItem->setPatientId(patientId);
            studyItem->setBirthdate(birthdate);
            studyItem->setStudyName(studyDescription);
            studyItem->setIndex(stIndex);
            studyItem->setData(medData);
            studyItem->setStudyUid(studyInstanceUID);

            insert(stIndex, studyItem);
        }
    }
    return stIndex;
}

medDataIndex medDataPacsController::storePatientMetaDataFromPacs(QString &patientName,
                                                                 QString &patientId,
                                                                 QString &birthdate)
{

    int patientDbId = -1;
    if (patientName.isEmpty() || patientName == "")
    {
        patientName = "John Doe";
    }
    // check if patient is already in the persistent database
    medDataIndex databaseIndex = medDataManager::instance().controller()->indexForPatient(patientName);
    medDatabaseNonPersistentItem *patientItem = nullptr;
    medDataIndex ptIndex;
    if (databaseIndex.isValid())
    {
        qDebug() << "Patient exists in the database, I reuse his Id";
        patientDbId = databaseIndex.patientId();
        ptIndex = medDataIndex(dataSourceId(), patientDbId, -1, -1);
        patientItem = d->items.value(ptIndex);
    }
    else
    {
        // check if patient is already in the data structure
        for (auto it : d->items.toStdMap())
        {
            medDataIndex index = it.first;
            medDatabaseNonPersistentItem *item = it.second;
            if (item->patientId() == patientId &&
                item->name() == patientName &&
                item->birthdate() == birthdate)
            {
                patientDbId = index.patientId();
                patientItem = item;
                ptIndex = index;
                break;
            }
        }
    }

    if (patientDbId == -1)
    {
        patientDbId = this->patientId(true);
    }

    if (patientItem == nullptr)
    {
        // create an item for patient
        patientItem = new medDatabaseNonPersistentItem;
        ptIndex = medDataIndex(dataSourceId(), patientDbId, -1, -1);

        medAbstractData *medData = new medAbstractData();
        medData->setMetaData(medMetaDataKeys::PatientName.key(), QStringList() << patientName);
        medData->setMetaData(medMetaDataKeys::BirthDate.key(), birthdate);

        patientItem->setName(patientName);
        patientItem->setPatientId(patientId);
        patientItem->setIndex(ptIndex);
        patientItem->setBirthdate(birthdate);
        patientItem->setData(medData);

        insert(ptIndex, patientItem);
    }
    return ptIndex;
}

int medDataPacsController::patientId(bool increment) const
{
    if (increment)
        return d->patientIndex++;
    else
        return d->patientIndex;
}

int medDataPacsController::studyId(bool increment)
{
    if (increment)
        return d->studyIndex++;
    else
        return d->studyIndex;
}

int medDataPacsController::seriesId(bool increment)
{
    if (increment)
        return d->seriesIndex++;
    else
        return d->seriesIndex;
}

medDataPacsController::medDataPacsController() : d(new medDataPacsControllerPrivate)
{
}

medDataPacsController::~medDataPacsController()
{
    delete d;
    d = nullptr;
}

bool medDataPacsController::isConnected() const
{
    // always connected as there is no database to control
    return true;
}

int medDataPacsController::dataSourceId() const
{
    return 3;
}
void medDataPacsController::requestDatabaseForModel(QHash<int, QHash<QString, QVariant> > &patientData,
                                                    QHash<int, QHash<QString, QVariant> > &studyData,
                                                    QHash<int, QHash<QString, QVariant> > &seriesData) const
{
}

bool medDataPacsController::isPersistent() const
{
    return false;
}

bool medDataPacsController::contains(const medDataIndex &index) const
{
    if (d->items.contains(index))
        return true;
    //we may not have a complete match, but we may contain it
    if (index.isValid() && index.dataSourceId() == dataSourceId())
    {
        //index is valid and comes from this dataSource
        typedef medDataPacsControllerPrivate::DataHashMapType MapType;
        // Cannot find an exact match for the given index.
        // Find first data that may match
        // using ordered map, and scan while index matches.
        MapType::const_iterator it = d->items.lowerBound(index);
        if (it != d->items.end() && medDataIndex::isMatch(it.key(), index))
        {
            return true;
        }
    }
    return false;
}

QString medDataPacsController::metaData(const medDataIndex &index, const QString &key) const
{
    QString retValue;
    if (d->items.contains(index))
    {
        medDatabaseNonPersistentItem *item = d->items[index];
        if (key == medMetaDataKeys::SeriesDescription.key())
        {
            retValue = item->seriesName();
        }
        else if (key == medMetaDataKeys::StudyDescription.key())
        {
            retValue = item->studyName();
        }
        else if (key == medMetaDataKeys::PatientName.key())
        {
            retValue = item->name();
        }
        else if (key == medMetaDataKeys::BirthDate.key())
        {
            retValue = item->birthdate();
        }
    }
    return retValue;
}

bool medDataPacsController::setMetaData(const medDataIndex &index, const QString &key, const QString &value)
{
    return false;
}

/**
 * @brief medDataPacsController::patients
 * @param none
 * @return each fetched patient index
 */
QList<medDataIndex> medDataPacsController::patients() const
{
    QSet<medDataIndex> indexes;

    for (medDataIndex index : d->items.keys())
    {
        indexes.insert(medDataIndex::makePatientIndex(dataSourceId(), index.patientId()));
    }
    return indexes.values();
}

/**
 * @brief medDataPacsController::studies
 * @param index of a series, study or patient
 * @return each study of the index's patient
 */
QList<medDataIndex> medDataPacsController::studies(const medDataIndex &index) const
{
    QSet<medDataIndex> indexes;

    if (!index.isValidForPatient())
    {
        qWarning() << "invalid index passed (not patient)";
        return indexes.values();
    }

    for (medDataIndex id : d->items.keys())
    {
        if (id.isValidForStudy())
        {
            if (id.patientId() == index.patientId())
            {
                indexes.insert(medDataIndex::makeStudyIndex(dataSourceId(), index.patientId(), id.studyId()));
            }
        }
    }
    return indexes.values();
}

/**
 * @brief medDataPacsController::series
 * @param index of a series or study
 * @return each series of the study of the index
 */
QList<medDataIndex> medDataPacsController::series(const medDataIndex &index) const
{
    QSet<medDataIndex> indexes;
    if (!index.isValidForStudy())
    {
        qWarning() << "invalid index passed (not study)";
        return indexes.values();
    }

    for (medDataIndex id : d->items.keys())
    {
        if (id.isValidForSeries())
        {
            if (id.patientId() == index.patientId() && id.studyId() == index.studyId())
            {
                indexes.insert(id);
            }
        }
    }
    return indexes.values();
}

QStringList medDataPacsController::series(const QString &seriesName, const QString &studyId) const
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

medDataIndex medDataPacsController::getStudyIfEmpty(const medDataIndex seriesIndex)
{
    medDataIndex index;
    bool foundOtherSeriesInStudy = false;
    medDataIndex studyIndex = medDataIndex();

    for (medDataIndex id : d->items.keys())
    {
        if (id.isValidForSeries() &&
            id.patientId() == seriesIndex.patientId() &&
            id.studyId() == seriesIndex.studyId() &&
            id.seriesId() != seriesIndex.seriesId())
        {
            foundOtherSeriesInStudy = true;
            break;
        }
    }

    if (foundOtherSeriesInStudy)
    {
        index = medDataIndex();
    }
    else
    {
        for (medDataIndex id : d->items.keys())
        {
            if (id.isValidForStudy() &&
                id.patientId() == seriesIndex.patientId() &&
                id.studyId() == seriesIndex.studyId())
            {
                studyIndex = id;
                break;
            }
        }
        index = studyIndex;
    }
    return index;
}

medDataIndex medDataPacsController::getPatientIfEmpty(const medDataIndex studyIndex)
{
    medDataIndex index;
    bool foundOtherStudyInPatient = false;
    medDataIndex patientIndex = medDataIndex();

    for (medDataIndex id : d->items.keys())
    {
        if (id.isValidForStudy() &&
            id.patientId() == studyIndex.patientId() &&
            id.studyId() != studyIndex.studyId())
        {
            foundOtherStudyInPatient = true;
            break;
        }
    }

    if (foundOtherStudyInPatient)
    {
        index = medDataIndex();
    }
    else
    {
        for (medDataIndex id : d->items.keys())
        {
            if (id.isValidForPatient() &&
                id.patientId() == studyIndex.patientId())
            {
                patientIndex = id;
                break;
            }
        }
        index = patientIndex;
    }
    return index;
}

void medDataPacsController::remove(const medDataIndex &index)
{
    // Main index to remove

    if (d->items.contains(index))
    {
        d->items.remove(index);
        emit dataRemoved(index);
    }
    if (index.isValidForSeries())
    {
        medDataIndex studyIndex = getStudyIfEmpty(index);
        if (studyIndex.isValid())
        {
            d->items.remove(studyIndex);
            emit dataRemoved(studyIndex);
            medDataIndex patientIndex = getPatientIfEmpty(studyIndex);
            if (patientIndex.isValid())
            {
                d->items.remove(patientIndex);
                emit dataRemoved(patientIndex);
            }
        }
    }
    else
    {
        if (index.isValidForStudy())
        {
            for (medDataIndex id : d->items.keys())
            {
                if (id.isValidForSeries() && id.studyId() == index.studyId())
                {
                    d->items.remove(id);
                    emit dataRemoved(id);
                }
            }
            if (series(index).isEmpty())
            {
                d->items.remove(index);
                emit dataRemoved(index);
            }
        }
        else if (index.isValidForPatient())
        {
            for (medDataIndex id : d->items.keys())
            {
                if ((id.isValidForSeries() || id.isValidForStudy()) && id.patientId() == index.patientId())
                {
                    d->items.remove(id);
                    emit dataRemoved(id);
                }
            }
            if (studies(index).isEmpty())
            {
                d->items.remove(index);
                emit dataRemoved(index);
            }
        }
    }
}

void medDataPacsController::removeAll()
{
    // objects are reference counted.
    // We could check if the item is still in use... but we just remove our reference here.
    qDeleteAll(d->items);

    d->items.clear();
    d->patientData.clear();
    d->seriesData.clear();
    d->patientIndex = d->startingIndex;
    d->studyIndex = d->startingIndex;
    d->seriesIndex = d->startingIndex;
    d->numberOfFiles = 1;
    d->timeout = 60000;
}

QList<medDataIndex> medDataPacsController::moveStudy(const medDataIndex &indexStudy, const medDataIndex &toPatient)
{
    QList<medDataIndex> newIndexList;
    if (!indexStudy.isValidForStudy() || !toPatient.isValidForPatient())
    {
        return newIndexList;
    }

    medDataIndex newIndex = medDataIndex::makeStudyIndex(dataSourceId(), toPatient.patientId(), indexStudy.studyId());
    if (indexStudy == newIndex)
    {
        //the study is being moved to the same patient, nothing to do
        return newIndexList;
    }
    newIndexList << newIndex;

    medDatabaseNonPersistentItem *patientItem = d->items.value(toPatient);
    medDatabaseNonPersistentItem *studyItem = d->items.value(indexStudy);
    if (!patientItem || !studyItem)
    {
        qDebug() << "[ERROR] moveStudy : Impossible Case : There're no item linked to index";
        return newIndexList;
    }
    medDatabaseNonPersistentItem *newStudyItem = new medDatabaseNonPersistentItem;

    newStudyItem->setName(patientItem->name());
    newStudyItem->setPatientId(patientItem->patientId());
    newStudyItem->setBirthdate(patientItem->birthdate());
    newStudyItem->setIndex(newIndex);
    newStudyItem->setStudyUid(studyItem->studyUid());
    newStudyItem->setStudyName(studyItem->studyName());

    insert(newIndex, newStudyItem);

    // we also have to update the series of the study
    QList<medDataIndex> seriesIndexList = series(indexStudy);
    for (medDataIndex seIndex : seriesIndexList)
    {
        medDataIndex newSeriesIndex = moveSeries(seIndex, newIndex);

        if (newSeriesIndex.isValid())
        {
            newIndexList << newSeriesIndex;
        }
    }

    d->items.remove(indexStudy);
    if (!newIndexList.isEmpty())
    {
        emit metadataModified(indexStudy);      // to signal the study has been removed
        emit metadataModified(newIndexList[0]); // to signal the study has been added
    }
    return newIndexList;
}

medDataIndex medDataPacsController::moveSeries(const medDataIndex &indexSeries, const medDataIndex &toStudy)
{
    if (!indexSeries.isValidForSeries() || !toStudy.isValidForStudy())
    {
        return medDataIndex();
    }

    medDataIndex newIndex = medDataIndex::makeSeriesIndex(dataSourceId(), toStudy.patientId(), toStudy.studyId(), indexSeries.seriesId());
    if (indexSeries == newIndex)
    {
        //the series is being moved to the same study, nothing to do
        return indexSeries;
    }

    medDatabaseNonPersistentItem *studyItem = d->items.value(toStudy);
    medDatabaseNonPersistentItem *seriesItem = d->items.value(indexSeries);
    if (!studyItem || !seriesItem)
    {
        qDebug() << "[ERROR] moveSeries : Impossible Case : There're no item linked to index";
        return medDataIndex();
    }
    medDatabaseNonPersistentItem *newSeriesItem = new medDatabaseNonPersistentItem;

    newSeriesItem->setName(studyItem->name());
    newSeriesItem->setPatientId(studyItem->patientId());
    newSeriesItem->setStudyUid(studyItem->studyUid());
    newSeriesItem->setStudyName(studyItem->studyName());
    newSeriesItem->setIndex(newIndex);
    newSeriesItem->setSeriesUid(seriesItem->seriesUid());
    newSeriesItem->setSeriesName(seriesItem->seriesName());

    insert(newIndex, newSeriesItem);

    d->items.remove(indexSeries);

    emit metadataModified(indexSeries); // to signal the series has been removed
    emit metadataModified(newIndex);    // to signal the series has been added

    return newIndex;
}

bool medDataPacsController::loadData(const medDataIndex &index)
{
    bool ret = false;
    QUuid uuid = QUuid();
    medMessageProgress *message = nullptr;
    if (!d->items.contains(index) || d->items.value(index) == nullptr)
    {
        return false;
    }
    medDatabaseNonPersistentItem *item = d->items.value(index);
    if (item->data() != nullptr && item->data()->data() != nullptr)
    {

        return true;
    }
    else
    {
        message = medMessageController::instance().showProgress(tr("Request data from PACS (C-Move Request)"));
        message->setProgress(0);
        QString instanceUID, queryLevel;
        if (index.isValidForSeries())
        {
            instanceUID = item->seriesUid();
            queryLevel = "SERIES";
        }
        else if (index.isValidForStudy())
        {
            instanceUID = item->studyUid();
            queryLevel = "STUDY";
        }
        else if (index.isValidForPatient())
        {
            instanceUID = item->patientId();
            queryLevel = "PATIENT";
        }
        uuid = d->uuids.value(index);
        emit moveRequested(instanceUID, queryLevel);
        connect(&medDataManager::instance(), SIGNAL(updateProgress(int)), message, SLOT(setProgress(int)));
    }
    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    connect(&timer, &QTimer::timeout, &loop, [&]() {
        loop.exit(ResultEventLoop::FAILURE);
    });
    timer.start(d->timeout);

    connect(&medDataManager::instance(), &medDataManager::moveState, &loop, [&](int status, const QString &pathOrMessage) {
        switch (status)
        {
        case 0: //OK
            onImportFinished(pathOrMessage, loop, uuid, timer);
            break;
        case 1: //PENDING
            timer.setInterval(d->timeout);
            break;
        case -1: //KO
        default:
            loop.exit(ResultEventLoop::FAILURE);
            break;
        }
    });
    int status = loop.exec();

    if (status == ResultEventLoop::SUCCESS)
    {
        message->success();
        ret = true;
    }
    else
    {
        message->setProgress(100);
        message->failure();
        remove(index);
        ret = false;
    }

    return ret;
}

void medDataPacsController::onImportFinished(const QString &path, QEventLoop &loop, QUuid &uuid, QTimer &timer)
{
    QFileInfo info(path);
    QString name;
    if (!uuid.isNull())
    {
        medDataIndex index = d->uuids.key(uuid);
        if (index.isValidForSeries())
        {
            name = d->items.value(index)->seriesName();
        }
        else if (index.isValidForStudy())
        {
            name = d->items.value(index)->studyName();
        }
        else if (index.isValidForPatient())
        {
            name = d->items.value(index)->name();
        }
        else
        {
            loop.exit(-1);
        }
    }
    medDataPacsImporter *importer = new medDataPacsImporter(info.absoluteFilePath(), uuid);
    medMessageProgress *message = medMessageController::instance().showProgress("Importing " + name);

    connect(importer, SIGNAL(progressed(int)), message, SLOT(setProgress(int)), Qt::UniqueConnection);
    connect(importer, &medDataPacsImporter::progressed, &timer, [&](int p) {
        timer.setInterval(d->timeout);
    });

    connect(importer,
            QOverload<const medDataIndex &, const QUuid &>::of(&medDataPacsImporter::dataImported),
            &loop, [&](const medDataIndex &index, const QUuid &uuid) -> void {
                emit dataImported(index, uuid);
                d->numberOfFiles--;
                if (d->numberOfFiles == 0 && index.isValid())
                {
                    d->numberOfFiles = 1;
                    if (index.isValid())
                    {
                        loop.quit();
                    }
                    else
                    {
                        loop.exit(ResultEventLoop::FAILURE);
                    }
                }
            });

    connect(importer,
            QOverload<const medDataIndex &>::of(&medDataPacsImporter::dataImported),
            &loop, [&](const medDataIndex &index) -> void {
                if (index.isValid())
                {
                    loop.quit();
                }
                else
                {
                    loop.exit(ResultEventLoop::FAILURE);
                }
            });

    connect(importer,
            QOverload<QObject *>::of(&medJobItemL::failure),
            &loop, [&](QObject *obj) -> void {
                loop.exit(ResultEventLoop::FAILURE);
            });

    connect(importer, SIGNAL(success(QObject *)), message, SLOT(success()));
    connect(importer, SIGNAL(failure(QObject *)), message, SLOT(failure()));
    connect(importer, SIGNAL(showError(const QString &, unsigned int)),
            &medMessageController::instance(), SLOT(showError(const QString &, unsigned int)));

    medJobManagerL::instance().registerJobItem(importer);
    QThreadPool::globalInstance()->start(importer);
}

medAbstractData *medDataPacsController::retrieve(const medDataIndex &index, bool readFullData) const
{
    medAbstractData *data = nullptr;
    medDatabaseNonPersistentItem *item = d->items.value(index);
    if (item && item->data())
    {
        data = item->data();
    }
    return data;
}

void medDataPacsController::setNumberOfFilesInDir(int num)
{
    d->numberOfFiles = num;
}

bool medDataPacsController::isDataLoaded(const medDataIndex &index)
{
    bool ret = false;
    if (!d->items.contains(index) || d->items.value(index) == nullptr)
    {
        ret = false;
    }
    else
    {
        medDatabaseNonPersistentItem *item = d->items.value(index);
        if (index.isValidForSeries())
        {
            if (item->data() != nullptr && item->data()->data() != nullptr)
            {
                ret = true;
            }
        }
        else if (index.isValidForStudy())
        {
            QList<medDataIndex> indexes = series(index);
            for (medDataIndex id : indexes)
            {
                ret = isDataLoaded(id);
                if (!ret)
                {
                    break;
                }
            }
        }
        else if (index.isValidForPatient())
        {
            QList<medDataIndex> indexes = studies(index);
            for (medDataIndex id : indexes)
            {
                ret = isDataLoaded(id);
                if (!ret)
                {
                    break;
                }
            }
        }
    }
    return ret;
}
