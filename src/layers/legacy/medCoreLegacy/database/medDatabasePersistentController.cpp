/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabasePersistentController.h"

#include <medJobManagerL.h>
#include <medMessageController.h>

#include "medDatabaseImporter.h"
#include "medDatabaseReader.h"
#include "medDatabaseRemover.h"
#include "medSettingsManager.h"
#include "medStorage.h"

class medDatabasePersistentControllerPrivate
{
public:
    void buildMetaDataLookup();
    struct TableEntry
    {
        TableEntry(QString t, QString c, bool isPath_ = false)
            : table(t), column(c), isPath(isPath_) {}
        QString table;
        QString column;
        bool isPath;
    };
    typedef QList<TableEntry> TableEntryList;
    typedef QHash<QString, TableEntryList> MetaDataMap;

    MetaDataMap metaDataLookup;
    // Reusable table names.
    static const QString T_series;
    static const QString T_study;
    static const QString T_patient;
};

const QString medDatabasePersistentControllerPrivate::T_series = "series";
const QString medDatabasePersistentControllerPrivate::T_study = "study";
const QString medDatabasePersistentControllerPrivate::T_patient = "patient";

void medDatabasePersistentControllerPrivate::buildMetaDataLookup()
{
    // The table defines the mapping between metadata in the medAbstractData and
    // the database tables.
    metaDataLookup.insert(medMetaDataKeys::ThumbnailPath.key(),
                          TableEntryList()
                              << TableEntry(T_series, "thumbnail", true)
                              << TableEntry(T_study, "thumbnail", true)
                              << TableEntry(T_patient, "thumbnail", true));

    // Patient data
    metaDataLookup.insert(medMetaDataKeys::PatientName.key(),
                          TableEntryList() << TableEntry(T_patient, "name"));
    metaDataLookup.insert(medMetaDataKeys::Gender.key(),
                          TableEntryList() << TableEntry(T_patient, "gender"));
    metaDataLookup.insert(medMetaDataKeys::BirthDate.key(),
                          TableEntryList() << TableEntry(T_patient, "birthdate"));
    metaDataLookup.insert(medMetaDataKeys::PatientID.key(),
                          TableEntryList() << TableEntry(T_patient, "patientId"));

    // Study Data
    metaDataLookup.insert(medMetaDataKeys::StudyDescription.key(),
                          TableEntryList() << TableEntry(T_study, "name"));
    metaDataLookup.insert(medMetaDataKeys::StudyInstanceUID.key(),
                          TableEntryList() << TableEntry(T_study, "uid"));
    metaDataLookup.insert(medMetaDataKeys::StudyID.key(),
                          TableEntryList() << TableEntry(T_study, "studyId"));
    metaDataLookup.insert(medMetaDataKeys::StudyTime.key(),
                          TableEntryList() << TableEntry(T_study, "time"));
    metaDataLookup.insert(medMetaDataKeys::StudyDate.key(),
                          TableEntryList() << TableEntry(T_study, "date"));
    // Series Data
    metaDataLookup.insert(medMetaDataKeys::Size.key(),
                          TableEntryList() << TableEntry(T_series, "size"));
    metaDataLookup.insert(medMetaDataKeys::SeriesDescription.key(),
                          TableEntryList() << TableEntry(T_series, "name"));
    metaDataLookup.insert(medMetaDataKeys::SeriesInstanceUID.key(),
                          TableEntryList() << TableEntry(T_series, "uid"));
    metaDataLookup.insert(medMetaDataKeys::SeriesID.key(),
                          TableEntryList() << TableEntry(T_series, "seriesId"));
    metaDataLookup.insert(medMetaDataKeys::Orientation.key(),
                          TableEntryList()
                              << TableEntry(T_series, "orientation"));
    metaDataLookup.insert(medMetaDataKeys::SeriesNumber.key(),
                          TableEntryList()
                              << TableEntry(T_series, "seriesNumber"));
    metaDataLookup.insert(medMetaDataKeys::SequenceName.key(),
                          TableEntryList()
                              << TableEntry(T_series, "sequenceName"));
    metaDataLookup.insert(medMetaDataKeys::SliceThickness.key(),
                          TableEntryList()
                              << TableEntry(T_series, "sliceThickness"));
    metaDataLookup.insert(medMetaDataKeys::Rows.key(),
                          TableEntryList() << TableEntry(T_series, "rows"));
    metaDataLookup.insert(medMetaDataKeys::Columns.key(),
                          TableEntryList() << TableEntry(T_series, "columns"));
    metaDataLookup.insert(medMetaDataKeys::Age.key(),
                          TableEntryList() << TableEntry(T_series, "age"));
    metaDataLookup.insert(medMetaDataKeys::Description.key(),
                          TableEntryList()
                              << TableEntry(T_series, "description"));
    metaDataLookup.insert(medMetaDataKeys::Modality.key(),
                          TableEntryList() << TableEntry(T_series, "modality"));
    metaDataLookup.insert(medMetaDataKeys::Protocol.key(),
                          TableEntryList() << TableEntry(T_series, "protocol"));
    metaDataLookup.insert(medMetaDataKeys::Comments.key(),
                          TableEntryList() << TableEntry(T_series, "comments"));
    metaDataLookup.insert(medMetaDataKeys::Status.key(),
                          TableEntryList() << TableEntry(T_series, "status"));
    metaDataLookup.insert(medMetaDataKeys::AcquisitionDate.key(),
                          TableEntryList()
                              << TableEntry(T_series, "acquisitiondate"));
    metaDataLookup.insert(medMetaDataKeys::ImportationDate.key(),
                          TableEntryList()
                              << TableEntry(T_series, "importationdate"));
    metaDataLookup.insert(medMetaDataKeys::Referee.key(),
                          TableEntryList() << TableEntry(T_series, "referee"));
    metaDataLookup.insert(medMetaDataKeys::Performer.key(),
                          TableEntryList() << TableEntry(T_series, "performer"));
    metaDataLookup.insert(medMetaDataKeys::Institution.key(),
                          TableEntryList()
                              << TableEntry(T_series, "institution"));
    metaDataLookup.insert(medMetaDataKeys::Report.key(),
                          TableEntryList() << TableEntry(T_series, "report"));
    metaDataLookup.insert(medMetaDataKeys::Origin.key(),
                          TableEntryList() << TableEntry(T_series, "origin"));
    metaDataLookup.insert(medMetaDataKeys::FlipAngle.key(),
                          TableEntryList() << TableEntry(T_series, "flipAngle"));
    metaDataLookup.insert(medMetaDataKeys::EchoTime.key(),
                          TableEntryList() << TableEntry(T_series, "echoTime"));
    metaDataLookup.insert(medMetaDataKeys::RepetitionTime.key(),
                          TableEntryList()
                              << TableEntry(T_series, "repetitionTime"));
    metaDataLookup.insert(medMetaDataKeys::AcquisitionTime.key(),
                          TableEntryList()
                              << TableEntry(T_series, "acquisitionTime"));
    metaDataLookup.insert(medMetaDataKeys::SeriesTime.key(),
                          TableEntryList() << TableEntry(T_series, "time"));
    metaDataLookup.insert(medMetaDataKeys::SeriesDate.key(),
                          TableEntryList() << TableEntry(T_series, "date"));
    metaDataLookup.insert(medMetaDataKeys::KVP.key(),
                          TableEntryList() << TableEntry(T_series, "kvp"));
}

