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
        : medAbstractSource(), m_Driver("QSQLITE"), m_instanceId(QString()), m_online(false), m_LevelNames({"patient","study","series"})
{
    m_DbPath = new medStringParameter("LocalDataBasePath", this);
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
            m_Engine = T::database("sqlite");
            if (!m_Engine.isValid())
            {
                m_Engine = T::addDatabase("QSQLITE", "sqlite");
            }
            m_Engine.setDatabaseName(m_DbPath->value() + "/db");
            bRes = m_Engine.open();
            if (bRes)
            {
                if (isDatabaseEmpty())
                {
                    // TODO : Find a way to test QSQLQuery
                    QString patientQuery = "CREATE TABLE IF NOT EXISTS patient ("
                                           " id       INTEGER PRIMARY KEY,"
                                           " name        TEXT,"
                                           " thumbnail   TEXT,"
                                           " birthdate   TEXT,"
                                           " gender      TEXT,"
                                           " patientId   TEXT"
                                           ");";
                    QString studyQuery = "CREATE TABLE IF NOT EXISTS study ("
                                         " id        INTEGER      PRIMARY KEY,"
                                         " patient   INTEGER," // FOREIGN KEY
                                         " name         TEXT,"
                                         " uid          TEXT,"
                                         " thumbnail    TEXT,"
                                         " studyId      TEXT"
                                         ");";
                    QString seriesQuery = "CREATE TABLE IF NOT EXISTS series ("
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
                                          ");";
                    bRes = createTable(patientQuery)
                            && createTable(studyQuery)
                            && createTable(seriesQuery);
                    if (!bRes)
                    {
                        m_Engine.close();
                    }
                }
                else if (!isValidDatabaseStructure())
                {
                    bRes = false;
                    m_Engine.close();
                }
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
        m_online = false;
        bRes = true;
    }
    return bRes;
}

template<typename T>
QList<medAbstractParameter *> medSQlite<T>::getAllParameters()
{
    return QList<medAbstractParameter *>();
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
    if (pi_uiLevel>0 || pi_uiLevel<m_LevelNames.size())
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
            return {"id", "name", "birthdate", "gender"};
        case 1:
            return {"id", "name"};
        case 2:
            return {"id", "name"};
        default:
            return QStringList();
    }

}

template <typename T>
QStringList medSQlite<T>::getAdditionalAttributesKeys(unsigned int pi_uiLevel)
{
    return QStringList();
}

template <typename T>
QList<medAbstractSource::levelMinimalEntries> medSQlite<T>::getMinimalEntries(unsigned int pi_uiLevel, QString id)
{
    return QList<levelMinimalEntries>();
}

template <typename T>
QList<QMap<QString, QString>> medSQlite<T>::getMandatoryAttributes(unsigned int pi_uiLevel, int id)
{
    return QList<QMap<QString, QString>>();
}

template <typename T>
QList<QMap<QString, QString>> medSQlite<T>::getAdditionalAttributes(unsigned int pi_uiLevel, int id)
{
    return QList<QMap<QString, QString>>();
}

template <typename T>
medAbstractData *medSQlite<T>::getDirectData(unsigned int pi_uiLevel, QString id)
{
    return nullptr;
}

template <typename T>
int medSQlite<T>::getAssyncData(unsigned int pi_uiLevel, QString id)
{
    return 0;
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
