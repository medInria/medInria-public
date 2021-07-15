/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medSQLite.h"

template <typename T>
medSQlite<T>::medSQlite()
        : medAbstractSource(), m_Driver("QSQLITE"), m_ConnectionName("sqlite"), m_instanceId(QString())
{
    m_DbPath = new medStringParameter("LocalDataBasePath", this);
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
    return false;
}

template<typename T>
bool medSQlite<T>::connect(bool pi_bEnable)
{
    bool bRes = m_Engine.isValid();
    if (bRes)
    {
        if (pi_bEnable) // establish connection
        {
            if (!m_DbPath->value().isEmpty())
            {
                m_Engine.setDatabaseName(m_DbPath->value() + "/" + "db");
                bRes = m_Engine.open();
            }
            else
            {
                bRes = false;
            }
        }
        else //disconnect
        {
            m_Engine.close();
        }
    }
    // TODO : else...
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
    return false;//db.isOpen();
}

template<typename T>
QString medSQlite<T>::getInstanceName()
{
    return QString();
}

template<typename T>
QString medSQlite<T>::getInstanceId()
{
    return m_instanceId;
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

template <typename T>
void medSQlite<T>::updateDatabaseName(QString const &path)
{

}