/**
* Import data into the db read from file
* @param const QString & file The file containing the data
* @param bool indexWithoutCopying true if the file must only be indexed by its current path,
* false if the file will be imported (copied or converted to the internal storage format)
*/
void medDatabasePersistentController::importPath(const QString &file, const QUuid &importUuid, bool indexWithoutCopying)
{
    QFileInfo info(file);
    qDebug() << "persistent import UUID " << importUuid;
    medDatabaseImporter *importer = new medDatabaseImporter(info.absoluteFilePath(), importUuid, indexWithoutCopying);
    medMessageProgress *message = medMessageController::instance().showProgress("Importing " + info.fileName());

    connect(importer, SIGNAL(progressed(int)), message, SLOT(setProgress(int)));
    // connect(importer, SIGNAL(dataImported(medDataIndex, QUuid)), this, SIGNAL(dataImported(medDataIndex, QUuid)));
    connect(importer, QOverload<const medDataIndex &, const QUuid &>::of(&medAbstractDatabaseImporter::dataImported), this, [&](medDataIndex index, QUuid uuid) {
        qDebug() << "Persistent : index " << index << " imported with uuid " << uuid;
        emit dataImported(index, uuid);
    });

    connect(importer, SIGNAL(success(QObject *)), message, SLOT(success()));
    connect(importer, SIGNAL(failure(QObject *)), message, SLOT(failure()));
    connect(importer, SIGNAL(showError(const QString &, unsigned int)),
            &medMessageController::instance(), SLOT(showError(const QString &, unsigned int)));

    medJobManagerL::instance().registerJobItem(importer);
    QThreadPool::globalInstance()->start(importer);
}

