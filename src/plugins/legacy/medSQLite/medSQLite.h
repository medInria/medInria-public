/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include "medSQLitePluginExport.h"

#include <medAbstractSource.h>
#include <medStringParameter.h>

#include <QSqlDatabase>


template <typename T>
class MEDSQLITEPLUGIN_EXPORT medSQlite: public medAbstractSource
{

public:
    explicit medSQlite();
    ~medSQlite() override = default;

    /* ***********************************************************************/
    /* *************** Init/set/ctrl source properties ***********************/
    /* ***********************************************************************/
    bool initialization(const QString &pi_instanceId) override;

    bool setInstanceName(const QString &pi_instanceName) override;

    bool connect(bool pi_bEnable) override;

    QList<medAbstractParameter *> getAllParameters() override;

    QList<medAbstractParameter *> getCipherParameters() override;

    QList<medAbstractParameter *> getVolatilParameters() override;

    /* ***********************************************************************/
    /* *************** Get source properties *********************************/
    /* ***********************************************************************/
    bool isWriteable() override;

    bool isLocal() override;

    bool isCached() override;

    bool isOnline() override;

    /* ***********************************************************************/
    /* *************** Get source structure information **********************/
    /* ***********************************************************************/
    QString getInstanceName() override;

    QString getInstanceId() override;

    unsigned int getLevelCount() override;

    QStringList getLevelNames() override;

    QString getLevelName(unsigned int pi_uiLevel) override;

    QStringList getMandatoryAttributesKeys(unsigned int pi_uiLevel) override;

    QStringList getAdditionalAttributesKeys(unsigned int pi_uiLevel) override;


    /* ***********************************************************************/
    /* *************** Get elements data *************************************/
    /* ***********************************************************************/
    QList<levelMinimalEntries> getMinimalEntries(unsigned int pi_uiLevel, QString id) override;

    QList<QMap<QString, QString>> getMandatoryAttributes(unsigned int pi_uiLevel, int id) override;

    QList<QMap<QString, QString>> getAdditionalAttributes(unsigned int pi_uiLevel, int id) override;

    /* ***********************************************************************/
    /* *************** Get data          *************************************/
    /* ***********************************************************************/
    medAbstractData *getDirectData(unsigned int pi_uiLevel, QString id) override;

    int getAssyncData(unsigned int pi_uiLevel, QString id) override;


public slots:
    void abort(int pi_iRequest) override;

    void updateDatabaseName(QString const &path);

protected:
    medStringParameter *m_DbPath;

private:
    T m_Engine;
    const QString m_Driver;
    const QString m_ConnectionName;
    QString m_instanceId;
    QString m_instanceName;
};

#include "medSQLite.txx"

/*
template <typename T>
medSQlite<T>::medSQlite()
        : medAbstractSource(), driver("QSQLITE"), connectionName("sqlite")
{
    database_path = new medStringParameter("LocalDataBasePath", this);
    connect(database_path, SIGNAL(valueChanged(QString const &)), this, SLOT(setDatabaseName(QString const &)));

}

template <typename T>
void medSQlite<T>::initialize()
{
    if(T::isDriverAvailable(driver))
    {
        db = T::database("sqlite");
        if (!db.isValid())
        {
            db = T::addDatabase(driver, "sqlite");
        }
        db.setDatabaseName(database_path->value() + "/" + "db");

        if (!db.open())
        {
            qDebug()<<"Cannot open database: unable to establish a database connection.";
            return;
        }

    }
}

template <typename T>
void medSQlite<T>::setDatabaseName(QString const &path)
{

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
    return db.isOpen();
}

template <typename T>
unsigned int medSQlite<T>::getLevelCount()
{
    return 0;
}

template <typename T>
QStringList medSQlite<T>::getLevelNames()
{
    return QStringList();
}

template <typename T>
QString medSQlite<T>::getLevelName(unsigned int pi_uiLevel)
{
    return QString();
}

template <typename T>
QStringList medSQlite<T>::getMandatoryAttributesKeys(unsigned int pi_uiLevel)
{
    return QStringList();
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

template<typename T>
QString medSQlite<T>::getInstanceName()
{
    return QString();
}

template<typename T>
QString medSQlite<T>::getInstanceId()
{
    return QString();
}
*/
