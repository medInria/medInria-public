/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medPgRemoteDbController.h"
#include "medDatabaseImporter.h"
#include "medDatabaseReader.h"
#include "medDatabaseRemover.h"
#include "medStorage.h"

#include <medJobManagerL.h>
#include <medMessageController.h>

class medLocalDbControllerPrivate
{
public:
    void buildMetaDataLookup();
    bool isConnected;
    struct TableEntry {
        TableEntry( QString t, QString c, bool isPath_ = false ) : table(t), column(c), isPath(isPath_) {}
        QString table;
        QString column;
        bool isPath;
    };
    typedef QList<TableEntry> TableEntryList;
    typedef QHash< QString , TableEntryList > MetaDataMap;

    MetaDataMap metaDataLookup;
    // Reusable table names.
    static const QString T_series ;
    static const QString T_study ;
    static const QString T_patient ;
};

const QString medLocalDbControllerPrivate::T_series = "series";
const QString medLocalDbControllerPrivate::T_study = "study";
const QString medLocalDbControllerPrivate::T_patient = "patient";

void medLocalDbControllerPrivate::buildMetaDataLookup()
{
// The table defines the mapping between metadata in the medAbstractData and the database tables.
    metaDataLookup.insert(medMetaDataKeys::ThumbnailPath.key(),
                          TableEntryList() << TableEntry(T_series, "thumbnail", true)
                          << TableEntry(T_study, "thumbnail", true)
                          << TableEntry(T_patient, "thumbnail", true) );

//Patient data
    metaDataLookup.insert(medMetaDataKeys::PatientName.key(),
        TableEntryList() << TableEntry(T_patient, "name") );
    metaDataLookup.insert(medMetaDataKeys::Gender.key(),
        TableEntryList() << TableEntry(T_patient, "gender") );
    metaDataLookup.insert(medMetaDataKeys::BirthDate.key(),
        TableEntryList() << TableEntry(T_patient, "birthdate") );
    metaDataLookup.insert(medMetaDataKeys::PatientID.key(),
        TableEntryList() << TableEntry(T_patient, "patientId") );

//Study Data
    metaDataLookup.insert(medMetaDataKeys::StudyDescription.key(),
        TableEntryList() << TableEntry(T_study, "name") );
    metaDataLookup.insert(medMetaDataKeys::StudyInstanceUID.key(),
        TableEntryList() << TableEntry(T_study, "uid") );
    metaDataLookup.insert(medMetaDataKeys::StudyID.key(),
        TableEntryList() << TableEntry(T_study, "studyId") );
//Series Data
    metaDataLookup.insert(medMetaDataKeys::Size.key(),
        TableEntryList() << TableEntry(T_series, "size") );
    metaDataLookup.insert(medMetaDataKeys::SeriesDescription.key(),
        TableEntryList() << TableEntry(T_series, "name") );
    metaDataLookup.insert(medMetaDataKeys::SeriesInstanceUID.key(),
        TableEntryList() << TableEntry(T_series, "uid") );
    metaDataLookup.insert(medMetaDataKeys::SeriesID.key(),
        TableEntryList() << TableEntry(T_series, "seriesId") );
    metaDataLookup.insert(medMetaDataKeys::Orientation.key(),
        TableEntryList() << TableEntry(T_series, "orientation") );
    metaDataLookup.insert(medMetaDataKeys::SeriesNumber.key(),
        TableEntryList() << TableEntry(T_series, "seriesNumber") );
    metaDataLookup.insert(medMetaDataKeys::SequenceName.key(),
        TableEntryList() << TableEntry(T_series, "sequenceName") );
    metaDataLookup.insert(medMetaDataKeys::SliceThickness.key(),
        TableEntryList() << TableEntry(T_series, "sliceThickness") );
    metaDataLookup.insert(medMetaDataKeys::Rows.key(),
        TableEntryList() << TableEntry(T_series, "rows") );
    metaDataLookup.insert(medMetaDataKeys::Columns.key(),
        TableEntryList() << TableEntry(T_series, "columns") );
    metaDataLookup.insert(medMetaDataKeys::Age.key(),
        TableEntryList() << TableEntry(T_series, "age") );
    metaDataLookup.insert(medMetaDataKeys::Description.key(),
        TableEntryList() << TableEntry(T_series, "description") );
    metaDataLookup.insert(medMetaDataKeys::Modality.key(),
        TableEntryList() << TableEntry(T_series, "modality") );
    metaDataLookup.insert(medMetaDataKeys::Protocol.key(),
        TableEntryList() << TableEntry(T_series, "protocol") );
    metaDataLookup.insert(medMetaDataKeys::Comments.key(),
        TableEntryList() << TableEntry(T_series, "comments") );
    metaDataLookup.insert(medMetaDataKeys::Status.key(),
        TableEntryList() << TableEntry(T_series, "status") );
    metaDataLookup.insert(medMetaDataKeys::AcquisitionDate.key(),
        TableEntryList() << TableEntry(T_series, "acquisitiondate") );
    metaDataLookup.insert(medMetaDataKeys::ImportationDate.key(),
        TableEntryList() << TableEntry(T_series, "importationdate") );
    metaDataLookup.insert(medMetaDataKeys::Referee.key(),
        TableEntryList() << TableEntry(T_series, "referee") );
    metaDataLookup.insert(medMetaDataKeys::Performer.key(),
        TableEntryList() << TableEntry(T_series, "performer") );
    metaDataLookup.insert(medMetaDataKeys::Institution.key(),
        TableEntryList() << TableEntry(T_series, "institution") );
    metaDataLookup.insert(medMetaDataKeys::Report.key(),
        TableEntryList() << TableEntry(T_series, "report") );
    metaDataLookup.insert(medMetaDataKeys::Origin.key(),
        TableEntryList() << TableEntry(T_series, "origin") );
    metaDataLookup.insert(medMetaDataKeys::FlipAngle.key(),
        TableEntryList() << TableEntry(T_series, "flipAngle") );
    metaDataLookup.insert(medMetaDataKeys::EchoTime.key(),
        TableEntryList() << TableEntry(T_series, "echoTime") );
    metaDataLookup.insert(medMetaDataKeys::RepetitionTime.key(),
        TableEntryList() << TableEntry(T_series, "repetitionTime") );
    metaDataLookup.insert(medMetaDataKeys::AcquisitionTime.key(),
        TableEntryList() << TableEntry(T_series, "acquisitionTime") );
}

