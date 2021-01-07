/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medSqlDbController.h"

#include <medJobManagerL.h>
#include <medMessageController.h>

#include "medDatabaseImporter.h"
#include "medDatabaseReader.h"
#include "medDatabaseRemover.h"
#include "medSettingsManager.h"
#include "medStorage.h"

class medSqlDbControllerPrivate
{
public:
    void buildMetaDataLookup();
    bool isConnected;
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

const QString medSqlDbControllerPrivate::T_series = "series";
const QString medSqlDbControllerPrivate::T_study = "study";
const QString medSqlDbControllerPrivate::T_patient = "patient";

void medSqlDbControllerPrivate::buildMetaDataLookup()
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
}

const QSqlDatabase &medSqlDbController::database(void) const
{
    return m_database;
}

/** create dataIndices out of partial ids */
medDataIndex medSqlDbController::indexForPatient(int id)
{
    return medDataIndex::makePatientIndex(this->dataSourceId(), id);
}

/**
 * Returns the index of a data given patient, study, and series name
 */
medDataIndex medSqlDbController::indexForPatient(
    const QString &patientName)
{
    QSqlQuery query(m_database);
    QVariant patientId = -1;

    query.prepare("SELECT id FROM patient WHERE name = :name");
    query.bindValue(":name", patientName);

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

medDataIndex medSqlDbController::indexForStudy(int id)
{
    QSqlQuery query(m_database);

    QVariant patientId = -1;

    query.prepare("SELECT patient FROM study WHERE id = :id");
    query.bindValue(":id", id);

    if (!execQuery(query, __FILE__, __LINE__))
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if (query.first())
        patientId = query.value(0);

    return medDataIndex::makeStudyIndex(this->dataSourceId(), patientId.toInt(),
                                        id);
}

medDataIndex medSqlDbController::indexForStudy(const QString &patientName,
                                               const QString &studyName)
{
    medDataIndex index = this->indexForPatient(patientName);
    if (!index.isValid())
        return index;

    QSqlQuery query(m_database);

    QVariant patientId = index.patientId();
    QVariant studyId = -1;

    query.prepare("SELECT id FROM study WHERE patient = :id AND name = :name");
    query.bindValue(":id", patientId);
    query.bindValue(":name", studyName);

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

medDataIndex medSqlDbController::indexForSeries(int id)
{
    QSqlQuery query(m_database);

    QVariant patientId = -1;
    QVariant studyId = -1;

    query.prepare("SELECT study FROM series WHERE id = :id");
    query.bindValue(":id", id);

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

medDataIndex medSqlDbController::indexForSeries(
    const QString &patientName, const QString &studyName,
    const QString &seriesName)
{
    medDataIndex index = this->indexForStudy(patientName, studyName);
    if (!index.isValid())
        return index;

    QSqlQuery query(m_database);

    QVariant studyId = index.studyId();

    query.prepare("SELECT id FROM series WHERE study = :id AND name = :name");
    query.bindValue(":id", studyId);
    query.bindValue(":name", seriesName);

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

/**
 * Status of connection
 * @return bool true on success
 */
bool medSqlDbController::isConnected() const
{
    return d->isConnected;
}

void medSqlDbController::setConnected(bool flag)
{
    d->isConnected = flag;
}

medSqlDbController::medSqlDbController()
    : d(new medSqlDbControllerPrivate)
{
    d->buildMetaDataLookup();
    d->isConnected = false;
}

medSqlDbController::~medSqlDbController()
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
QList<medDataIndex> medSqlDbController::moveStudy(
    const medDataIndex &indexStudy, const medDataIndex &toPatient)
{
    QSqlQuery query(m_database);

    bool result = false;
    QList<medDataIndex> newIndexList;
    medDataIndex newIndex;

    if (indexStudy.isValidForStudy() && toPatient.isValidForPatient())
    {
        query.prepare("UPDATE study SET patient=:patientId WHERE id=:studyId");
        query.bindValue(":patientId", toPatient.patientId());
        query.bindValue(":studyId", indexStudy.studyId());

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
medDataIndex medSqlDbController::moveSeries(
    const medDataIndex &indexSeries, const medDataIndex &toStudy)
{
    QSqlQuery query(m_database);

    bool result = false;
    medDataIndex newIndex;

    if (indexSeries.isValidForSeries() && toStudy.isValidForStudy())
    {
        query.prepare("UPDATE series SET study=:studyId  WHERE id=:seriesId");
        query.bindValue(":studyId", toStudy.studyId());
        query.bindValue(":seriesId", indexSeries.seriesId());

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
QString medSqlDbController::metaData(const medDataIndex &index,
                                     const QString &key) const
{
    typedef medSqlDbControllerPrivate::MetaDataMap MetaDataMap;
    typedef medSqlDbControllerPrivate::TableEntryList TableEntryList;

    QSqlQuery query(m_database);

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
            query.prepare("SELECT " + columnName + " FROM " + tableName +
                          " WHERE id = :id");
            query.bindValue(":id", id);
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
bool medSqlDbController::setMetaData(const medDataIndex &index,
                                     const QString &key,
                                     const QString &value)
{
    typedef medSqlDbControllerPrivate::MetaDataMap MetaDataMap;
    typedef medSqlDbControllerPrivate::TableEntryList TableEntryList;

    QSqlQuery query(m_database);

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
            query.prepare(QString("UPDATE %1 SET %2 = :value WHERE id = :id")
                              .arg(tableName)
                              .arg(columnName));
            query.bindValue(":value", value);
            query.bindValue(":id", id);
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
QList<medDataIndex> medSqlDbController::studies(
    const medDataIndex &index) const
{
    QList<medDataIndex> ret;

    if (!index.isValidForPatient())
    {
        qWarning() << "invalid index passed (not patient)";
        return ret;
    }

    QSqlQuery query(m_database);
    query.prepare("SELECT id FROM study WHERE patient = :patientId");
    query.bindValue(":patientId", index.patientId());
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
QList<medDataIndex> medSqlDbController::series(
    const medDataIndex &index) const
{
    QList<medDataIndex> ret;

    if (!index.isValidForStudy())
    {
        qWarning() << "invalid index passed";
        return ret;
    }

    QSqlQuery query(m_database);
    query.prepare("SELECT id FROM series WHERE study = :studyId");
    query.bindValue(":studyId", index.studyId());
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

bool medSqlDbController::execQuery(QSqlQuery &query, const char *file,
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
QString medSqlDbController::stringForPath(const QString &name) const
{
    QString ret = name.simplified();
    ret.replace(0x00EA, 'e');
    ret.replace(0x00E4, 'a');
    return ret;
}

bool medSqlDbController::contains(const medDataIndex &index) const
{
    if (index.isValid() && index.dataSourceId() == dataSourceId())
    {
        // index is valid and comes from this dataSource
        QVariant patientId = index.patientId();
        QVariant studyId = index.studyId();
        QVariant seriesId = index.seriesId();

        QSqlQuery query(m_database);
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

        if (!execQuery(query, __FILE__, __LINE__))
        {
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        }
        if (query.first())
            return true;
    }
    return false;
}