/**
* Import data into the db read from memory
* @param medAbstractData * data dataObject
*/
void medDatabasePersistentController::importData(medAbstractData *data, const QUuid &importUuid, bool allowDuplicateSeriesName)
{
    medDatabaseImporter *importer = new medDatabaseImporter(data, importUuid, allowDuplicateSeriesName);
    medMessageProgress *message = medMessageController::instance().showProgress("Saving database item");

    connect(importer, SIGNAL(progressed(int)), message, SLOT(setProgress(int)));
    connect(importer, SIGNAL(dataImported(medDataIndex, QUuid)), this, SIGNAL(dataImported(medDataIndex, QUuid)));

    connect(importer, SIGNAL(success(QObject *)), message, SLOT(success()));
    connect(importer, SIGNAL(failure(QObject *)), message, SLOT(failure()));
    connect(importer, SIGNAL(showError(const QString &, unsigned int)),
            &medMessageController::instance(), SLOT(showError(const QString &, unsigned int)));

    medJobManagerL::instance().registerJobItem(importer);
    QThreadPool::globalInstance()->start(importer);
}

void medDatabasePersistentController::showOpeningError(QObject *sender)
{
    medMessageController::instance().showError("Opening item failed.", 3000);
}

/**
* Change the storage location of the database by copy, verify, delete
* @param QString newLocation path of new storage location, must be empty
* @return bool true on success
*/
bool medDatabasePersistentController::moveDatabase(QString newLocation)
{
    // close connection if necessary
    if (this->isConnected())
    {
        getMainConnection().close();
    }

    // now update the datastorage path and make sure to reconnect
    medStorage::setDataLocation(newLocation);

    qDebug() << "Restarting connection...";
    this->createConnection();

    return true;
}

/** override base class */
void medDatabasePersistentController::remove(const medDataIndex &index)
{
    medDatabaseRemover *remover = new medDatabaseRemover(index);
    medMessageProgress *message = medMessageController::instance().showProgress("Removing item");

    connect(remover, SIGNAL(progressed(int)), message, SLOT(setProgress(int)));
    connect(remover, SIGNAL(success(QObject *)), message, SLOT(success()));
    connect(remover, SIGNAL(failure(QObject *)), message, SLOT(failure()));
    connect(remover, SIGNAL(removed(const medDataIndex &)), this, SIGNAL(dataRemoved(medDataIndex)));

    medJobManagerL::instance().registerJobItem(remover);
    QThreadPool::globalInstance()->start(remover);
}

/** Implement base class */
int medDatabasePersistentController::dataSourceId() const
{
    return 1;
}

QPixmap medDatabasePersistentController::thumbnail(const medDataIndex &index) const
{
    QString thumbpath = this->metaData(index, medMetaDataKeys::ThumbnailPath.key());

    QFileInfo fileInfo(thumbpath);
    if (fileInfo.exists())
    {
        return QPixmap(thumbpath);
    }
    return QPixmap();
}

