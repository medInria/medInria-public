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

#define MED_DATASOURCES_FILENAME "DataSources.json"

medSourcesLoader *medSourcesLoader::s_instance = nullptr;

medSourcesLoader *medSourcesLoader::instance(QObject *parent)
{
    if (!s_instance)
        s_instance = new medSourcesLoader(parent);
    return s_instance;
}

medSourcesLoader::~medSourcesLoader()
{
}

/**
* @brief  Register a new type of source.
* @param  type is identifier of source type. It must be not empty.
* @param  name is the name of source type.
* @param  description is human readable short description .
* @param  instantiator is the function pointer to instantiate this type of source.
*         Function pointer signature must be medAbstractSource* (*)()
*         instantiator must be not nullptr or already registered.
* @return Returns true if this type of source was registered. False otherwise.
*/
bool medSourcesLoader::registerSourceType(QString type, QString name, QString description, instantiateSource instantiator)
{
    bool bRes = (!type.isEmpty() && instantiator != nullptr && !m_sourcesMap.contains(type));

    m_mutexMap.lock();
    if (bRes)
    {
        medSourceTool tool = { type, name, description, instantiator };
        m_sourcesMap.insert(type, tool);
    }
    reparseUnresolvedCnx();  //Try to re-read failed connections to a source 
    m_mutexMap.unlock();

    return bRes;
}

QList<std::tuple<QString, QString, QString>> medSourcesLoader::sourcesTypeAvailables()
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

bool medSourcesLoader::createCnx(QString & instanceId, QString const & type)
{
    bool bRes = true;

    m_mutexMap.lock();
    bRes = generateUniqueSourceId(instanceId, type);
    
    if (bRes)
    {
        medAbstractSource *pDataSource = createInstanceOfSource(type);
    
        bRes = pDataSource != nullptr;
        if (bRes)
        {
            pDataSource->initialization(instanceId);
            pDataSource->setInstanceName(instanceId);

            m_instanceMapType[instanceId] = type;
            m_instancesMap.insert(instanceId, QSharedPointer<medAbstractSource>(pDataSource));
            saveToDisk();
            emit(sourceAdded(pDataSource));
        }
    }
    m_mutexMap.unlock();

    return bRes;
}

bool medSourcesLoader::removeCnx(QString const & instanceId)
{
    bool bRes = false;

    m_mutexMap.lock();
    if (m_instancesMap.contains(instanceId))
    {
        auto oldCnx = m_instancesMap.take(instanceId);
        m_instanceMapType.remove(instanceId);
        saveToDisk();
        QTimer::singleShot(5*60*1000, this, [&]() {oldCnx.reset(); }); //the removed connection will be deleted after 5 min of secured time gap
    }
    m_mutexMap.unlock();

    return bRes;
}

QList<medAbstractSource*> medSourcesLoader::sourcesList()
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

bool medSourcesLoader::renameSource(QString const & instanceId, QString const & name)
{
    bool bRes = false;

    if (!name.isEmpty())
    {
        m_mutexMap.lock();
        if (m_instancesMap.contains(instanceId))
        {
            m_instancesMap[instanceId]->setInstanceName(name);
            bRes = saveToDisk();
        }
        m_mutexMap.unlock();
    }

    return bRes;
}

medAbstractSource * medSourcesLoader::getSource(QString const &instanceId)
{
    medAbstractSource *sourceRes = nullptr;

    m_mutexMap.lock();
    if (m_instancesMap.contains(instanceId))
    {
        sourceRes = &(*m_instancesMap[instanceId]);
    }
    m_mutexMap.unlock();

    return sourceRes;
}




















medAbstractSource* medSourcesLoader::createInstanceOfSource(QString const & type) const
{
    medAbstractSource * pDataSource = nullptr;
    
    if (m_sourcesMap.contains(type))
    {
        pDataSource = m_sourcesMap[type].instanciator();
    }

    return pDataSource;
}

medSourcesLoader::medSourcesLoader(QObject *parent)
{
    setParent(parent);
    m_CnxParametersPath = ".";
}