medPgRemoteDbController * medPgRemoteDbController::s_instance = NULL;

medPgRemoteDbController* medPgRemoteDbController::instance() {
    if ( ! s_instance) {
        s_instance = new medPgRemoteDbController();
    }
    return s_instance;
}


const QSqlDatabase& medPgRemoteDbController::database(void) const
{
    return m_database;
}

bool medPgRemoteDbController::createConnection(void)
{
    medStorage::mkpath(medStorage::dataLocation() + "/");

    m_database = QSqlDatabase::database("sqlite");
    if (!m_database.isValid())
    {
        m_database = QSqlDatabase::addDatabase("QSQLITE", "sqlite");
    }
    m_database.setDatabaseName(medStorage::dataLocation() + "/" + "db");

    if (!m_database.open())
    {
        qDebug() << DTK_COLOR_FG_RED << "Cannot open database: unable to establish a database connection." << DTK_NO_COLOR;
        return false;
    }
    else
    {
        qDebug() << "Database opened at: " << qPrintable(m_database.databaseName());
        d->isConnected = true;
    }

    if(    !createPatientTable()
        || !createStudyTable()
        || !createSeriesTable()
        || !updateFromNoVersionToVersion1())
    {
        return false;
    }

    // optimize speed of sqlite db
    QSqlQuery query(m_database);
    if (!(query.prepare(QLatin1String("PRAGMA synchronous = 0"))
          && EXEC_QUERY(query))) {
        qDebug() << "Could not set sqlite synchronous mode to asynchronous mode.";
    }
    if (!(query.prepare(QLatin1String("PRAGMA journal_mode=wal"))
          && EXEC_QUERY(query))) {
        qDebug() << "Could not set sqlite write-ahead-log journal mode";
    }

    return true;
}