/** Implement base class */
bool medDatabasePersistentController::isPersistent() const
{
    return true;
}

medAbstractData *medDatabasePersistentController::retrieve(const medDataIndex &index, bool readFullData) const
{
    QScopedPointer<medDatabaseReader> reader(new medDatabaseReader(index));
    medMessageProgress *message = medMessageController::instance().showProgress("Opening database item");

    connect(reader.data(), SIGNAL(progressed(int)), message, SLOT(setProgress(int)));
    connect(reader.data(), SIGNAL(success(QObject *)), message, SLOT(success()));
    connect(reader.data(), SIGNAL(failure(QObject *)), message, SLOT(failure()));

    connect(reader.data(), SIGNAL(failure(QObject *)), this, SLOT(showOpeningError(QObject *)));

    medAbstractData* data;
    reader->setReadMode(readFullData ? medDatabaseReader::READ_ALL : medDatabaseReader::READ_INFORMATION);
    data = reader->run();
    return data;
}

void medDatabasePersistentController::removeAll()
{
    qWarning() << "Attempt to remove all item from PERSISTENT dataBase";
}

/** create dataIndices out of partial ids */
medDataIndex medDatabasePersistentController::indexForPatient(int id)
{
    return medDataIndex::makePatientIndex(this->dataSourceId(), id);
}

/**
 * Returns the index of a data given patient, study, and series name
 */
medDataIndex medDatabasePersistentController::indexForPatient(
    const QString &patientName)
{
    QSqlDatabase dbConnection = getThreadSpecificConnection();
    QSqlQuery query(dbConnection);
    QVariant patientId = -1;

    query.prepare("SELECT id FROM patient WHERE name = :name");
    query.bindValue(":name", patientName);

    QMutexLocker mutexLocker(&getDatabaseMutex());

    if (!execQuery(query, __FILE__, __LINE__))
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if (query.first())
    {
        patientId = query.value(0);
        return medDataIndex::makePatientIndex(this->dataSourceId(),
                                              patientId.toInt());
    }

    return medDataIndex();
}

medDataIndex medDatabasePersistentController::indexForPatientID(const QString& patientId)
{
    medDataIndex dataIndex;

    if (!patientId.isEmpty())
    {
        QSqlDatabase dbConnection = getThreadSpecificConnection();
        QSqlQuery query(dbConnection);

        query.prepare("SELECT id FROM patient WHERE patientId = :patientId");
        query.bindValue(":patientId", patientId);

        QMutexLocker mutexLocker(&getDatabaseMutex());

        if (!execQuery(query, __FILE__, __LINE__))
        {
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        }

        if (query.first())
        {
            QVariant patientDbId = query.value(0);
            dataIndex = medDataIndex::makePatientIndex(this->dataSourceId(), patientDbId.toInt());
        }
    }

    return dataIndex;
}

medDataIndex medDatabasePersistentController::indexForStudy(int id)
{
    QSqlDatabase dbConnection = getThreadSpecificConnection();
    QSqlQuery query(dbConnection);

    QVariant patientId = -1;

    query.prepare("SELECT patient FROM study WHERE id = :id");
    query.bindValue(":id", id);

    QMutexLocker mutexLocker(&getDatabaseMutex());

    if (!execQuery(query, __FILE__, __LINE__))
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if (query.first())
        patientId = query.value(0);

    return medDataIndex::makeStudyIndex(this->dataSourceId(), patientId.toInt(),
                                        id);
}

medDataIndex medDatabasePersistentController::indexForStudy(const QString &patientName,
                                                            const QString &studyName)
{
    QSqlDatabase dbConnection = getThreadSpecificConnection();

    medDataIndex index = this->indexForPatient(patientName);
    if (!index.isValid())
        return index;

    QSqlQuery query(dbConnection);

    QVariant patientId = index.patientId();
    QVariant studyId = -1;

    query.prepare("SELECT id FROM study WHERE patient = :id AND name = :name");
    query.bindValue(":id", patientId);
    query.bindValue(":name", studyName);

    QMutexLocker mutexLocker(&getDatabaseMutex());

    if (!execQuery(query, __FILE__, __LINE__))
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if (query.first())
    {
        studyId = query.value(0);
        index.setStudyId(studyId.toInt());
        return index;
    }

    return medDataIndex();
}

