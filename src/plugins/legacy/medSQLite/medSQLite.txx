/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medSQLite.h"

#include <QSqlDriver>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>

template <typename T>
medSQlite<T>::medSQlite()
        : medAbstractSource(), m_Driver("QSQLITE"),
        m_ConnectionName("sqlite"), m_instanceId(QString()),
        m_online(false), m_LevelNames({"patient","study","series"})
{
#ifdef Q_OS_MAC
    auto vDbLoc = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/" + QCoreApplication::organizationName() + "/" + QCoreApplication::applicationName();
#else
    auto vDbLoc = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/data/" + QCoreApplication::organizationName() + "/" + QCoreApplication::applicationName();
#endif

    m_DbPath = new medStringParameter("LocalDataBasePath", this);
    m_DbPath->setCaption("Path to sqlite local DB");
    m_DbPath->setValue(vDbLoc);
//    m_DbPath =
    QObject::connect(m_DbPath, &medStringParameter::valueChanged, this, &medSQlite::updateDatabaseName);
//    QObject::connect(this, SIGNAL(progress(int, eRequestStatus)), this, SLOT(foo(int, eRequestStatus)));
//    emit progress(0, eRequestStatus::aborted);
}

template<typename T>
bool medSQlite<T>::initialization(const QString &pi_instanceId)
{
    bool bRes = !pi_instanceId.isEmpty();
    if (bRes)
    {
        bRes = T::isDriverAvailable(m_Driver);
        if(bRes)
        {
            m_Engine = T::addDatabase(m_Driver, "sqlite");
            bRes = m_Engine.isValid();
            if (bRes)
            {
                m_instanceId = pi_instanceId;
            }
        }
    }
    return bRes;
}

template<typename T>
bool medSQlite<T>::setInstanceName(const QString &pi_instanceName)
{
    bool bRes = false;

    if (!pi_instanceName.isEmpty())
    {
        m_instanceName = pi_instanceName;
    }

    return bRes;
}

template<typename T>
bool medSQlite<T>::connect(bool pi_bEnable)
{
    bool bRes = false;
    if (pi_bEnable) // establish connection
    {
        if (!m_DbPath->value().isEmpty())
        {
            m_Engine = T::database(m_ConnectionName);
            if (!m_Engine.isValid())
            {
                m_Engine = T::addDatabase(m_Driver, m_ConnectionName);
            }
            m_Engine.setDatabaseName(m_DbPath->value() + "/db");
            bRes = m_Engine.open();
            if (bRes)
            {
                if (isDatabaseEmpty())
                {
                    QString patientQuery = "CREATE TABLE IF NOT EXISTS patient ("
                                           " id       INTEGER PRIMARY KEY, name        TEXT,"
                                           " thumbnail   TEXT, birthdate   TEXT,"
                                           " gender      TEXT, patientId   TEXT"
                                           ");";

                    QString studyQuery = "CREATE TABLE IF NOT EXISTS study ("
                                         " id        INTEGER      PRIMARY KEY, patient   INTEGER," // FOREIGN KEY
                                         " name         TEXT, uid          TEXT,"
                                         " thumbnail    TEXT, studyId      TEXT"
                                         ");";

                    QString seriesQuery = "CREATE TABLE IF NOT EXISTS series ("
                                          " id       INTEGER      PRIMARY KEY, study    INTEGER," // FOREIGN KEY
                                          " size     INTEGER, name            TEXT, path            TEXT,"
                                          " uid             TEXT, seriesId        TEXT, orientation     TEXT,"
                                          " seriesNumber    TEXT, sequenceName    TEXT, sliceThickness  TEXT,"
                                          " rows            TEXT, columns         TEXT, thumbnail       TEXT,"
                                          " age             TEXT, description     TEXT, modality        TEXT,"
                                          " protocol        TEXT, comments        TEXT, status          TEXT,"
                                          " acquisitiondate TEXT, importationdate TEXT, referee         TEXT,"
                                          " performer       TEXT, institution     TEXT, report          TEXT,"
                                          " origin          TEXT, flipAngle       TEXT, echoTime        TEXT,"
                                          " repetitionTime  TEXT, acquisitionTime TEXT"
                                          ");";

                    bRes = createTable(patientQuery)
                            && createTable(studyQuery)
                            && createTable(seriesQuery);
                    if (!bRes)
                    {
                        m_Engine.close();
                        T::removeDatabase(m_ConnectionName);
                    }
                }
                else if (!isValidDatabaseStructure())
                {
                    bRes = false;
                    m_Engine.close();
                    T::removeDatabase(m_ConnectionName);
                }
                if (bRes)
                {
                    optimizeSpeedSQLiteDB();
                }
            }
            else
            {
                T::removeDatabase(m_ConnectionName);
            }
        }
        else
        {
            bRes = false;
        }
        m_online = bRes;
    }
    else //disconnect
    {
        m_Engine.close();
        m_Engine = T();
        T::removeDatabase(m_ConnectionName);
        m_online = false;
        bRes = true;
    }
    return bRes;
}

