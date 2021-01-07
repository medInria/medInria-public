/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medRemoteDbController.h"

#include <medJobManagerL.h>
#include <medMessageController.h>

#include "medDatabaseImporter.h"
#include "medDatabaseReader.h"
#include "medDatabaseRemover.h"
#include "medSettingsManager.h"
#include "medStorage.h"

medRemoteDbController *medRemoteDbController::s_instance = NULL;

medRemoteDbController *medRemoteDbController::instance()
{
    if (!s_instance)
    {
        s_instance = new medRemoteDbController();
    }
    return s_instance;
}

medRemoteDbController::medRemoteDbController() : medSqlDbController()
{
}

bool medRemoteDbController::createConnection(void)
{
    m_database = QSqlDatabase::database("psqldb");
    if (!m_database.isValid())
    {
        m_database = QSqlDatabase::addDatabase("QPSQL", "psqldb");
    }
    int port = medSettingsManager::instance()
                   ->value("database", "port", 5432, false)
                   .toInt();
    QString hostname = medSettingsManager::instance()
                           ->value("database", "hostname", "localhost", false)
                           .toString();
    qDebug() << "hostname " << hostname;
    qDebug() << "port " << port;
    m_database.setHostName(hostname);
    m_database.setPort(port);
    m_database.setDatabaseName("musicdb");
    m_database.setUserName("music");
    m_database.setPassword("music");

    if (!m_database.open())
    {
        qDebug()
            << DTK_COLOR_FG_RED
            << "Cannot open database: unable to establish a database connection."
            << DTK_NO_COLOR;
        return false;
    }
    else
    {
        qDebug() << "Database opened at: " << qPrintable(m_database.databaseName());
        setConnected(true);
    }
    return true;
}

bool medRemoteDbController::closeConnection(void)
{
    m_database.close();
    QSqlDatabase::removeDatabase("QPSQL");
    setConnected(false);
    return true;
}

/** Enumerate all patients stored in this DB*/
QList<medDataIndex> medRemoteDbController::patients() const
{
    QList<medDataIndex> ret;

    int center = medSettingsManager::instance()
                     ->value("database", "center", -1, false)
                     .toInt();
    QSqlQuery query(m_database);

    if (center == -1)
    {
        query.prepare("SELECT id FROM patient");
    }
    else
    {
        query.prepare("SELECT id FROM patient WHERE center = :centerId");
        query.bindValue(":centerId", center);
        /* code */
    }

    execQuery(query, __FILE__, __LINE__);
#if QT_VERSION > 0x0406FF
    ret.reserve(query.size());
#endif
    while (query.next())
    {
        ret.push_back(medDataIndex::makePatientIndex(this->dataSourceId(),
                                                     query.value(0).toInt()));
    }
    return ret;
}

QList<QList<QVariant>> medSqlDbController::requestDatabaseForModel() const
{
    QList<QList<QVariant>> ret;

    int center = medSettingsManager::instance()
                     ->value("database", "center", -1, false)
                     .toInt();
    QString queryStr = "select patient.id, patient.name, patient.birthdate, patient.gender, \
                    study.id, study.name, \
                    series.id, series.name, series.size, series.age, series.modality , series.acquisitiondate, \
                    series.importationdate, series.referee, series.performer, series.institution, \
                    series.report, series.thumbnail \
                    from patient \
                    inner join study on patient.id = study.patient \
                    inner join series on study.id = series.study";

    QSqlQuery query(m_database);
    if (center == -1)
    {
        query.prepare(queryStr);
    }
    else
    {
        queryStr += " WHERE center = :centerId";
        query.prepare(queryStr);
        query.bindValue(":centerId", center);
    }

    execQuery(query, __FILE__, __LINE__);
#if QT_VERSION > 0x0406FF
    ret.reserve(query.size());
#endif
    while (query.next())
    {
        QList<QVariant> columns;
        for (int i = 0; i < 18; i++)
        {
            columns.append(query.value(i));
        }
        ret.append(columns);
    }
    return ret;
}