medDataIndex medDatabasePersistentController::indexForStudyUID(const QString &patientName,
                                                               const QString &studyInstanceUID)
{
    QSqlDatabase dbConnection = getThreadSpecificConnection();

    medDataIndex index = this->indexForPatient(patientName);
    if (!index.isValid())
        return index;

    QSqlQuery query(dbConnection);

    QVariant patientId = index.patientId();
    QVariant studyId = -1;

    query.prepare("SELECT id FROM study WHERE patient = :id AND uid = :uid");
    query.bindValue(":id", patientId);
    query.bindValue(":uid", studyInstanceUID);

    QMutexLocker mutexLocker(&getDatabaseMutex());

    if (!execQuery(query, __FILE__, __LINE__))
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if (query.first())
    {
        studyId = query.value(0);
        index.setStudyId(studyId.toInt());
        return index;
    }

    return medDataIndex();
}

medDataIndex medDatabasePersistentController::indexForSeries(int id)
{
    QSqlDatabase dbConnection = getThreadSpecificConnection();
    QSqlQuery query(dbConnection);

    QVariant patientId = -1;
    QVariant studyId = -1;

    query.prepare("SELECT study FROM series WHERE id = :id");
    query.bindValue(":id", id);

    QMutexLocker mutexLocker(&getDatabaseMutex());

    if (!execQuery(query, __FILE__, __LINE__))
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if (query.first())
        studyId = query.value(0);

    query.prepare("SELECT patient FROM study WHERE id = :id");
    query.bindValue(":id", studyId);

    if (!execQuery(query, __FILE__, __LINE__))
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if (query.first())
        patientId = query.value(0);

    return medDataIndex::makeSeriesIndex(this->dataSourceId(), patientId.toInt(),
                                         studyId.toInt(), id);
}

medDataIndex medDatabasePersistentController::indexForSeries(
    const QString &patientName, const QString &studyName,
    const QString &seriesName)
{
    QSqlDatabase dbConnection = getThreadSpecificConnection();

    medDataIndex index = this->indexForStudy(patientName, studyName);
    if (!index.isValid())
        return index;

    QSqlQuery query(dbConnection);

    QVariant studyId = index.studyId();

    query.prepare("SELECT id FROM series WHERE study = :id AND name = :name");
    query.bindValue(":id", studyId);
    query.bindValue(":name", seriesName);

    QMutexLocker mutexLocker(&getDatabaseMutex());

    if (!execQuery(query, __FILE__, __LINE__))
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if (query.first())
    {
        QVariant seriesId = query.value(0);
        index.setSeriesId(seriesId.toInt());
        return index;
    }

    return medDataIndex();
}

medDataIndex medDatabasePersistentController::indexForSeriesUID(
    const QString &patientName, const QString &studyInstanceUID,
    const QString &seriesInstanceUID)
{
    QSqlDatabase dbConnection = getThreadSpecificConnection();

    medDataIndex index = this->indexForStudyUID(patientName, studyInstanceUID);
    if (!index.isValid())
        return index;

    QSqlQuery query(dbConnection);

    QVariant studyId = index.studyId();

    query.prepare("SELECT id FROM series WHERE study = :id AND uid = :uid");
    query.bindValue(":id", studyId);
    query.bindValue(":uid", seriesInstanceUID);

    QMutexLocker mutexLocker(&getDatabaseMutex());

    if (!execQuery(query, __FILE__, __LINE__))
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if (query.first())
    {
        QVariant seriesId = query.value(0);
        index.setSeriesId(seriesId.toInt());
        return index;
    }

    return medDataIndex();
}

medDatabasePersistentController::medDatabasePersistentController()
    : d(new medDatabasePersistentControllerPrivate)
{
    d->buildMetaDataLookup();
}