template<typename T>
QList<medAbstractParameter *> medSQlite<T>::getAllParameters()
{
    QList<medAbstractParameter *> paramListRes;

    paramListRes.push_back(m_DbPath);

    return paramListRes;
}

template<typename T>
QList<medAbstractParameter *> medSQlite<T>::getCipherParameters()
{
    return QList<medAbstractParameter *>();
}

template<typename T>
QList<medAbstractParameter *> medSQlite<T>::getVolatilParameters()
{
    return QList<medAbstractParameter *>();
}

template <typename T>
bool medSQlite<T>::isWriteable()
{
    return true;
}

template <typename T>
bool medSQlite<T>::isLocal()
{
    return true;
}

template <typename T>
bool medSQlite<T>::isCached()
{
    return false;
}

template <typename T>
bool medSQlite<T>::isOnline()
{
    return m_online;
}

template<typename T>
QString medSQlite<T>::getInstanceName()
{
    return m_instanceName;
}

template<typename T>
QString medSQlite<T>::getInstanceId()
{
    return m_instanceId;
}

template <typename T>
unsigned int medSQlite<T>::getLevelCount()
{
    return m_LevelNames.size();
}

template <typename T>
QStringList medSQlite<T>::getLevelNames()
{
    return m_LevelNames;
}

template <typename T>
QString medSQlite<T>::getLevelName(unsigned int pi_uiLevel)
{
    QString retVal;
    if (pi_uiLevel>0 || pi_uiLevel<static_cast<unsigned int>(m_LevelNames.size()))
    {
        retVal = m_LevelNames.value((int)pi_uiLevel);
    }
    return retVal;
}

template <typename T>
QStringList medSQlite<T>::getMandatoryAttributesKeys(unsigned int pi_uiLevel)
{
    switch (pi_uiLevel)
    {
        case 0:
            return {"id", "name", "patientId"};
        case 1:
            return {"id", "name", "uid"};
        case 2:
            return {"id", "name", "uid"};
        default:
            return QStringList();
    }
}

template <typename T>
QStringList medSQlite<T>::getAdditionalAttributesKeys(unsigned int pi_uiLevel)
{
    switch (pi_uiLevel)
    {
        case 0:
            return {"thumbnail", "birthdate", "gender"};
        case 1:
            return {"patient"};
            // There is 2 other keys {"thumbnail", "studyId"} in table study
            // but not sure if we want to keep them in future version
        case 2:
            // this is the list of all available additional attributes keys in table series
            // {"study", "size", "path", "orientation", "seriesNumber", "sequenceName", "sliceThickness",
            // "rows", "columns", "thumbnail", "age", "description", "modality", "protocol", "comments",
            // "status", "acquisitiondate", "importationdate", "referee", "performer", "institution",
            // "report", "origin", "flipAngle", "echoTime", "repetitionTime", "acquisitionTime", "isIndexed"};
            // For now, we choose to send only the key from legacy medinria data source
            return {"size", "age", "modality", "acquisitiondate", "importationdate", "referee",
                    "performer", "institution", "report", "thumbnail"};
        default:
            return QStringList();
    }

}

