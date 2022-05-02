/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medTemporaryDataSource.h"

#include <medBoolParameter.h>

struct medTemporaryDataSourcePrivate
{
    QString instanceId;
    QString instanceName;
    // QMap<QString, QVariant> m_pluginKeyToDataMap;

    int levelCount;
    int desiredWritableLevel;
    medBoolParameter *flatTree;

    QMap<unsigned int /*level*/, QMap<QString, QVariant>> medAbstractDataMap;
    QMap<unsigned int, QMultiMap<QString, medAbstractSource::levelMinimalEntries>> minimalEntriesMap;
};

medTemporaryDataSource::medTemporaryDataSource()
    : d(new medTemporaryDataSourcePrivate)
{

    d->flatTree = new medBoolParameter("Flat Source");
    d->flatTree->setCaption("Flat Source");
    d->flatTree->setValue(true);
    d->flatTree->setDefaultRepresentation(2);

    d->levelCount = d->flatTree->value()?0:-1;
    d->desiredWritableLevel = d->flatTree->value()?0:-1;
    
    // QObject::connect(d->flatTree, &medBoolParameter::valueChanged, [&](bool state){ 
    //     int level = (state)? 0 : -1;
    //     d->levelCount = level;
    //     d->desiredWritableLevel = level;
    // });
}

medTemporaryDataSource::~medTemporaryDataSource()
{
}

bool medTemporaryDataSource::initialization(const QString &pi_instanceId)
{
    bool bRes = false;

    if (!pi_instanceId.isEmpty())
    {
        d->instanceId = pi_instanceId;
    }

    return bRes;
}

bool medTemporaryDataSource::setInstanceName(const QString &pi_instanceName)
{
    bool bRes = false;

    if (!pi_instanceName.isEmpty())
    {
        d->instanceName = pi_instanceName;
    }
    return bRes;
}

bool medTemporaryDataSource::connect(bool pi_bEnable)
{
    return true;
}

QList<medAbstractParameter *> medTemporaryDataSource::getAllParameters()
{
    return {d->flatTree};
}

QList<medAbstractParameter *> medTemporaryDataSource::getCipherParameters()
{
    return {};
}

QList<medAbstractParameter *> medTemporaryDataSource::getVolatilParameters()
{
    return {};
}

QList<medAbstractParameter *> medTemporaryDataSource::getFilteringParameters()
{
    return {};
}

bool medTemporaryDataSource::isWritable()
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
    return d->instanceName;
}

QString medTemporaryDataSource::getInstanceId()
{
    return d->instanceId;
}

unsigned int medTemporaryDataSource::getLevelCount()
{
    return d->levelCount;
}

unsigned int medTemporaryDataSource::getLevelDesiredWritable()
{
    return d->desiredWritableLevel;
}

QStringList medTemporaryDataSource::getLevelNames()
{
    return {};
}

QString medTemporaryDataSource::getLevelName(unsigned int pi_uiLevel)
{
    return "";
}

bool medTemporaryDataSource::isLevelWritable(unsigned int pi_uiLevel)
{
    return true;
}

QStringList medTemporaryDataSource::getMandatoryAttributesKeys(unsigned int pi_uiLevel)
{
    return {"key", "name", "desc"};
}

QList<medAbstractSource::levelMinimalEntries> medTemporaryDataSource::getMinimalEntries(unsigned int pi_uiLevel, QString parentId)
{
    QList<levelMinimalEntries> entries;
    auto minimalEntriesMultiMap = d->minimalEntriesMap[pi_uiLevel];
    entries = minimalEntriesMultiMap.values(parentId);

    return entries;
}

QList<QMap<QString, QString>> medTemporaryDataSource::getMandatoryAttributes(unsigned int pi_uiLevel, QString parentId)
{
    QList<QMap<QString, QString>> res;
    QList<levelMinimalEntries> entries;
    if (d->minimalEntriesMap.contains(pi_uiLevel))
    {
        auto minimalEntriesMultiMap = d->minimalEntriesMap[pi_uiLevel];
        qDebug() << "unique Keys " << minimalEntriesMultiMap.uniqueKeys();
        if (pi_uiLevel==0 && !minimalEntriesMultiMap.uniqueKeys().isEmpty())
        {
            entries = minimalEntriesMultiMap.values(minimalEntriesMultiMap.firstKey());
        }
        else
        {
            entries = minimalEntriesMultiMap.values(parentId);
        }
            
        for (auto entry : entries)
        {
            QMap<QString, QString> map;
            map["key"] = entry.key;
            map["name"] = entry.name;
            map["desc"] = entry.description;
            res.append(map);
        }
    }
    return res;
}