medDatabasePersistentController::~medDatabasePersistentController()
{
    delete d;
}

/**
 * Moves study and its series from one patient to another and returns the list
 * of new indexes
 * @param const medDataIndex & indexStudy The data index of the study to be
 * moved
 * @param const medDataIndex & toPatient The data index to move the study to.
 */
QList<medDataIndex> medDatabasePersistentController::moveStudy(
    const medDataIndex &indexStudy, const medDataIndex &toPatient)
{
    bool result = false;
    QList<medDataIndex> newIndexList;
    medDataIndex newIndex;

    if (indexStudy.isValidForStudy() && toPatient.isValidForPatient())
    {
        QSqlDatabase dbConnection = getThreadSpecificConnection();
        QSqlQuery query(dbConnection);

        query.prepare("UPDATE study SET patient=:patientId WHERE id=:studyId");
        query.bindValue(":patientId", toPatient.patientId());
        query.bindValue(":studyId", indexStudy.studyId());

        QMutexLocker mutexLocker(&getDatabaseMutex());

        result = execQuery(query, __FILE__, __LINE__);

        if (result)
        {
            // we need to update patient id in study index
            newIndex = indexStudy;
            newIndex.setPatientId(toPatient.patientId());

            newIndexList << newIndex;

            // and update patient id in series indexes
            QList<medDataIndex> seriesIndexList = series(indexStudy);
            foreach (medDataIndex newSeriesIndex, seriesIndexList)
            {
                newSeriesIndex.setPatientId(toPatient.patientId());
                newIndexList << newSeriesIndex;
            }
        }
    }

    if (!newIndexList.isEmpty())
    {
        emit metadataModified(indexStudy, medMetaDataKeys::PatientID.key(),
                              QString::number(toPatient.patientId()));
        emit metadataModified(newIndex);
    }
    return newIndexList;
}

/**
 * Moves series from one study to another and returns the new index of the
 * series
 * @param const medDataIndex & indexSeries The data index of the series to be
 * moved
 * @param const medDataIndex & toStudy The data index to move the series to.
 */
medDataIndex medDatabasePersistentController::moveSeries(
    const medDataIndex &indexSeries, const medDataIndex &toStudy)
{
    bool result = false;
    medDataIndex newIndex;

    if (indexSeries.isValidForSeries() && toStudy.isValidForStudy())
    {
        QSqlDatabase dbConnection = getThreadSpecificConnection();
        QSqlQuery query(dbConnection);

        query.prepare("UPDATE series SET study=:studyId  WHERE id=:seriesId");
        query.bindValue(":studyId", toStudy.studyId());
        query.bindValue(":seriesId", indexSeries.seriesId());

        QMutexLocker mutexLocker(&getDatabaseMutex());

        result = execQuery(query, __FILE__, __LINE__);

        if (result)
        {
            newIndex = indexSeries;
            newIndex.setPatientId(toStudy.patientId());
            newIndex.setStudyId(toStudy.studyId());
        }
    }

    emit metadataModified(indexSeries); // to signal the series has been removed
    emit metadataModified(newIndex);    // to signal the series has been added

    return newIndex;
}

/** Get metadata for specific item. Return uninitialized string if not present.
 */