template <typename T>
QList<medAbstractSource::levelMinimalEntries> medSQlite<T>::getMinimalEntries(unsigned int pi_uiLevel, QString parentId)
{

    // TODO : id doit etre passé au level 1 & 2. Il correspond à l'id du level supérieur (get study from a patient id)
    // renommer id en parentId ?
    QList<levelMinimalEntries> entries;
    QString key;
    if (parentId.contains("/"))
    {
        QStringList splittedUri = parentId.split("/");
        key = splittedUri[pi_uiLevel-1];
    }
    else
    {
        key = parentId;
    }
    
    switch (pi_uiLevel)
    {
        case 0:
            entries = getPatientMinimalEntries();
            break;
        case 1:
            entries = getStudyMinimalEntries(key);
            break;
        case 2:
            entries = getSeriesMinimalEntries(key);
            break;
        default:
            break;
    }
    return entries;
}

template <typename T>
QList<QMap<QString, QString>> medSQlite<T>::getMandatoryAttributes(unsigned int pi_uiLevel, QString parentId)
{
    QList < QMap<QString, QString>> res;
    if (pi_uiLevel == 0)
    {
        QMap<QString, QString> tmp;
        tmp["toto"] = "tata";
        res.push_back(tmp);
        tmp["toto"] = "titi";
        res.push_back(tmp);
    }
    return res;
}

template <typename T>
QList<QMap<QString, QString>> medSQlite<T>::getAdditionalAttributes(unsigned int pi_uiLevel, QString parentId)
{
    return QList<QMap<QString, QString>>();
}

template <typename T>
QString medSQlite<T>::getDirectData(unsigned int pi_uiLevel, QString key)
{
    QString path;
    if (pi_uiLevel==2)
    {
        path = m_DbPath->value() + getSeriesDirectData(key);
    }
    return path;
}

template <typename T>
int medSQlite<T>::getAssyncData(unsigned int pi_uiLevel, QString id)
{
    return 0;
}

template<typename T>
bool medSQlite<T>::addData(void * data, QString uri)
{
    bool bRes = true;

    QSqlQuery query = m_Engine.exec();
    auto splittedUri = uri.split('/');
    auto queryString = QString("INSERT INTO series(study,name) VALUES(:study,:name)");
    bRes = query.prepare(queryString);
    query.bindValue(":study", splittedUri[1]);
    query.bindValue(":name",  splittedUri[2]);

    if (bRes)
    {
        if (!query.exec())
        {
            qDebug() << query.lastError();
            bRes = false;
        }
    }

    return bRes;
}

template <typename T>
void medSQlite<T>::abort(int pi_iRequest)
{

}

template <typename T>
void medSQlite<T>::updateDatabaseName(QString const &path)
{

}

template<typename T>
bool medSQlite<T>::isValidDatabaseStructure()
{
    bool bRes = false;
    QStringList tables = m_Engine.tables();
    bRes = tables.contains(m_LevelNames.value(0))
            && tables.contains(m_LevelNames.value(1))
            && tables.contains(m_LevelNames.value(2));
    return bRes;
}

template<typename T>
bool medSQlite<T>::isDatabaseEmpty()
{
    bool bRes = false;
    QStringList tables = m_Engine.tables();
    if (tables.empty())
    {
        bRes = true;
    }
    return bRes;
}

template<typename T>
bool medSQlite<T>::createTable(const QString &strQuery)
{
    bool bRes = true;
    QSqlQuery query = m_Engine.exec();
    if (query.prepare(strQuery))
    {
        if (!query.exec())
        {
            qDebug() << "The query was: " << query.lastQuery().simplified();
            bRes = false;
        }
    }
    else
    {
        qDebug() << query.lastError();
        bRes = false;
    }
    return bRes;
}

