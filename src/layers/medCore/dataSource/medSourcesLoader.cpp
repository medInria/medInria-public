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
#include <QStandardPaths>
#include <QCryptographicHash>
//#include <QPasswordDigestor >
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <QMutex>
#include <QSharedPointer>

medDBSourcesLoader *medDBSourcesLoader::s_instance = nullptr;

medDBSourcesLoader *medDBSourcesLoader::instance(void)
{
    if (!s_instance)
        s_instance = new medDBSourcesLoader;
    return s_instance;
}

medDBSourcesLoader::~medDBSourcesLoader()
{
    m_instancesMap;
    m_sourcesMap;
}

bool medDBSourcesLoader::registerSourceType(QString type, QString name, QString description, instanciateSource instanciator)
{
    bool bRes = (!type.isEmpty() && instanciator != nullptr && !m_sourcesMap.contains(type));

    m_mutexMap.lock();
    if (bRes)
    {
        medSourceTool tool = { type, name, description, instanciator };
        m_sourcesMap.insert(type, tool);
    }
    m_mutexMap.unlock();

    return bRes;
}

QList<std::tuple<QString, QString, QString>> medDBSourcesLoader::sourcesTypeAvailables()
{
    QList<std::tuple<QString, QString, QString>> listRes;

    m_mutexMap.lock();
    for (auto model : m_sourcesMap)
    {
        listRes.append(std::tuple<QString, QString, QString>(model.type, model.name, model.description));
    }
    m_mutexMap.unlock();

    return listRes;
}

bool medDBSourcesLoader::createNewCnx(QString & IdName, QString const & type)
{
    bool bRes = true;

    m_mutexMap.lock();
    bRes = ensureUniqueSourceIdName(IdName);
    
    if (bRes)
    {
        medAbstractSource *pDataSource = createInstanceOfSource(type, IdName, IdName);
    
        bRes = pDataSource != nullptr;
        if (bRes)
        {
            pDataSource->initialization(IdName);
            pDataSource->setInstanceName(IdName);

            m_instanceMapType[IdName] = type;
            m_instancesMap.insert(IdName, QSharedPointer<medAbstractSource>(pDataSource));
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
    if (m_instancesMap.contains(IdName))
    {
        auto oldCnx = m_instancesMap.take(IdName);
        m_instanceMapType.remove(IdName);
        saveToDisk();
        QTimer::singleShot(5*60*1000, this, [&]() {oldCnx.reset(); }); //the removed connection will be deleted after 5 min of secured time gap
    }
    m_mutexMap.unlock();

    return bRes;
}

QList<medAbstractSource*> medDBSourcesLoader::sourcesList()
{
    QList<medAbstractSource*> instanceList;

    m_mutexMap.lock();
    for (auto instance : m_instancesMap)
    {
        instanceList.push_back(&(*instance));
    }
    m_mutexMap.unlock();
    
    return instanceList;
}

medAbstractSource * medDBSourcesLoader::getSource(QString IdName)
{
    medAbstractSource *sourceRes = nullptr;

    m_mutexMap.lock();
    if (m_instancesMap.contains(IdName))
    {
        sourceRes = &(*m_instancesMap[IdName]);
    }
    m_mutexMap.unlock();

    return sourceRes;
}




















medAbstractSource* medDBSourcesLoader::createInstanceOfSource(QString const & type, QString const & IdName, QString const & Name)
{
    medAbstractSource * pDataSource = nullptr;
    
    if (m_sourcesMap.contains(type))
    {
//        pDataSource = m_sourcesMap[type].instanciator(IdName, Name);
        pDataSource = m_sourcesMap[type].instanciator();
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
    for (auto instance : m_instancesMap)
    {
        QJsonObject entry;
        entry.insert("sourceTypeId", QJsonValue::fromVariant(m_instanceMapType[instance->getInstanceId()]));
        entry.insert("conxId", QJsonValue::fromVariant(instance->getInstanceId()));
        entry.insert("cnxName", QJsonValue::fromVariant(instance->getInstanceName()));
        //TODO PARAMETERS
        entries.push_back(entry);
    }
    jsonSaveDoc.setArray(entries);
    QByteArray payload = jsonSaveDoc.toJson();

    QFile cnxSourcesParametersFile(m_CnxParametersPath + "/foo.json");
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
    while (m_instancesMap.contains(IdNameTmp) && suffix < 32767) // TODO bRes check
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