QString medDatabasePersistentController::metaData(const medDataIndex &index,
                                                  const QString &key) const
{
    typedef medDatabasePersistentControllerPrivate::MetaDataMap MetaDataMap;
    typedef medDatabasePersistentControllerPrivate::TableEntryList TableEntryList;

    // Attempt to translate the desired metadata into a table / column entry.
    MetaDataMap::const_iterator it(d->metaDataLookup.find(key));
    if (it == d->metaDataLookup.end())
    {
        return QString();
    }

    QString ret;
    bool isPath = false;
    for (TableEntryList::const_iterator entryIt(it.value().begin());
         entryIt != it.value().end(); ++entryIt)
    {
        const QString tableName = entryIt->table;
        const QString columnName = entryIt->column;
        isPath = entryIt->isPath;

        int id = -1;

        if (tableName == d->T_series && index.isValidForSeries())
        {
            id = index.seriesId();
        }
        else if (tableName == d->T_study && index.isValidForStudy())
        {
            id = index.studyId();
        }
        else if (tableName == d->T_patient && index.isValidForPatient())
        {
            id = index.patientId();
        }
        if (id != -1)
        {
            QSqlDatabase dbConnection = getThreadSpecificConnection();
            QSqlQuery query(dbConnection);

            query.prepare("SELECT " + columnName + " FROM " + tableName +
                          " WHERE id = :id");
            query.bindValue(":id", id);
            QMutexLocker mutexLocker(&getDatabaseMutex());
            execQuery(query, __FILE__, __LINE__);
            if (query.next())
            {
                ret = query.value(0).toString();
                break;
            }
        }
    }

    if (!ret.isEmpty() && isPath)
        ret = medStorage::dataLocation() + ret;

    return ret;
}

/** Set metadata for specific item. Return true on success, false otherwise. */
bool medDatabasePersistentController::setMetaData(const medDataIndex &index,
                                                  const QString &key,
                                                  const QString &value)
{
    typedef medDatabasePersistentControllerPrivate::MetaDataMap MetaDataMap;
    typedef medDatabasePersistentControllerPrivate::TableEntryList TableEntryList;

    // Attempt to translate the desired metadata into a table / column entry.
    MetaDataMap::const_iterator it(d->metaDataLookup.find(key));
    if (it == d->metaDataLookup.end())
    {
        return false;
    }

    bool success(false);

    for (TableEntryList::const_iterator entryIt(it.value().begin());
         entryIt != it.value().end(); ++entryIt)
    {
        const QString tableName = entryIt->table;
        const QString columnName = entryIt->column;

        int id = -1;

        if (tableName == d->T_series && index.isValidForSeries())
        {
            id = index.seriesId();
        }
        else if (tableName == d->T_study && index.isValidForStudy())
        {
            id = index.studyId();
        }
        else if (tableName == d->T_patient && index.isValidForPatient())
        {
            id = index.patientId();
        }
        if (id != -1)
        {
            QSqlDatabase dbConnection = getThreadSpecificConnection();
            QSqlQuery query(dbConnection);

            query.prepare(QString("UPDATE %1 SET %2 = :value WHERE id = :id")
                              .arg(tableName)
                              .arg(columnName));
            query.bindValue(":value", value);
            query.bindValue(":id", id);
            QMutexLocker mutexLocker(&getDatabaseMutex());
            success = execQuery(query, __FILE__, __LINE__);
            if (success)
            {
                break;
            }
        }
    }

    return success;
}

/** Enumerate all studies for given patient*/
QList<medDataIndex> medDatabasePersistentController::studies(
    const medDataIndex &index) const
{
    QSqlDatabase dbConnection = getThreadSpecificConnection();
    QList<medDataIndex> ret;

    if (!index.isValidForPatient())
    {
        qWarning() << "invalid index passed (not patient)";
        return ret;
    }

    QSqlQuery query(dbConnection);
    query.prepare("SELECT id FROM study WHERE patient = :patientId");
    query.bindValue(":patientId", index.patientId());
    QMutexLocker mutexLocker(&getDatabaseMutex());
    execQuery(query, __FILE__, __LINE__);
#if QT_VERSION > 0x0406FF
    ret.reserve(query.size());
#endif
    while (query.next())
    {
        ret.push_back(medDataIndex::makeStudyIndex(
            this->dataSourceId(), index.patientId(), query.value(0).toInt()));
    }
    return ret;
}