bool medSourcesLoader::saveToDisk()
{
    bool bRes = true;

    QJsonDocument jsonSaveDoc;
    QJsonArray entries;
    for (auto instance : m_instancesMap)
    {
        QJsonObject entry;
        entry.insert("sourceType", QJsonValue::fromVariant(m_instanceMapType[instance->getInstanceId()]));
        entry.insert("cnxId", QJsonValue::fromVariant(instance->getInstanceId()));
        entry.insert("cnxName", QJsonValue::fromVariant(instance->getInstanceName()));
        
        auto allParameters     = instance->getAllParameters();
        auto cipherParameters  = instance->getCipherParameters();
        auto volatilParameters = instance->getVolatilParameters();

        for (auto paramToRemove : cipherParameters)
        {
            allParameters.removeAll(paramToRemove);
        }
        for (auto paramToRemove : volatilParameters)
        {
            allParameters.removeAll(paramToRemove);
        }

        QJsonObject entryParameters;
        for (auto medParam : allParameters)
        {
            QJsonObject paramAsJson = QJsonObject::fromVariantMap(medParam->toVariantMap());
            entryParameters.insert(medParam->id(), paramAsJson);
        }
        for (auto medParam : cipherParameters)
        {
            QJsonObject paramAsJson;
            convertCipherParamToJson(paramAsJson, medParam); //TODO Cipher
            entryParameters.insert(medParam->id(), paramAsJson);
        }
        entry.insert("parameters", entryParameters);
        entries.push_back(entry);
    }

    for (auto unresolvedSavedCnxEntry : m_unresolvedSavedCnx)
    {
        entries.push_back(unresolvedSavedCnxEntry);
    }

    jsonSaveDoc.setArray(entries);
    QByteArray payload = jsonSaveDoc.toJson();

    QFile cnxSourcesParametersFile(m_CnxParametersPath + "/" + MED_DATASOURCES_FILENAME);
    bRes = cnxSourcesParametersFile.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    if (bRes)
    {
        bRes = cnxSourcesParametersFile.write(payload) != -1;
        cnxSourcesParametersFile.close();
    }

    return bRes;
}

bool medSourcesLoader::loadFromDisk()
{
    bool bRes = false;

    QString content;

    QFile cnxSourcesParametersFile;
    cnxSourcesParametersFile.setFileName(m_CnxParametersPath + "/" + MED_DATASOURCES_FILENAME);
    bRes = cnxSourcesParametersFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (bRes)
    {
        int iCnxOk = 0;
        int iCnxWithoutPlugin = 0;
        int iCnxInvalid = 0;

        content = cnxSourcesParametersFile.readAll();
        cnxSourcesParametersFile.close();

        qWarning() << content;
        QJsonDocument jsonSaveDoc = QJsonDocument::fromJson(content.toUtf8());
        QJsonArray entries = jsonSaveDoc.array();
        for (auto entry : entries)
        {
            auto obj = entry.toObject();

            if (obj.contains("sourceType") && obj["sourceType"].isString() && !obj["sourceType"].toString().isEmpty() &&
                obj.contains("cnxId") && obj["cnxId"].isString() && !obj["cnxId"].toString().isEmpty() &&
                obj.contains("cnxName") && obj["cnxName"].isString() && !obj["cnxName"].toString().isEmpty())
            {
                if (m_sourcesMap.contains(obj["sourceType"].toString()))
                {
                    reloadCnx(obj);
                    iCnxOk++;
                }
                else
                {
                    qWarning() << "[WARN] Source loading can't find dataSource plugin for : "
                        << "\nSource type     = " << obj["sourceType"]
                        << "\nConnection Id   = " << obj["cnxId"]
                        << "\nConnection name = " << obj["cnxName"];
                    m_unresolvedSavedCnx.push_back(obj);
                    iCnxWithoutPlugin++;
                }
            }
            else
            {
                qWarning() << "[WARN] Source loading invalid entry detected : "
                    << "\nSource type     = " << obj["sourceType"]
                    << "\nConnection Id   = " << obj["cnxId"]
                    << "\nConnection name = " << obj["cnxName"];
                iCnxInvalid++;
            }
        }
        qWarning() << "[INFO] Source loading statistics : "
            << "\nConnection ok             = " << iCnxOk
            << "\nConnection without plugin = " << iCnxWithoutPlugin
            << "\nConnection invalid        = " << iCnxInvalid;
        bRes = iCnxOk > 0;
    }

    return bRes;
}

