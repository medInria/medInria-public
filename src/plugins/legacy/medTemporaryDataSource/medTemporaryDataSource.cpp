/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medTemporaryDataSource.h"

medTemporaryDataSource::medTemporaryDataSource()
        : medAbstractSource()
{
}

medTemporaryDataSource::~medTemporaryDataSource()
{
}

bool medTemporaryDataSource::initialization(const QString &pi_instanceId)
{
    bool bRes = false;

    if (!pi_instanceId.isEmpty())
    {
        m_instanceId = pi_instanceId;
    }
    return bRes;
}

bool medTemporaryDataSource::setInstanceName(const QString &pi_instanceName)
{
    bool bRes = false;

    if (!pi_instanceName.isEmpty())
    {
        m_instanceName = pi_instanceName;
    }
    return bRes;
}

bool medTemporaryDataSource::connect(bool pi_bEnable)
{
    return true;
}

QList<medAbstractParameter *> medTemporaryDataSource::getAllParameters()
{
    QList<medAbstractParameter *> paramListRes;

    return paramListRes;
}

QList<medAbstractParameter *> medTemporaryDataSource::getCipherParameters()
{
    return {};
}

QList<medAbstractParameter *> medTemporaryDataSource::getVolatilParameters()
{
    return {};
}

bool medTemporaryDataSource::isWriteable()
{
    return true;
}

bool medTemporaryDataSource::isLocal()
{
    return true;
}

bool medTemporaryDataSource::isCached()
{
    return true;
}

bool medTemporaryDataSource::isOnline()
{
    return true;
}

bool medTemporaryDataSource::isFetchByMinimalEntriesOrMandatoryAttributes()
{
    return false;
}

QString medTemporaryDataSource::getInstanceName()
{
    return m_instanceName;
}

QString medTemporaryDataSource::getInstanceId()
{
    return m_instanceId;
}

unsigned int medTemporaryDataSource::getLevelCount()
{
    return 3;
}

QStringList medTemporaryDataSource::getLevelNames()
{
    return {"Patient", "Study", "Series"};
}

QString medTemporaryDataSource::getLevelName(unsigned int pi_uiLevel)
{
    return "TemporaryData";
}

QStringList medTemporaryDataSource::getMandatoryAttributesKeys(unsigned int pi_uiLevel)
{
    return {};
}

QList<medAbstractSource::levelMinimalEntries> medTemporaryDataSource::getMinimalEntries(unsigned int pi_uiLevel, QString parentId)
{
    QList<levelMinimalEntries> entries;

    return entries;
}

QList<QMap<QString, QString>> medTemporaryDataSource::getMandatoryAttributes(unsigned int pi_uiLevel, QString parentId)
{
    QList < QMap<QString, QString>> res;

    return res;
}

bool medTemporaryDataSource::getAdditionalAttributes(unsigned int pi_uiLevel, QString id, datasetAttributes4 &po_attributes)
{
    bool bRes = false;
    return bRes;
}

QVariant medTemporaryDataSource::getDirectData(unsigned int pi_uiLevel, QString key)
{
    QString localKey = QString::number(pi_uiLevel) + ":" + key;
    QVariant res;
    if (m_pluginKeyToDataMap.contains(localKey))
    {
        res = m_pluginKeyToDataMap[localKey];
    }
    return res;
}

int medTemporaryDataSource::getAssyncData(unsigned int pi_uiLevel, QString id)
{
    return -1;
}

QString medTemporaryDataSource::addData(QVariant data, QStringList parentUri, QString name)
{
    QString keyRes;
//    int sourceDelimterIndex = parentUri.indexOf(QString(":"));
//    QStringList uriAsList = parentUri.right(parentUri.size() - sourceDelimterIndex - 1).split(QString("\r\n"));
//    int level = uriAsList.size();
//    QString key = QString::number(level) + ":" + uriAsList.last();
//    if (!m_pluginKeyToDataMap.contains(key))
//    {
//        m_pluginKeyToDataMap[key] = data;
//    }
    return keyRes;
}

void medTemporaryDataSource::abort(int pi_iRequest)
{

}