/** Enumerate all series for given study*/
QList<medDataIndex> medDatabasePersistentController::series(
    const medDataIndex &index) const
{
    QSqlDatabase dbConnection = getThreadSpecificConnection();
    QList<medDataIndex> ret;

    if (!index.isValidForStudy())
    {
        qWarning() << "invalid index passed";
        return ret;
    }

    QSqlQuery query(dbConnection);
    query.prepare("SELECT id FROM series WHERE study = :studyId");
    query.bindValue(":studyId", index.studyId());
    QMutexLocker mutexLocker(&getDatabaseMutex());
    execQuery(query, __FILE__, __LINE__);
#if QT_VERSION > 0x0406FF
    ret.reserve(query.size());
#endif
    while (query.next())
    {
        ret.push_back(
            medDataIndex::makeSeriesIndex(this->dataSourceId(), index.patientId(),
                                          index.studyId(), query.value(0).toInt()));
    }
    return ret;
}

QStringList medDatabasePersistentController::series(const QString &seriesName, const QString &studyId) const
{
    QSqlDatabase dbConnection = getThreadSpecificConnection();
    QSqlQuery query(dbConnection);
    if (studyId.isEmpty())
    {
        query.prepare ( "SELECT name FROM series WHERE name LIKE '" + seriesName + "%'");
    }
    else
    {
        query.prepare ( "SELECT name FROM series WHERE study = :studyId AND name LIKE '" + seriesName + "%'");
        query.bindValue ( ":studyId", studyId );
    }

    QMutexLocker mutexLocker(&getDatabaseMutex());
    execQuery(query, __FILE__, __LINE__);
    QStringList seriesNames;
    while (query.next())
    {
        QString sname = query.value(0).toString();
        seriesNames << sname;
    }
    return seriesNames;
}

bool medDatabasePersistentController::execQuery(QSqlQuery &query, const char *file,
                                                int line) const
{
    if (!query.exec())
    {
        qDebug() << file << "(" << line << ") :" << DTK_COLOR_FG_RED
                 << query.lastError() << DTK_NO_COLOR;
        qDebug() << "The query was: " << query.lastQuery().simplified();
        return false;
    }

    return true;
}

/** Remove / replace characters to transform into a pathname component. */
QString medDatabasePersistentController::stringForPath(const QString &name) const
{
    QString ret = name.simplified();
    ret.replace(0x00EA, 'e');
    ret.replace(0x00E4, 'a');
    return ret;
}

bool medDatabasePersistentController::contains(const medDataIndex &index) const
{

    if (index.isValid() && index.dataSourceId() == dataSourceId())
    {
        // index is valid and comes from this dataSource
        QVariant patientId = index.patientId();
        QVariant studyId = index.studyId();
        QVariant seriesId = index.seriesId();

        QSqlDatabase dbConnection = getThreadSpecificConnection();
        QSqlQuery query(dbConnection);
        QString fromRequest = "SELECT * FROM patient";
        QString whereRequest = " WHERE patient.id = :id";

        if (studyId != -1)
        {
            fromRequest += " INNER JOIN study ON (patient.id = study.patient)";
            whereRequest += " AND study.id = :stID";
            if (seriesId != -1)
            {
                fromRequest += " INNER JOIN series ON (study.id = series.study)";
                whereRequest += " AND series.id = :seID";
            }
        }
        QString request = fromRequest + whereRequest;

        query.prepare(request);
        query.bindValue(":id", patientId);
        if (studyId != -1)
            query.bindValue(":stID", studyId);
        if (seriesId != -1)
            query.bindValue(":seID", seriesId);

        QMutexLocker mutexLocker(&getDatabaseMutex());

        if (!execQuery(query, __FILE__, __LINE__))
        {
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        }
        if (query.first())
            return true;
    }
    return false;
}

QString medDatabasePersistentController::attachedMetadataFileExists(const medDataIndex &index)
{
    QString resPath = "";
    if (contains(index))
    {
        QVariant seriesId = index.seriesId();
        QSqlDatabase dbConnection = getThreadSpecificConnection();
        QSqlQuery query(dbConnection);
        QString request = "SELECT json_meta_path as json_path FROM series WHERE series.id = :id";
        query.prepare(request);
        query.bindValue(":id", seriesId);
        QMutexLocker mutexLocker(&getDatabaseMutex());
        if (!execQuery(query, __FILE__, __LINE__))
        {
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        }
        if (query.first())
            resPath = query.value("json_path").toString();
    }
    return resPath;
}