void medSourcesLoader::reloadCnx(QJsonObject &obj)
{
    int iAppliedParametersCount = 0;
    auto pDataSource = createInstanceOfSource(obj["sourceType"].toString());

    pDataSource->initialization(obj["cnxId"].toString());
    pDataSource->setInstanceName(obj["cnxName"].toString());
    auto normalParameters = pDataSource->getAllParameters();
    auto cipherParameters = pDataSource->getCipherParameters();
    auto volatilParameters = pDataSource->getVolatilParameters();

    for (auto paramToRemove : cipherParameters)
    {
        normalParameters.removeAll(paramToRemove);
    }
    for (auto paramToRemove : volatilParameters)
    {
        normalParameters.removeAll(paramToRemove);
    }

    for (QString paramId : obj["parameters"].toObject().keys())
    {
        bool bFind = false;
        int i = 0;

        while (!bFind && i< normalParameters.size())
        {
            bFind = paramId == normalParameters[i]->id();
            if (!bFind) i++;
        }

        if (bFind)
        {
            if (normalParameters[i]->fromVariantMap(obj["parameters"].toObject()[paramId].toObject().toVariantMap()))
            {
                iAppliedParametersCount++;
            }
            else
            {
                qWarning() << "[WARN] Source loading invalid parameter detected for : "
                    << "\nSource type     = " << obj["sourceType"]
                    << "\nConnection Id   = " << obj["cnxId"]
                    << "\nConnection name = " << obj["cnxName"]
                    << "\nParameter Name  = " << normalParameters[i]->caption();
            }
            normalParameters.removeAt(i);
        }
        else
        {
            i = 0;
            while (!bFind && i < cipherParameters.size())
            {
                bFind = paramId != cipherParameters[i]->id();
                if (!bFind) i++;
            }
            if (bFind)
            {
                if (cipherParameters[i]->fromVariantMap(obj["parameters"].toObject()[paramId].toObject().toVariantMap())) //TODO Handle cipher param
                {
                    iAppliedParametersCount++;
                }
                else
                {
                    qWarning() << "[WARN] Source loading invalid parameter detected for : "
                        << "\nSource type     = " << obj["sourceType"]
                        << "\nConnection Id   = " << obj["cnxId"]
                        << "\nConnection name = " << obj["cnxName"]
                        << "\nCipher Parameter Name  = " << normalParameters[i]->caption();
                }
                cipherParameters.removeAt(i);
            }
            else
            {
                qWarning() << "[WARN] Source loading invalid parameter detected for : "
                    << "\nSource type     = " << obj["sourceType"]
                    << "\nConnection Id   = " << obj["cnxId"]
                    << "\nConnection name = " << obj["cnxName"]
                    << "\nParameter name  = " << paramId;
            }
        }

        if (!normalParameters.isEmpty() || !cipherParameters.isEmpty())
        {
            qWarning() << "[WARN] Source loading empty parameter(s) detected for : "
                << "\nSource type     = " << obj["sourceType"]
                << "\nConnection Id   = " << obj["cnxId"]
                << "\nConnection name = " << obj["cnxName"];
            for (auto param : normalParameters)
            {
                qWarning() << "    Parameter Id = " << param->id();
                qWarning() << "    Parameter Name = " << param->caption();
            }
            for (auto param : cipherParameters)
            {
                qWarning() << "    Parameter Id = " << param->id();
                qWarning() << "    Parameter Name = " << param->caption();
            }
        }

    }

    int iParamLakeCount = iAppliedParametersCount - (pDataSource->getAllParameters().size() - pDataSource->getVolatilParameters().size());
    if (iParamLakeCount < 0)
    {
        qWarning() << "[WARN] Source loading lake " << iParamLakeCount << " parameter detected for : "
            << "\nSource type     = " << obj["sourceType"]
            << "\nConnection Id   = " << obj["cnxId"]
            << "\nConnection name = " << obj["cnxName"];
    }

    m_instanceMapType[obj["cnxId"].toString()] = obj["sourceType"].toString();
    m_instancesMap.insert(obj["cnxId"].toString(), QSharedPointer<medAbstractSource>(pDataSource));
    pDataSource->connect();
    emit(sourceAdded(pDataSource));
}

bool medSourcesLoader::generateUniqueSourceId(QString & Id, QString const & type) const
{
    bool bRes = true;
    
    if (Id.isEmpty())
    {
        auto date = QDate::currentDate().toString("ddMMyy");
        Id = type + "_" + date;
    }

    QString IdTmp = Id;
    unsigned int suffix = 0;
    while (m_instancesMap.contains(IdTmp) && suffix < 32767 && bRes) //Warning no more than 32767 cnx on same source
    {
        suffix++;
        IdTmp = Id + "_" + QString::number(suffix);
    }
    
    bRes = suffix < 32768;
    if (bRes)
    {
        Id = IdTmp;
    }

    return bRes;
}

void medSourcesLoader::reparseUnresolvedCnx()
{
    int i = 0;
    while ( i< m_unresolvedSavedCnx.size())
    {        
        if (m_sourcesMap.keys().contains(m_unresolvedSavedCnx[i]["sourceType"].toString()))
        {
            auto cnx = m_unresolvedSavedCnx.takeAt(i); 
            reloadCnx(cnx);
        }
        else
        {
            i++;
        }
    }
}



void medSourcesLoader::convertCipherParamToJson(QJsonObject & po_oJson, medAbstractParameter * pi_pParam)
{
    po_oJson = QJsonObject::fromVariantMap(pi_pParam->toVariantMap());
}

