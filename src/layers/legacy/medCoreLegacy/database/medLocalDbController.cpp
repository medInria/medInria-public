/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medLocalDbController.h"
#include "medDatabaseImporter.h"
#include "medDatabaseReader.h"
#include "medDatabaseRemover.h"
#include "medStorage.h"

#include <medJobManagerL.h>
#include <medMessageController.h>

medLocalDbController *medLocalDbController::s_instance = NULL;

medLocalDbController *medLocalDbController::instance()
{
    if (!s_instance)
    {
        s_instance = new medLocalDbController();
    }
    return s_instance;
}

medLocalDbController::medLocalDbController() : medDatabasePersistentController()
{
}

bool medLocalDbController::createConnection(void)
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
        setConnected(true);
    }

    if (!createPatientTable() || !createStudyTable() || !createSeriesTable() || !updateFromNoVersionToVersion1())
    {
        return false;
    }

    // optimize speed of sqlite db
    QSqlQuery query(m_database);
    if (!(query.prepare(QLatin1String("PRAGMA synchronous = 0")) && execQuery(query, __FILE__, __LINE__)))
    {
        qDebug() << "Could not set sqlite synchronous mode to asynchronous mode.";
    }
    if (!(query.prepare(QLatin1String("PRAGMA journal_mode=wal")) && execQuery(query, __FILE__, __LINE__)))
    {
        qDebug() << "Could not set sqlite write-ahead-log journal mode";
    }

    return true;
}

bool medLocalDbController::closeConnection(void)
{
    m_database.close();
    QSqlDatabase::removeDatabase("QSQLITE");
    setConnected(false);
    return true;
}

bool medLocalDbController::createPatientTable()
{
    QSqlQuery query(m_database);

    if (!query.prepare(
            "CREATE TABLE IF NOT EXISTS patient ("
            " id       INTEGER PRIMARY KEY,"
            " name        TEXT,"
            " thumbnail   TEXT,"
            " birthdate   TEXT,"
            " gender      TEXT,"
            " patientId   TEXT"
            ");"))
    {
        qDebug() << query.lastError();
    }
    return execQuery(query, __FILE__, __LINE__);
}

bool medLocalDbController::createStudyTable()
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
               ");") &&
           execQuery(query, __FILE__, __LINE__);
}

bool medLocalDbController::createSeriesTable()
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
        ");") &&
        execQuery(query, __FILE__, __LINE__);

    // Get all the information about the table columns
    query.prepare("PRAGMA table_info(series)");
    if (!execQuery(query, __FILE__, __LINE__))
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

void medLocalDbController::addTextColumnToSeriesTableIfNeeded(QSqlQuery query, QString columnName)
{
    bool isColumnThere = false;
    query.first();

    while (query.next())
    {
        if (query.value(1).toString() == columnName)
        {
            isColumnThere = true;
        }
    }

    // If columnName is not defined in the db series table, add it.
    if (!isColumnThere)
    {
        query.prepare("ALTER TABLE series ADD COLUMN " + columnName + " TEXT");
        execQuery(query, __FILE__, __LINE__);
    }
}

