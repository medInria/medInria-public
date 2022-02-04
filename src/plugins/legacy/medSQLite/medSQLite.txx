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
    QObject::connect(m_DbPath, &medStringParameter::valueChanged, this, &medSQlite::updateDatabaseName);

    m_MandatoryKeysByLevel["Patient"] = QStringList({"id", "name", "patientId",
                                                     "birthdate", "gender"});
    m_MandatoryKeysByLevel["Study"] = QStringList( {"id", "name", "uid", "patient"});
    m_MandatoryKeysByLevel["Series"] = QStringList( {"id", "name", "uid", "study",
                                                     "age", "modality", "protocol",
                                                     "origin", "rows", "columns"}) ;

}

template<typename T>
medSQlite<T>::~medSQlite()
{
    //TODO
    connect(false);
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
bool medSQlite<T>::isFetchByMinimalEntriesOrMandatoryAttributes()
{
    return false;
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
            return m_MandatoryKeysByLevel["Patient"];//{"id", "name", "patientId", "birthdate", "gender"};
        case 1:
            return m_MandatoryKeysByLevel["Study"];//return {"id", "patient", "name", "uid"};
        case 2:
            return m_MandatoryKeysByLevel["Series"];//return {"id", "study", "name", "uid", "age", "modality", "protocol", "origin", "rows", "cols"};
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

    switch (pi_uiLevel)
    {
        case 0:
            entries = getPatientMinimalEntries(parentId);
            break;
        case 1:
            entries = getStudyMinimalEntries(parentId);
            break;
        case 2:
            entries = getSeriesMinimalEntries(parentId);
            break;
        default:
            break;
    }
    return entries;
}

template<typename T>
QList<medAbstractSource::levelMinimalEntries> medSQlite<T>::getPatientMinimalEntries(QString &key)
{
    QList<levelMinimalEntries> patientEntries;
    QSqlQuery query = m_Engine.exec();

    bool isInt;
    int id = key.toInt(&isInt);
    if (key.isEmpty() || (!key.isEmpty() && isInt))
    {
        if (key.isEmpty())
        {
            query.prepare("SELECT id as db_id, name as patient_name, patientID as patient_id from patient");
        }
        else
        {
            query.prepare("SELECT id as db_id, name as patient_name, patientID as patient_id from patient where id = :id");
            query.bindValue(":id", id);
        }

        if (!query.exec())
        {
            qDebug() << "Error returned by query : " << query.lastError();
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

template <typename T>
QList<QMap<QString, QString>> medSQlite<T>::getMandatoryAttributes(unsigned int pi_uiLevel, QString parentId)
{
    QList < QMap<QString, QString>> res;

    switch (pi_uiLevel)
    {
        case 0:
            res = getPatientMandatoriesAttributes(parentId);
            break;
        case 1:
            res = getStudyMandatoriesAttributes(parentId);
            break;
        case 2:
            res = getSeriesMandatoriesAttributes(parentId);
            break;
        default:
            break;
    }
    return res;
}


template<typename T>
QList<QMap<QString, QString>> medSQlite<T>::getPatientMandatoriesAttributes(const QString &key)
{
    QList<QMap<QString, QString>> patientRes;
    bool isInt;
    int id = key.toInt(&isInt);
    if (key.isEmpty() || (!key.isEmpty() && isInt))
    {
        QSqlQuery query = m_Engine.exec();
        QStringList patientMandatoryKeys = m_MandatoryKeysByLevel["Patient"];//{"id", "name", "patientId", "birthdate", "gender"}
        QString selectQuery = "SELECT ";
        for (auto str : patientMandatoryKeys)
        {
            selectQuery += str + " as " + str + ", ";
        }
        selectQuery = selectQuery.left(selectQuery.size() - 2);
        if (key.isEmpty())
        {
            selectQuery += " from patient";
            query.prepare(selectQuery);
        }
        else
        {
            selectQuery += " from patient where id = :id";
            query.prepare(selectQuery);
            query.bindValue(":id", id);
        }

        if (!query.exec())
        {
            qDebug() << "Error returned by query : " << query.lastError();
            qDebug() << "The query was: " << query.lastQuery().simplified();
        }
        else
        {
            while (query.next())
            {
                QMap<QString, QString> patientMap;
                for (const QString &str : patientMandatoryKeys)
                {
                    patientMap[str] = query.value(str).toString();
                }
                patientRes.append(patientMap);
            }
        }
    }
    return patientRes;
}

template<typename T>
QList<QMap<QString, QString>> medSQlite<T>::getStudyMandatoriesAttributes(const QString &key)
{
    QList<QMap<QString, QString>> studyRes;
    bool isValid;
    int patient = key.toInt(&isValid);
    if (isValid)
    {
        QSqlQuery query = m_Engine.exec();
        QStringList studyMandatoryKeys = m_MandatoryKeysByLevel["Study"];
        QString selectQuery = "SELECT ";
        for (auto str : studyMandatoryKeys)
        {
            selectQuery += str + " as " + str + ", ";
        }
        selectQuery = selectQuery.left(selectQuery.size() - 2);
        selectQuery += " from study where patient = :patient";
        query.prepare(selectQuery);
        query.bindValue(":patient", patient);

        if (!query.exec())
        {
            qDebug() << "Error returned by query : " << query.lastError();
            qDebug() << "The query was: " << query.lastQuery().simplified();
        }
        else
        {
            while (query.next())
            {
                QMap<QString, QString> studyMap;
                for (const QString &str : studyMandatoryKeys)
                {
                    studyMap[str] = query.value(str).toString();
                }
                studyRes.append(studyMap);
            }
        }
    }
    return studyRes;
}

template<typename T>
QList<QMap<QString, QString>> medSQlite<T>::getSeriesMandatoriesAttributes(const QString &key)
{
    QList<QMap<QString, QString>> seriesRes;
    bool isValid;
    int study = key.toInt(&isValid);
    if (isValid)
    {
        QSqlQuery query = m_Engine.exec();
        QStringList seriesMandatoryKeys = m_MandatoryKeysByLevel["Series"];
        QString selectQuery = "SELECT ";
        for (auto str : seriesMandatoryKeys)
        {
            selectQuery += str + " as " + str + ", ";
        }
        selectQuery = selectQuery.left(selectQuery.size() - 2);
        selectQuery += " from series where study = :study";
        query.prepare(selectQuery);
        query.bindValue(":study", study);

        if (!query.exec())
        {
            qDebug() << "Error returned by query : " << query.lastError();
            qDebug() << "The query was: " << query.lastQuery().simplified();
        }
        else
        {
            while (query.next())
            {
                QMap<QString, QString> seriesMap;
                for (const QString &str : seriesMandatoryKeys)
                {
                    seriesMap[str] = query.value(str).toString();
                }
                seriesRes.append(seriesMap);
            }
        }
    }
    return seriesRes;
}

template <typename T>
bool medSQlite<T>::getAdditionalAttributes(unsigned int pi_uiLevel, QString id, datasetAttributes4 &po_attributes)
{
    bool bRes = false;
    // renommer id en parentId ?

    switch (pi_uiLevel)
    {
        case 0:
            bRes = true;
            break;
        case 1:
            bRes = true;
            break;
        case 2:
            bRes = getSeriesAdditionalAttributes(id, po_attributes);
            break;
        default:
            break;
    }
    return bRes;
}

template<typename T>
bool medSQlite<T>::getSeriesAdditionalAttributes(const QString &key, medAbstractSource::datasetAttributes4 &po_attributes)
{
    bool bRes;
    int id = key.toInt(&bRes);
    if (bRes)
    {

        QSqlQuery query = m_Engine.exec();
        QStringList columnNames;
        columnNames <<"size"<<"path"<<"orientation"<<"seriesNumber"<<"sequenceName"<<"sliceThickness"
                    <<"thumbnail"<<"description"<<"comments"<<"status"<<"acquisitiondate"<<"importationdate"
                    <<"referee"<<"performer"<<"institution"<<"report"<<"origin"<<"flipAngle"<<"echoTime"
                    <<"repetitionTime"<<"acquisitionTime"<<"isIndexed";

        QString selectQuery = "SELECT ";
        for (const auto& str : columnNames)
        {
            selectQuery += str + " as " + str + ", ";
        }
        selectQuery = selectQuery.left(selectQuery.size() - 2);
        selectQuery += " from series where id = :id";
        bRes = query.prepare(selectQuery);
        if (bRes)
        {
            query.bindValue(":id", id);
            bRes = query.exec();
            if (bRes)
            {
                while (query.next())
                {
                    QMap<QString, QString> seriesMap;
                    for (const QString &str : columnNames)
                    {
                        po_attributes.values[str] = query.value(str).toString();
                    }
                }
            }
        }
    }
    return bRes;
}

template <typename T>
QVariant medSQlite<T>::getDirectData(unsigned int pi_uiLevel, QString key)
{
    QVariant res;
    if (pi_uiLevel==2)
    {
        QString path;
        if (getSeriesDirectData(key, path))
        {
            res = QVariant(m_DbPath->value() + path);
        }
    }
    return res;
}

template<typename T>
bool medSQlite<T>::getSeriesDirectData(QString &key, QString &path)
{
    bool bRes;
    int db_id = key.toInt(&bRes);
    if (bRes)
    {
        QSqlQuery query = m_Engine.exec();
        query.prepare("SELECT path as data_path "
                      "FROM series "
                      "WHERE id = :id");
        query.bindValue(":id", db_id);

        bRes = query.exec();
        if (bRes)
        {
            while (query.next())
            {
                path = query.value("data_path").toString();
            }
//            emit progress(10, medAbstractSource::eRequestStatus::finish);
        }
    }
    else
    {
        qDebug() << "The key is not a valid integer ";
    }
    return bRes;
}

template <typename T>
int medSQlite<T>::getAssyncData(unsigned int pi_uiLevel, QString id)
{
    return 0;
}

template<typename T>
QString medSQlite<T>::addData(QVariant data, QStringList parentUri, QString name)
{
    QString keyRes;

//    int sourceDelimterIndex = parentUri.indexOf(QString(":"));
//    QStringList uriAsList = parentUri.right(parentUri.size() - sourceDelimterIndex - 1).split(QString("\r\n"));

    int level  = parentUri.size() - 1;

    switch (level)
    {
        case 0: //Patient
        {
            //if (!additionalAttributes.values.isEmpty() || !additionalAttributes.tags.isEmpty())
            //{
            //    qWarning()<<"No expected additional values or tags at level patient in this local SQLite datasource plugin";
            //}
            //keyRes = addDataToPatientLevel(mandatoryAttributes);

            break;
        }
        case 1: //Study
        {
            //if (!additionalAttributes.values.isEmpty() || !additionalAttributes.tags.isEmpty())
            //{
            //    qWarning()<<"No expected additional values or tags at level study in this local SQLite datasource plugin";
            //}
            //keyRes = addDataToStudyLevel(mandatoryAttributes);
            break;
        }
        case 2://Series
        {
            keyRes = addDataToSeriesLevel(data, name, parentUri.last());
            break;
        }
        default: //Unknown level
        {
            break;
        }
    }

    return keyRes;
}

template<typename T>
QString medSQlite<T>::addDataToPatientLevel(QMap<QString, QString> &mandatoryAttributes)
{
    QString keyRes;
    QSqlQuery query = m_Engine.exec();

    //m_MandatoryKeysByLevel["Patient"];//{"id", "name", "patientId", "birthdate", "gender"}
    // 1st : Remove unecessary key
    auto id = mandatoryAttributes.take("id");
    QStringList keys = m_MandatoryKeysByLevel["Patient"]; //{ "name", "patientId", "birthdate", "gender"}
    keys.takeFirst();

    bool bVal = true;
    if (mandatoryAttributes.size() == keys.size())
    {
        for (const auto &key: keys)
        {
            if (!mandatoryAttributes.contains(key))
            {
                bVal = false;
                qWarning() << "The key " << key << " is missing in mandatory attributes at level patient";
            }
        }
    }
    else
    {
        qWarning()<<"mandatory attributes size mismatch";
        bVal = false;
    }

    if (bVal)
    {
//                query.prepare ( "INSERT INTO patient (name, patientId, birthdate, gender) "
//                                "VALUES (:name, :patientId, :birthdate, :gender)" );
        QString insertQuery = "INSERT INTO patient (";
        for (const auto& key : keys)
        {
            insertQuery += key + ", ";
        }
        insertQuery = insertQuery.left(insertQuery.size() - 2);
        insertQuery += ") VALUES (:";
        for (const auto& key : keys)
        {
            insertQuery += key + ", :";
        }
        insertQuery = insertQuery.left(insertQuery.size() - 3);
        insertQuery += ")";
        query.prepare(insertQuery);
        for (const auto& key : keys)
        {
            query.bindValue(":"+key, mandatoryAttributes[key]);
        }
        if (query.exec())
        {
            keyRes = query.lastInsertId().toString();
        }
    }
    return keyRes;
}

template<typename T>
QString medSQlite<T>::addDataToStudyLevel(QMap<QString, QString> mandatoryAttributes)
{
    QString keyRes;
    QSqlQuery query = m_Engine.exec();

    //m_MandatoryKeysByLevel["Study"];//return {"id", "patient", "name", "uid"};
    // 1st : Remove unecessary key
    auto id = mandatoryAttributes.take("id");
    QStringList keys = m_MandatoryKeysByLevel["Study"]; //{ "patient", "name", "uid"}
    keys.takeFirst();

    bool bVal = true;
    if (mandatoryAttributes.size() == keys.size())
    {
        for (const auto &key: keys)
        {
            if (!mandatoryAttributes.contains(key))
            {
                qWarning() << "The key " << key << " is missing in mandatory attributes at level study";
                bVal = false;
            }
        }
    }
    else
    {
        qWarning()<<"mandatory attributes size mismatch";
        bVal = false;
    }

    if (bVal)
    {
//                query.prepare ( "INSERT INTO study (patient, name, uid) "
//                                "VALUES (:patient, :name, :uid)" );
        QString insertQuery = "INSERT INTO study (";
        for (const auto& key : keys)
        {
            insertQuery += key + ", ";
        }
        insertQuery = insertQuery.left(insertQuery.size() - 2);
        insertQuery += ") VALUES (:";
        for (const auto& key : keys)
        {
            insertQuery += key + ", :";
        }
        insertQuery = insertQuery.left(insertQuery.size() - 3);
        insertQuery += ")";
        query.prepare(insertQuery);
        for (const auto& key : keys)
        {
            query.bindValue(":"+key, mandatoryAttributes[key]);
        }
        if (query.exec())
        {
            keyRes = query.lastInsertId().toString();
        }
    }
    return keyRes;
}

template<typename T>
QString medSQlite<T>::addDataToSeriesLevel(const QVariant& dataset,const QString & name, const QString & studyId)
{
    QString keyRes;
    QSqlQuery query = m_Engine.exec();

    // TODO extract path from dataset parameter and update additionalAttributes.values ==> addtionalAttributes.values["path"] = dataset.toString()
    bool bVal = true;
    if (dataset.canConvert<QString>())
    {
        QString pathIn = dataset.toString();
        QString fileName = pathIn.right(pathIn.size() - pathIn.lastIndexOf(QDir::separator()) - 1);
        QString pathOut = QUuid::createUuid().toString().replace("{", "").replace("}", "");// + "/" + fileName;

        QDir dir(m_DbPath->value());
        dir.mkdir(pathOut);
        pathOut.prepend("/");
        pathOut.append("/"+fileName);
        QString pathToCopy = m_DbPath->value() + pathOut;

        if (QFile::copy(pathIn, pathToCopy))
        {
            query.prepare("INSERT INTO series (study, name, uid, path) VALUES (:study, :name, :uid, :path)");
            query.bindValue(":study", studyId);
            query.bindValue(":name", name);
            query.bindValue(":uid", QUuid::createUuid().toString().replace("{", "").replace("}", ""));
            query.bindValue(":path", pathOut);

            if (query.exec())
            {
                keyRes = query.lastInsertId().toString();
            }
            else
            {
                qDebug()<<query.lastError();
            }
        }
    }

    return keyRes;
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