bool medTemporaryDataSource::getAdditionalAttributes(unsigned int pi_uiLevel, QString id, datasetAttributes4 &po_attributes)
{
    bool bRes = false;

    return bRes;
}

QVariant medTemporaryDataSource::getDirectData(unsigned int pi_uiLevel, QString key)
{
    auto uuidToDataMap = d->medAbstractDataMap[pi_uiLevel];
    return uuidToDataMap[key];
}

int medTemporaryDataSource::getAssyncData(unsigned int pi_uiLevel, QString id)
{
    return -1;
}

// QString medTemporaryDataSource::addData(QVariant data, QStringList parentUri, QString name)
// {
//     QString keyRes;
//     //    int sourceDelimterIndex = parentUri.indexOf(QString(":"));
//     //    QStringList uriAsList = parentUri.right(parentUri.size() - sourceDelimterIndex - 1).split(QString("\r\n"));
//     //    int level = uriAsList.size();
//     //    QString key = QString::number(level) + ":" + uriAsList.last();
//     //    if (!m_pluginKeyToDataMap.contains(key))
//     //    {
//     //        m_pluginKeyToDataMap[key] = data;
//     //    }
//     return keyRes;
// }

int medTemporaryDataSource::getIOInterface()
{
    // TODO
    return IO_MEDABSTRACTDATA;
}

QMap<QString, QStringList> medTemporaryDataSource::getTypeAndFormat()
{
    // TODO
    return QMap<QString, QStringList>();
}

bool medTemporaryDataSource::addDirectData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey)
{
    bool bRes = true;

    auto uuidToDataMap = d->medAbstractDataMap[pi_uiLevel];
    QString keyUid = QUuid::createUuid().toString().replace("{", "").replace("}", "");
    // QMap<QString, QVariant> uuidToDataMap;
    uuidToDataMap[keyUid] = data;
    d->medAbstractDataMap[pi_uiLevel] = uuidToDataMap;


    pio_minimalEntries.key = keyUid;
    auto minimalEntriesMultiMap = d->minimalEntriesMap[pi_uiLevel];
    // QMultiMap<QString, medAbstractSource::levelMinimalEntries> minimalEntriesMultiMap;
    minimalEntriesMultiMap.insert(parentKey, pio_minimalEntries);
    d->minimalEntriesMap[pi_uiLevel] = minimalEntriesMultiMap;

    return bRes;
}

int medTemporaryDataSource::addAssyncData(QVariant data, levelMinimalEntries & pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey)
{
    // TODO
    return -1;
}

QVariant medTemporaryDataSource::getAsyncResults(int pi_iRequest)
{
    // TODO
    return QVariant();
}

bool medTemporaryDataSource::createPath(QList<levelMinimalEntries> &pio_path, datasetAttributes4 const &pi_attributes, unsigned int pi_uiLevel, QString parentKey)
{
    // TODO
    return false;
}

bool medTemporaryDataSource::createFolder(levelMinimalEntries &pio_minimalEntries, datasetAttributes4 const &pi_attributes, unsigned int pi_uiLevel, QString parentKey)
{
    bool bRes = true;

    QString keyUid = QUuid::createUuid().toString().replace("{", "").replace("}", "");
    pio_minimalEntries.key = keyUid;

    // QMultiMap<QString, medAbstractSource::levelMinimalEntries> minimalEntriesMultiMap;
    auto minimalEntriesMultiMap = d->minimalEntriesMap[pi_uiLevel];
    minimalEntriesMultiMap.insert(parentKey, pio_minimalEntries);
    d->minimalEntriesMap[pi_uiLevel] = minimalEntriesMultiMap;
 
    return bRes;
}

bool medTemporaryDataSource::alterMetaData(datasetAttributes4 const &pi_attributes, unsigned int pi_uiLevel, QString key)
{
    // TODO
    return false;
}

bool medTemporaryDataSource::getThumbnail(QPixmap &po_thumbnail, unsigned int pi_uiLevel, QString key)
{
    // TODO
    return false;
}

bool medTemporaryDataSource::setThumbnail(QPixmap &pi_thumbnail, unsigned int pi_uiLevel, QString key)
{
    // TODO
    return false;
}

bool medTemporaryDataSource::commitData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey)
{
    // TODO
    return false;
}

int medTemporaryDataSource::push(unsigned int pi_uiLevel, QString key)
{
    // TODO
    return 0;
}

void medTemporaryDataSource::abort(int pi_iRequest)
{
}