bool medPgRemoteDbController::closeConnection(void)
{
    m_database.close();
    QSqlDatabase::removeDatabase("QSQLITE");
    d->isConnected = false;
    return true;
}

/** create dataIndices out of partial ids */
medDataIndex medPgRemoteDbController::indexForPatient(int id)
{
    return medDataIndex::makePatientIndex(this->dataSourceId(), id);
}

/**
* Returns the index of a data given patient, study, and series name
*/
medDataIndex medPgRemoteDbController::indexForPatient (const QString &patientName)
{
    QSqlQuery query(m_database);
    QVariant patientId = -1;

    query.prepare("SELECT id FROM patient WHERE name = :name");
    query.bindValue(":name", patientName);

    if(!EXEC_QUERY(query))
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if(query.first()) {
        patientId = query.value(0);
        return medDataIndex::makePatientIndex(this->dataSourceId(), patientId.toInt());
    }

    return medDataIndex();
}

medDataIndex medPgRemoteDbController::indexForStudy(int id)
{
    QSqlQuery query(m_database);

    QVariant patientId = -1;

    query.prepare("SELECT patient FROM study WHERE id = :id");
    query.bindValue(":id", id);

    if(!EXEC_QUERY(query))
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if(query.first())
        patientId = query.value(0);

    return medDataIndex::makeStudyIndex(this->dataSourceId(), patientId.toInt(), id);
}

medDataIndex medPgRemoteDbController::indexForStudy(const QString &patientName, const QString &studyName)
{
    medDataIndex index = this->indexForPatient(patientName);
    if (!index.isValid())
        return index;

    QSqlQuery query(m_database);

    QVariant patientId = index.patientId();
    QVariant studyId   = -1;

    query.prepare("SELECT id FROM study WHERE patient = :id AND name = :name");
    query.bindValue(":id",   patientId);
    query.bindValue(":name", studyName);

    if(!EXEC_QUERY(query))
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if(query.first()) {
        studyId = query.value(0);
        index.setStudyId(studyId.toInt());
        return index;
    }

    return medDataIndex();
}

medDataIndex medPgRemoteDbController::indexForSeries(int id)
{
    QSqlQuery query(m_database);

    QVariant patientId = -1;
    QVariant   studyId = -1;

    query.prepare("SELECT study FROM series WHERE id = :id");
    query.bindValue(":id", id);

    if(!EXEC_QUERY(query))
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if(query.first())
        studyId = query.value(0);

    query.prepare("SELECT patient FROM study WHERE id = :id");
    query.bindValue(":id", studyId);

    if(!EXEC_QUERY(query))
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if(query.first())
        patientId = query.value(0);

    return medDataIndex::makeSeriesIndex(this->dataSourceId(), patientId.toInt(), studyId.toInt(), id);
}

medDataIndex medPgRemoteDbController::indexForSeries(const QString &patientName, const QString &studyName,
                                                       const QString &seriesName)
{
    medDataIndex index = this->indexForStudy(patientName, studyName);
    if (!index.isValid())
        return index;

    QSqlQuery query(m_database);

    QVariant studyId   = index.studyId();

    query.prepare("SELECT id FROM series WHERE study = :id AND name = :name");
    query.bindValue(":id",   studyId);
    query.bindValue(":name", seriesName);

    if(!EXEC_QUERY(query))
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    if(query.first()) {
        QVariant seriesId = query.value(0);
        index.setSeriesId(seriesId.toInt());
        return index;
    }

    return medDataIndex();
}

bool medPgRemoteDbController::createPatientTable()
{
    QSqlQuery query(m_database);

    if( ! query.prepare(
                "CREATE TABLE IF NOT EXISTS patient ("
                " id       INTEGER PRIMARY KEY,"
                " name        TEXT,"
                " thumbnail   TEXT,"
                " birthdate   TEXT,"
                " gender      TEXT,"
                " patientId   TEXT"
                ");"
                ) )
    {
        qDebug() << query.lastError();
    }
    return EXEC_QUERY(query);
}

