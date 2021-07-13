/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSourcesLoader.h>

#include <QString>
#include <QMap>

#include <QFile>
#include <QStandardPaths    >
#include <QCryptographicHash>
//#include <QPasswordDigestor >
#include <QJsonObject       >
#include <QJsonDocument     >
#include <QTimer            >
#include <QMutex            >
#include <QSharedPointer    >

medDBSourcesLoader *medDBSourcesLoader::s_instance = nullptr;

medDBSourcesLoader *medDBSourcesLoader::instance(void)
{
    if (!s_instance)
        s_instance = new medDBSourcesLoader;
    return s_instance;
}

medDBSourcesLoader::~medDBSourcesLoader()
{
    m_sourcesInstances;
    m_sourcesModelMap;
}

bool medDBSourcesLoader::registerSourceType(QString sourceType, QString shortDescr, QString longDescr, instanciateSource instanciator)
{
    bool bRes = (!sourceType.isEmpty() && instanciator != nullptr && !m_sourcesModelMap.contains(sourceType));
    
    if (bRes)
    {
        medSourceTool tool = { sourceType, shortDescr, longDescr, instanciator };
        m_sourcesModelMap.insert(sourceType, tool);
    }

    return bRes;
}

QList<std::tuple<QString, QString, QString>> medDBSourcesLoader::sourcesTypeAvailables()
{
    QList<std::tuple<QString, QString, QString>> listRes;
    
    for (auto model : m_sourcesModelMap)
    {
        listRes.append(std::tuple<QString, QString, QString>(model.id, model.name, model.Description));
    }

    return listRes;
}

bool medDBSourcesLoader::createNewCnx(QString & IdName, QString const & sourceType)
{
    bool bRes = true;

    m_mutexMap.lock();
    bRes = ensureUniqueSourceIdName(IdName);
    
    if (bRes)
    {
        medAbstractSource *pDataSource = createInstanceOfSource(sourceType, IdName, IdName);
    
        bRes = pDataSource != nullptr;
        if (bRes)
        {
            m_cnxMapSource[IdName] = sourceType;
            m_sourcesInstances.insert(IdName, QSharedPointer<medAbstractSource>(pDataSource));
            saveToDisk();
            emit(sourceAdded(pDataSource));
        }
    }
    m_mutexMap.unlock();

    return bRes;
}

bool medDBSourcesLoader::removeOldCnx(QString & IdName)
{
    bool bRes = false;

    m_mutexMap.lock();
    if (m_sourcesInstances.contains(IdName))
    {
        auto oldCnx = m_sourcesInstances.take(IdName);
        m_cnxMapSource.remove(IdName);
        saveToDisk();
        QTimer::singleShot(5*60*1000, this, [&]() {oldCnx.reset(); }); //the removed connection will be deleted after 5 min of secured time gap
    }
    m_mutexMap.unlock();

    return bRes;
}

QList<medAbstractSource*> medDBSourcesLoader::sourcesList()
{
    QList<medAbstractSource*> instanceList;
    
    for (auto instance : m_sourcesInstances)
    {
        instanceList.push_back(&(*instance));
    }
    
    return instanceList;
}

medAbstractSource * medDBSourcesLoader::getSource(QString IdName)
{
    medAbstractSource *sourceRes = nullptr;

    if (m_sourcesInstances.contains(IdName))
    {
        sourceRes = &(*m_sourcesInstances[IdName]);
    }

    return sourceRes;
}




















medAbstractSource* medDBSourcesLoader::createInstanceOfSource(QString const & sourceType, QString const & IdName, QString const & Name)
{
    medAbstractSource * pDataSource = nullptr;
    
    if (m_sourcesModelMap.contains(sourceType))
    {
        pDataSource = m_sourcesModelMap[sourceType].instanciator(IdName, Name);
    }

    return pDataSource;
}

medDBSourcesLoader::medDBSourcesLoader()
{
    m_CnxParametersPath = ".";
}

bool medDBSourcesLoader::saveToDisk()
{
    bool bRes = true;

    QJsonDocument jsonSaveDoc;
    QJsonArray entries;
    for (auto instance : m_sourcesInstances)
    {
        QJsonObject entry;
        entry.insert("sourceTypeId", QJsonValue::fromVariant(m_cnxMapSource[instance->getInstanceId()]));
        entry.insert("conxId", QJsonValue::fromVariant(instance->getInstanceId()));
        entry.insert("cnxName", QJsonValue::fromVariant(instance->getInstanceName()));

        entries.push_back(entry);
    }
    jsonSaveDoc.setArray(entries);
    QByteArray payload = jsonSaveDoc.toJson();

    QFile cnxSourcesParametersFile(m_CnxParametersPath);
    bRes = cnxSourcesParametersFile.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    if (bRes)
    {
        bRes = cnxSourcesParametersFile.write(payload) != -1;
        cnxSourcesParametersFile.close();
    }

    return bRes;
}

bool medDBSourcesLoader::ensureUniqueSourceIdName(QString & IdName)
{
    bool bRes = !IdName.isEmpty();

    QString IdNameTmp = IdName;
    unsigned int suffix = 0;
    while (m_sourcesInstances.contains(IdNameTmp) && suffix < 32767)
    {
        suffix++;
        IdNameTmp = IdName + "_" + suffix;
    }
    
    bRes &= suffix < 32768;
    if (bRes)
    {
        IdName = IdNameTmp;
    }

    return bRes;
}