template<typename T>
QList<medAbstractSource::levelMinimalEntries> medSQlite<T>::getPatientMinimalEntries()
{
    QList<levelMinimalEntries> patientEntries;
    QSqlQuery query = m_Engine.exec();
    if (!(query.prepare("SELECT id as db_id, name as patient_name, patientID as patient_id from patient")
    && query.exec()))
    {
        qDebug() << "Error returned by query : "<< query.lastError();
        qDebug() << "The query was: " << query.lastQuery().simplified();
    }
    else
    {
        while (query.next())
        {
            levelMinimalEntries entry;
            entry.key = query.value("db_id").toString();
            entry.name = query.value("patient_name").toString();
            entry.description = query.value("patient_id").toString();
            patientEntries.append(entry);
        }
    }
    return patientEntries;
}
template<typename T>
QList<medAbstractSource::levelMinimalEntries> medSQlite<T>::getStudyMinimalEntries(QString &parentId)
{
    QList<levelMinimalEntries> studyEntries;

    bool isValid;
    int patientId = parentId.toInt(&isValid);
    if (isValid)
    {
        QSqlQuery query = m_Engine.exec();
        query.prepare("SELECT id as db_id, name as study_name, uid as study_instance_uid "
                      "FROM study "
                      "WHERE patient = :patientId");
        query.bindValue(":patientId", patientId);

        if (!query.exec())
        {
            qDebug() << "Error returned by query : " << query.lastError();
            qDebug() << "The query was: " << query.lastQuery().simplified();
            // TODO : call to abort ?
        }
        else
        {
            while (query.next())
            {
                levelMinimalEntries entry;
                entry.key = query.value("db_id").toString();
                entry.name = query.value("study_name").toString();
                entry.description = query.value("study_instance_uid").toString();
                studyEntries.append(entry);
            }
            // TODO : call to success ?
        }
    }
    else
    {
        qDebug() << "The key is not a valid integer ";
        // TODO : call to abort ?
    }
    return studyEntries;
}

template<typename T>
QList<medAbstractSource::levelMinimalEntries> medSQlite<T>::getSeriesMinimalEntries(QString &parentId)
{
    QList<levelMinimalEntries> seriesEntries;

    bool isValid;
    int studyId = parentId.toInt(&isValid);
    if (isValid)
    {
        QSqlQuery query = m_Engine.exec();
        query.prepare("SELECT id as db_id, name as series_name, uid as series_instance_uid "
                      "FROM series "
                      "WHERE study = :studyId");
        query.bindValue(":studyId", studyId);

        if (!query.exec())
        {
            qDebug() << "Error returned by query : " << query.lastError();
            qDebug() << "The query was: " << query.lastQuery().simplified();
            // TODO : call to abort ?
        }
        else
        {
            while (query.next())
            {
                levelMinimalEntries entry;
                entry.key = query.value("db_id").toString();
                entry.name = query.value("series_name").toString();
                entry.description = query.value("series_instance_uid").toString();
                seriesEntries.append(entry);
            }
            // TODO : call to success ?
        }
    }
    else
    {
        qDebug() << "The key is not a valid integer ";
        // TODO : call to abort ?
    }
    return seriesEntries;
}

template<typename T>
void medSQlite<T>::optimizeSpeedSQLiteDB()
{
    // From legacy medDatabaseController
    // optimize speed of sqlite db
    QSqlQuery query = m_Engine.exec();
    if (!(query.prepare(QLatin1String("PRAGMA synchronous = 0")) && query.exec()))
    {
        qDebug() << "Could not set sqlite synchronous mode to asynchronous mode.";
    }
    if (!(query.prepare(QLatin1String("PRAGMA journal_mode=wal")) && query.exec()))
    {
        qDebug() << "Could not set sqlite write-ahead-log journal mode";
    }
}

template<typename T>
QString medSQlite<T>::getSeriesDirectData(QString &key)
{
    QString path;
    bool isValid;
    int db_id = key.toInt(&isValid);
    if (isValid)
    {
        QSqlQuery query = m_Engine.exec();
        query.prepare("SELECT path as data_path "
                      "FROM series "
                      "WHERE id = :id");
        query.bindValue(":id", db_id);

        if (!query.exec())
        {
            qDebug() << "Error returned by query : " << query.lastError();
            qDebug() << "The query was: " << query.lastQuery().simplified();
            // TODO : call to abort ?
        }
        else
        {
            while (query.next())
            {
                path = query.value("data_path").toString();
            }
            emit progress(10, medAbstractSource::eRequestStatus::finish);
            // TODO : call to success ?
        }
    }
    else
    {
        qDebug() << "The key is not a valid integer ";
        // TODO : call to abort ?
    }
    return path;
}