bool medPgRemoteDbController::createStudyTable()
{
    QSqlQuery query(m_database);

    return query.prepare(
                "CREATE TABLE IF NOT EXISTS study ("
                " id        INTEGER      PRIMARY KEY,"
                " patient   INTEGER," // FOREIGN KEY
                " name         TEXT,"
                " uid          TEXT,"
                " thumbnail    TEXT,"
                " studyId      TEXT"
                ");"
                ) && EXEC_QUERY(query);
}

bool medPgRemoteDbController::createSeriesTable()
{
    QSqlQuery query(m_database);

    query.prepare(
                "CREATE TABLE IF NOT EXISTS series ("
                " id       INTEGER      PRIMARY KEY,"
                " study    INTEGER," // FOREIGN KEY
                " size     INTEGER,"
                " name            TEXT,"
                " path            TEXT,"
                " uid             TEXT,"
                " seriesId        TEXT,"
                " orientation     TEXT,"
                " seriesNumber    TEXT,"
                " sequenceName    TEXT,"
                " sliceThickness  TEXT,"
                " rows            TEXT,"
                " columns         TEXT,"
                " thumbnail       TEXT,"
                " age             TEXT,"
                " description     TEXT,"
                " modality        TEXT,"
                " protocol        TEXT,"
                " comments        TEXT,"
                " status          TEXT,"
                " acquisitiondate TEXT,"
                " importationdate TEXT,"
                " referee         TEXT,"
                " performer       TEXT,"
                " institution     TEXT,"
                " report          TEXT,"
                " origin          TEXT,"
                " flipAngle       TEXT,"
                " echoTime        TEXT,"
                " repetitionTime  TEXT,"
                " acquisitionTime TEXT"
                ");"
                ) && EXEC_QUERY(query);

    // Get all the information about the table columns
    query.prepare("PRAGMA table_info(series)");
    if ( !EXEC_QUERY(query) )
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    }

    this->addTextColumnToSeriesTableIfNeeded(query, "origin");
    this->addTextColumnToSeriesTableIfNeeded(query, "flipAngle");
    this->addTextColumnToSeriesTableIfNeeded(query, "echoTime");
    this->addTextColumnToSeriesTableIfNeeded(query, "repetitionTime");
    this->addTextColumnToSeriesTableIfNeeded(query, "acquisitionTime");

    return true;
}

void medPgRemoteDbController::addTextColumnToSeriesTableIfNeeded(QSqlQuery query, QString columnName)
{
    bool isColumnThere = false;
    query.first();

    while ( query.next() )
    {
        if (query.value(1).toString() == columnName)
        {
            isColumnThere = true;
        }
    }

    // If columnName is not defined in the db series table, add it.
    if (!isColumnThere)
    {
        query.prepare("ALTER TABLE series ADD COLUMN "+columnName+" TEXT");
        EXEC_QUERY(query);
    }
}