bool medLocalDbController::updateFromNoVersionToVersion1()
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

    if (!(q.exec("PRAGMA user_version") && q.first()))
    {
        qWarning("medLocalDbController: Testing DB version for upgrade failed.");
        qDebug() << q.lastError();
        return false;
    }

    if (!q.exec("BEGIN EXCLUSIVE TRANSACTION"))
    {
        qWarning("medLocalDbController: Could not begin transaction.");
        qDebug() << q.lastError();
        return false;
    }

    // Test for isIndexed column to series
    if (!(q.exec("SELECT sql FROM sqlite_master WHERE type=='table' AND tbl_name=='series'") && q.first()))
    {
        qWarning("medLocalDbController: Could not test for `isIndexed` column in table `series`.");
        qDebug() << q.lastError();
        return false;
    }

    bool missingColumn = (q.value(0).toString().indexOf("isIndexed") == -1);

    if (missingColumn)
    {
        // Add isIndexed column if missing
        if (!q.exec("ALTER TABLE series ADD COLUMN isIndexed BOOLEAN"))
        {
            qWarning("medLocalDbController: Could not add `isIndexed` column to table `series`.");
            qDebug() << q.lastError();
            return false;
        }
    }

    // If the image column does not exist, the database is already a new version,
    // and user_version should be set without trying to access the image column from old versions.
    if (q.value(0).toString().indexOf("image") != -1)
    {
        // Migrate path and isIndexed fields from table `image` to `series`
        if (!q.exec("SELECT id,path FROM image WHERE isIndexed == 'true'"))
        {
            qWarning("medLocalDbController: getting a list of path from the `image` table failed.");
            qDebug() << q.lastError();
            return false;
        }
        QHash<int, QStringList> imagePaths;
        while (q.next())
        {
            imagePaths[q.value(0).toInt()] += q.value(0).toString();
        }
        for (int id : imagePaths.keys())
        {
            q.prepare("UPDATE series SET path==:paths,isIndexed='true' WHERE id==:seriesId");
            q.bindValue(":paths", imagePaths[id].join(";"));
            q.bindValue(":seriesId", id);
            if (!q.exec())
            {
                qWarning("medLocalDbController: updating the `series` table failed for id %d.", id);
                qDebug() << q.lastError();
                return false;
            }
        }

        // Delete table `image`
        if (!q.exec("DROP TABLE image"))
        {
            qWarning("medLocalDbController: could not drop table `image`.");
            qDebug() << q.lastError();
            return false;
        }
    }

    // finally, update DB version
    if (!q.exec("PRAGMA user_version = 1"))
    {
        qWarning("medLocalDbController: updating DB version to 1 after upgrade failed.");
        qDebug() << q.lastError();
        return false;
    }

    if (!q.exec("END TRANSACTION"))
    {
        qWarning("medLocalDbController: Could not end transaction.");
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

/** Enumerate all patients stored in this DB*/
QList<medDataIndex> medLocalDbController::patients() const
{
    QList<medDataIndex> ret;
    QSqlQuery query(m_database);
    query.prepare("SELECT id FROM patient");
    execQuery(query, __FILE__, __LINE__);
#if QT_VERSION > 0x0406FF
    ret.reserve(query.size());
#endif
    while (query.next())
    {
        ret.push_back(medDataIndex::makePatientIndex(this->dataSourceId(), query.value(0).toInt()));
    }
    return ret;
}

void medLocalDbController::requestDatabaseForModel(QHash<int, QHash<QString, QVariant>> &patientData,
                                                   QHash<int, QHash<QString, QVariant>> &studyData,
                                                   QHash<int, QHash<QString, QVariant>> &seriesData) const
{
    QString queryStr = "select patient.id as patient_id, patient.name as patient_name, patient.birthdate, patient.gender, \
                    study.id as study_id, study.name as study_name, \
                    series.id as series_id, series.name as series_name, series.size, series.age, series.modality , series.acquisitiondate, \
                    series.importationdate, series.referee, series.performer, series.institution, \
                    series.report, series.thumbnail \
                    from patient \
                    inner join study on patient.id = study.patient \
                    inner join series on study.id = series.study";

    QSqlQuery query(m_database);
    query.prepare(queryStr);

    execQuery(query, __FILE__, __LINE__);
    while (query.next())
    {
        QHash<QString, QVariant> &patientEntry = patientData[query.value("patient_id").toInt()];
        if (patientEntry.isEmpty())
        {
            patientEntry[medMetaDataKeys::PatientName.key()] = query.value("patient_name").toString();
            patientEntry[medMetaDataKeys::BirthDate.key()] = query.value("birthdate").toString();
            patientEntry[medMetaDataKeys::Gender.key()] = query.value("gender").toString();
        }

        QHash<QString, QVariant> &studyEntry = studyData[query.value("study_id").toInt()];

        if (studyEntry.isEmpty())
        {
            QList<QVariant> studies = patientEntry["studies"].toList();
            studies.append(query.value("study_id").toInt());
            patientEntry["studies"] = studies;
            studyEntry[medMetaDataKeys::StudyDescription.key()] = query.value("study_name").toString();
        }
        QHash<QString, QVariant> &seriesEntry = seriesData[query.value("series_id").toInt()];
        if (seriesEntry.isEmpty())
        {
            QList<QVariant> series = studyEntry["series"].toList();
            series.append(query.value("series_id").toInt());
            studyEntry["series"] = series;
            seriesEntry[medMetaDataKeys::SeriesDescription.key()] = query.value("series_name").toString();
            seriesEntry[medMetaDataKeys::Size.key()] = query.value("size").toString();
            seriesEntry[medMetaDataKeys::Age.key()] = query.value("age").toString();
            seriesEntry[medMetaDataKeys::Modality.key()] = query.value("modality").toString();
            seriesEntry[medMetaDataKeys::AcquisitionDate.key()] = query.value("acquisitiondate").toString();
            seriesEntry[medMetaDataKeys::ImportationDate.key()] = query.value("importationdate").toString();
            seriesEntry[medMetaDataKeys::Referee.key()] = query.value("referee").toString();
            seriesEntry[medMetaDataKeys::Performer.key()] = query.value("performer").toString();
            seriesEntry[medMetaDataKeys::Institution.key()] = query.value("institution").toString();
            seriesEntry[medMetaDataKeys::Report.key()] = query.value("report").toString();
            seriesEntry[medMetaDataKeys::ThumbnailPath.key()] = query.value("thumbnail").toString();
        }
    }
    return;
}