bool medPgRemoteDbController::updateFromNoVersionToVersion1()
{
    // Updates the DB schema from the original, un-versioned schema, to the
    // version 1 schema:
    // - Table series gains a column isIndexed
    // - Field image.isIndexed is migrated to series.isIndexed
    // - Field image.path is migrated to series.path if image.isIndexed is true
    // - Table image is removed
    // We to do this in a transaction so that we won't mess up the DB if for
    // whatever reason the app/computer crashes, and we'll just try again on the
    // next launch.

    QSqlQuery q(m_database);

    if ( ! (q.exec("PRAGMA user_version") && q.first()))
    {
        qWarning("medPgRemoteDbController: Testing DB version for upgrade failed.");
        qDebug() << q.lastError();
        return false;
    }

    if ( ! q.exec("BEGIN EXCLUSIVE TRANSACTION"))
    {
        qWarning("medPgRemoteDbController: Could not begin transaction.");
        qDebug() << q.lastError();
        return false;
    }

    // Test for isIndexed column to series
    if ( ! (q.exec("SELECT sql FROM sqlite_master WHERE type=='table' AND tbl_name=='series'") && q.first()))
    {
        qWarning("medPgRemoteDbController: Could not test for `isIndexed` column in table `series`.");
        qDebug() << q.lastError();
        return false;
    }

    bool missingColumn = (q.value(0).toString().indexOf("isIndexed") == -1);

    if( missingColumn )
    {
        // Add isIndexed column if missing
        if ( ! q.exec("ALTER TABLE series ADD COLUMN isIndexed BOOLEAN"))
        {
            qWarning("medPgRemoteDbController: Could not add `isIndexed` column to table `series`.");
            qDebug() << q.lastError();
            return false;
        }
    }

    // If the image column does not exist, the database is already a new version,
    // and user_version should be set without trying to access the image column from old versions.
    if (q.value(0).toString().indexOf("image") != -1)
    {
        // Migrate path and isIndexed fields from table `image` to `series`
        if ( ! q.exec("SELECT id,path FROM image WHERE isIndexed == 'true'"))
        {
            qWarning("medPgRemoteDbController: getting a list of path from the `image` table failed.");
            qDebug() << q.lastError();
            return false;
        }
        QHash<int, QStringList> imagePaths;
        while(q.next())
        {
            imagePaths[q.value(0).toInt()] += q.value(0).toString();
        }
        foreach(int id, imagePaths.keys())
        {
            q.prepare("UPDATE series SET path==:paths,isIndexed='true' WHERE id==:seriesId");
            q.bindValue(":paths", imagePaths[id].join(";"));
            q.bindValue(":seriesId", id);
            if (! q.exec())
            {
                qWarning("medPgRemoteDbController: updating the `series` table failed for id %d.", id);
                qDebug() << q.lastError();
                return false;
            }
        }

        // Delete table `image`
        if ( ! q.exec("DROP TABLE image"))
        {
            qWarning("medPgRemoteDbController: could not drop table `image`.");
            qDebug() << q.lastError();
            return false;
        }
    }

    // finally, update DB version
    if ( ! q.exec("PRAGMA user_version = 1"))
    {
        qWarning("medPgRemoteDbController: updating DB version to 1 after upgrade failed.");
        qDebug() << q.lastError();
        return false;
    }

    if ( ! q.exec("END TRANSACTION"))
    {
        qWarning("medPgRemoteDbController: Could not end transaction.");
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

/**
* Status of connection
* @return bool true on success
*/
bool medPgRemoteDbController::isConnected() const
{
    return d->isConnected;
}

medPgRemoteDbController::medPgRemoteDbController() : d(new medLocalDbControllerPrivate)
{
    d->buildMetaDataLookup();
    d->isConnected = false;
}

medPgRemoteDbController::~medPgRemoteDbController()
{
    delete d;
}

/**
 * Moves study and its series from one patient to another and returns the list of new indexes
 * @param const medDataIndex & indexStudy The data index of the study to be moved
 * @param const medDataIndex & toPatient The data index to move the study to.
 */
QList<medDataIndex> medPgRemoteDbController::moveStudy( const medDataIndex& indexStudy, const medDataIndex& toPatient)
{
    QSqlQuery query(m_database);

    bool result = false;
    QList<medDataIndex> newIndexList;
    medDataIndex newIndex;

    if(indexStudy.isValidForStudy() && toPatient.isValidForPatient())
    {
        query.prepare("UPDATE study SET patient=:patientId WHERE id=:studyId");
        query.bindValue(":patientId", toPatient.patientId());
        query.bindValue(":studyId", indexStudy.studyId());

        result = EXEC_QUERY(query);

        if(result)
        {
            // we need to update patient id in study index
            newIndex = indexStudy;
            newIndex.setPatientId(toPatient.patientId());

            newIndexList << newIndex;

            // and update patient id in series indexes
            QList<medDataIndex> seriesIndexList = series(indexStudy);
            foreach(medDataIndex newSeriesIndex, seriesIndexList)
            {
                newSeriesIndex.setPatientId(toPatient.patientId());
                newIndexList << newSeriesIndex;
            }
        }
    }

    if(!newIndexList.isEmpty())
    {
        emit metadataModified(indexStudy, medMetaDataKeys::PatientID.key(), QString::number(toPatient.patientId()));
        emit metadataModified(newIndex);
    }
    return newIndexList;
}

/**
 * Moves series from one study to another and returns the new index of the series
 * @param const medDataIndex & indexSeries The data index of the series to be moved
 * @param const medDataIndex & toStudy The data index to move the series to.
 */
medDataIndex medPgRemoteDbController::moveSeries( const medDataIndex& indexSeries, const medDataIndex& toStudy)
{
    QSqlQuery query(m_database);

    bool result = false;
    medDataIndex newIndex;

    if(indexSeries.isValidForSeries() && toStudy.isValidForStudy())
    {
        query.prepare("UPDATE series SET study=:studyId  WHERE id=:seriesId");
        query.bindValue(":studyId", toStudy.studyId());
        query.bindValue(":seriesId", indexSeries.seriesId());

        result = EXEC_QUERY(query);

        if(result)
        {
            newIndex = indexSeries;
            newIndex.setPatientId(toStudy.patientId());
            newIndex.setStudyId(toStudy.studyId());
        }
    }

    emit metadataModified(indexSeries); // to signal the series has been removed
    emit metadataModified(newIndex);   // to signal the series has been added

    return newIndex;
}

/** Get metadata for specific item. Return uninitialized string if not present. */
QString medPgRemoteDbController::metaData(const medDataIndex& index,const QString& key) const
{
    typedef medLocalDbControllerPrivate::MetaDataMap MetaDataMap;
    typedef medLocalDbControllerPrivate::TableEntryList TableEntryList;

    QSqlQuery query(m_database);

    // Attempt to translate the desired metadata into a table / column entry.
    MetaDataMap::const_iterator it(d->metaDataLookup.find(key));
    if (it == d->metaDataLookup.end() )
    {
        return QString();
    }

    QString ret;
    bool isPath = false;
    for ( TableEntryList::const_iterator entryIt(it.value().begin());
        entryIt != it.value().end(); ++entryIt )
    {
        const QString tableName = entryIt->table;
        const QString columnName = entryIt->column;
        isPath = entryIt->isPath;

        int id = -1;

        if ( tableName == d->T_series && index.isValidForSeries() )
        {
            id = index.seriesId();
        }
        else if ( tableName == d->T_study && index.isValidForStudy() )
        {
            id = index.studyId();
        }
        else if ( tableName == d->T_patient && index.isValidForPatient() )
        {
            id = index.patientId();
        }
        if ( id != -1 )
        {
            query.prepare("SELECT " + columnName + " FROM " + tableName + " WHERE id = :id");
            query.bindValue(":id", id);
            EXEC_QUERY(query);
            if ( query.next() )
            {
                ret = query.value(0).toString();
                break;
            }
        }
    }

    if ( !ret.isEmpty() && isPath )
        ret = medStorage::dataLocation() + ret;

    return ret;
}

/** Set metadata for specific item. Return true on success, false otherwise. */
bool medPgRemoteDbController::setMetaData( const medDataIndex& index, const QString& key, const QString& value )
{
    typedef medLocalDbControllerPrivate::MetaDataMap MetaDataMap;
    typedef medLocalDbControllerPrivate::TableEntryList TableEntryList;

    QSqlQuery query(m_database);

    // Attempt to translate the desired metadata into a table / column entry.
    MetaDataMap::const_iterator it(d->metaDataLookup.find(key));
    if (it == d->metaDataLookup.end() ) {
        return false;
    }

    bool success (false);

    for ( TableEntryList::const_iterator entryIt(it.value().begin() ); entryIt != it.value().end(); ++entryIt ) {

        const QString tableName = entryIt->table;
        const QString columnName = entryIt->column;

        int id = -1;

        if ( tableName == d->T_series && index.isValidForSeries() )
        {
            id = index.seriesId();
        }
        else if ( tableName == d->T_study && index.isValidForStudy() )
        {
            id = index.studyId();
        }
        else if ( tableName == d->T_patient && index.isValidForPatient() )
        {
            id = index.patientId();
        }
        if ( id != -1 )
        {
            query.prepare(QString("UPDATE %1 SET %2 = :value WHERE id = :id")
                .arg(tableName).arg(columnName) );
            query.bindValue(":value", value);
            query.bindValue(":id", id);
            success = EXEC_QUERY(query);
            if ( success )
            {
                break;
            }
        }
    }

    return success;
}

/** Enumerate all patients stored in this DB*/
QList<medDataIndex> medPgRemoteDbController::patients() const
{
    QList<medDataIndex> ret;
    QSqlQuery query(m_database);
    query.prepare("SELECT id FROM patient");
    EXEC_QUERY(query);
#if QT_VERSION > 0x0406FF
    ret.reserve( query.size() );
#endif
    while( query.next() ){
        ret.push_back( medDataIndex::makePatientIndex(this->dataSourceId(), query.value(0).toInt()));
    }
    return ret;
}

/** Enumerate all studies for given patient*/
QList<medDataIndex> medPgRemoteDbController::studies( const medDataIndex& index ) const
{
    QList<medDataIndex> ret;

    if ( !index.isValidForPatient() )
    {
        qWarning() << "invalid index passed (not patient)";
        return ret;
    }

    QSqlQuery query(m_database);
    query.prepare("SELECT id FROM study WHERE patient = :patientId");
    query.bindValue(":patientId", index.patientId());
    EXEC_QUERY(query);
#if QT_VERSION > 0x0406FF
    ret.reserve( query.size() );
#endif
    while( query.next() ){
        ret.push_back( medDataIndex::makeStudyIndex(this->dataSourceId(), index.patientId(), query.value(0).toInt()));
    }
    return ret;
}

/** Enumerate all series for given study*/
QList<medDataIndex> medPgRemoteDbController::series( const medDataIndex& index) const
{
    QList<medDataIndex> ret;

    if ( !index.isValidForStudy() )
    {
        qWarning() << "invalid index passed";
        return ret;
    }

    QSqlQuery query(m_database);
    query.prepare("SELECT id FROM series WHERE study = :studyId");
    query.bindValue(":studyId", index.studyId());
    EXEC_QUERY(query);
#if QT_VERSION > 0x0406FF
    ret.reserve( query.size() );
#endif
    while( query.next() ){
        ret.push_back( medDataIndex::makeSeriesIndex(this->dataSourceId(), index.patientId(), index.studyId(), query.value(0).toInt()));
    }
    return ret;
}

bool medPgRemoteDbController::execQuery(QSqlQuery& query, const char* file, int line) const
{
    if (!query.exec())
    {
        qDebug() << file << "(" << line << ") :" << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        qDebug() << "The query was: " << query.lastQuery().simplified();
        return false;
    }

    return true;
}

/** Remove / replace characters to transform into a pathname component. */
QString medPgRemoteDbController::stringForPath( const QString & name ) const
{
    QString ret = name.simplified();
    ret.replace(0x00EA, 'e');
    ret.replace (0x00E4, 'a');
    return ret;
}

bool medPgRemoteDbController::contains(const medDataIndex &index) const
{
    if (index.isValid() && index.dataSourceId() == dataSourceId())
    {
        //index is valid and comes from this dataSource
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
                whereRequest +=  " AND series.id = :seID";
            }
        }
        QString request = fromRequest + whereRequest;

        query.prepare(request);
        query.bindValue(":id", patientId);
        if (studyId != -1)
            query.bindValue(":stID", studyId);
        if (seriesId != -1)
            query.bindValue(":seID", seriesId);

        if(!EXEC_QUERY(query))
        {
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        }
        if(query.first())
            return true;

    }
    return false;
}
