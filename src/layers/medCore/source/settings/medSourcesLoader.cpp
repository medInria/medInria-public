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

#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <QMutex>
#include <QSharedPointer>

#include <medSettingsManager.h>
#include <medStorage.h>

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

            m_sourcesIDs.push_back(instanceId);
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
        m_sourcesIDs.removeAt(m_sourcesIDs.indexOf(instanceId));
        m_instanceMapType.remove(instanceId);
        saveToDisk();
        emit sourceRemoved(&(*oldCnx));
        QTimer::singleShot(5*60*1000, this, [oldCnx]() 
        {   //Do nothing into the lambda because oldCnx is a shared pointer copied by value passing. Then it will be automatically deleted at the end of lambda execution/scope.
            //Solution to avoid this timer is to used only QSharedPointer when used a source instance.
        }); //the removed connection will be deleted after 5 min of secured time gap
    }
    m_mutexMap.unlock();

    return bRes;
}

QList<medAbstractSource*> medSourcesLoader::sourcesList()
{
    QList<medAbstractSource*> instanceList;

    m_mutexMap.lock();
    for (auto instance : m_sourcesIDs)
    {
        instanceList.push_back(&(*m_instancesMap[instance]));
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
            emit sourcesUpdated();
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

medAbstractSource * medSourcesLoader::getDefaultWorkingSource()
{
    return &(*m_instancesMap.value(medSettingsManager::instance().value("Sources", "Default", "").toString()));
}


bool medSourcesLoader::setDefaultWorkingSource(QString const &instanceId)
{
    bool bRes = m_instancesMap.contains(instanceId);

    if (bRes)
    {
        auto source = m_instancesMap[instanceId];
        bool bWritable = source->isWritable();
        bool bLocal = source->isLocal();
        bool bCached = source->isCached();
        bool bOnline = source->isOnline();

        if (bWritable && (bLocal || (!bLocal && bCached && bOnline)))
        {
            m_defaultSource = &(*source);
            medSettingsManager::instance().setValue("Sources", "Default", instanceId);
            emit defaultWorkingSource(m_defaultSource);
        }
        else
        {
            bRes = false;
        }
    }
    return bRes;
}








bool medSourcesLoader::setPath(QString path)
{
    bool bRes = true;

    if (QDir(path).exists())
    {
        if (QFile::exists(path + '/' + MED_DATASOURCES_FILENAME))
        {
            medSettingsManager::instance().setValue("Sources", "Conf dir", path);
        }
        else
        {
            auto oldPath = m_CnxParametersPath;
            m_CnxParametersPath = path;
            if (!saveToDisk())
            {
                m_CnxParametersPath = oldPath;
                bRes = false;
            }
        }
    }
    else if (QFile::exists(path))
    {
        medSettingsManager::instance().setValue("Sources", "Conf dir", path);
    }
    else
    {
        bRes = false;
    }

    return bRes;
}

QString medSourcesLoader::getPath()
{
    return m_CnxParametersFile == MED_DATASOURCES_FILENAME ? m_CnxParametersPath : m_CnxParametersPath + "/" + m_CnxParametersFile;
}

QString medSourcesLoader::path()
{
    QString cnxParametersFile = MED_DATASOURCES_FILENAME;
    QString cnxParametersPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/" + QCoreApplication::organizationName() + "/" + QCoreApplication::applicationName();

    auto cnxParametersSaved = medSettingsManager::instance()->value("Sources", "Conf dir", ".").toString();

    QFileInfo info(cnxParametersSaved);
    if (info.isFile())
    {
        cnxParametersFile = info.fileName();
        cnxParametersPath = info.dir().path();
    }
    else if (QDir(cnxParametersSaved).exists())
    {
        cnxParametersPath = cnxParametersSaved;
    }

    return cnxParametersPath + '/' + cnxParametersFile;
}

bool medSourcesLoader::initSourceLoaderCfg(QString src, QString dst)
{
    bool bRes = true;

    QFile file(src);

    if (!file.open(QFile::ReadOnly | QIODevice::Text))
    {
        bRes = false;
    }
    else
    {
        int iCnxOk = 0;
        int iCnxWithoutPlugin = 0;
        int iCnxInvalid = 0;

        file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
        QString content = file.readAll();
        file.close();


        QJsonDocument jsonSaveDoc = QJsonDocument::fromJson(content.toUtf8());
        QJsonArray entries = jsonSaveDoc.array();
        for (QJsonValueRef & entry : entries)
        {
            auto obj = entry.toObject();

            if (obj.contains("sourceType") && obj["sourceType"].isString() && !obj["sourceType"].toString().isEmpty() &&
                obj.contains("cnxId") && obj["cnxId"].isString() && !obj["cnxId"].toString().isEmpty() &&
                obj.contains("cnxName") && obj["cnxName"].isString() && !obj["cnxName"].toString().isEmpty())
            {
                if (obj["sourceType"].toString() == "medSQLite" && obj["cnxId"].toString() == "medSQLite_default")
                {
                    if (obj.contains("parameters") && obj["parameters"].isObject())
                    {
                        QJsonObject params = obj["parameters"].toObject();
                        if (params.contains("LocalDataBasePath") && params["LocalDataBasePath"].isObject())
                        {
                            QJsonObject localDataBasePath = params["LocalDataBasePath"].toObject();
                            localDataBasePath["value"] = medStorage::dataLocation();

                            params["LocalDataBasePath"] = localDataBasePath;
                            obj["parameters"] = params;

                            entry = obj;

                            jsonSaveDoc.setArray(entries);
                        }
                    }
                }
            }
        }


        if (!file.open(QFile::WriteOnly | QIODevice::Text))
        {
            bRes = false;
        }
        else
        {
            file.write(jsonSaveDoc.toJson());
        }
    }

    return bRes;
}


medAbstractSource* medSourcesLoader::createInstanceOfSource(QString const & type) const
{
    medAbstractSource * pDataSource = nullptr;
    
    if (m_sourcesMap.contains(type))
    {
        pDataSource = m_sourcesMap[type].instanciator();
        for (auto *parameter : pDataSource->getAllParameters())
        {
            connect(parameter, &medAbstractParameter::triggered, [=]() {this->saveToDisk(); });
        }
    }

    return pDataSource;
}

medSourcesLoader::medSourcesLoader(QObject *parent)
{
    setParent(parent);
    m_CnxParametersFile = MED_DATASOURCES_FILENAME;
    m_CnxParametersPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/" + QCoreApplication::organizationName() + "/" + QCoreApplication::applicationName();

    auto cnxParametersSaved = medSettingsManager::instance().value("Sources", "Conf dir", ".").toString();
    
    QFileInfo info(cnxParametersSaved); 
    if (info.isFile())
    {
        m_CnxParametersFile = info.fileName();
        m_CnxParametersPath = info.dir().path();
    }
    else if (QDir(cnxParametersSaved).exists())
    {
        m_CnxParametersPath = cnxParametersSaved;
    }
}

bool medSourcesLoader::saveToDisk() const
{
    bool bRes = true;

    QJsonDocument jsonSaveDoc;
    QJsonArray entries;
    for (auto instanceId : m_sourcesIDs)
    {
        auto instance = m_instancesMap[instanceId];
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
            convertCipherParamToJson(paramAsJson, medParam); 
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

    QFile cnxSourcesParametersFile(m_CnxParametersPath + "/" + m_CnxParametersFile);
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
    cnxSourcesParametersFile.setFileName(m_CnxParametersPath + "/" + m_CnxParametersFile);
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

    if (bRes)
    {
        setDefaultWorkingSource(medSettingsManager::instance().value("Sources", "Default", 0).toString());
    }

    return bRes;
}

void medSourcesLoader::changeSourceOrder(int oldPlace, int newPlace)
{
    if (oldPlace < m_sourcesIDs.size() && newPlace < m_sourcesIDs.size() && oldPlace != newPlace)
    {
        m_sourcesIDs.insert(oldPlace > newPlace ? newPlace : newPlace-1, m_sourcesIDs.takeAt(oldPlace));
        emit sourcesUpdated();
    }
    saveToDisk();
}

void medSourcesLoader::reloadCnx(QJsonObject &obj)
{
    int iAppliedParametersCount = 0;
    auto pDataSource = createInstanceOfSource(obj["sourceType"].toString());

    pDataSource->initialization(obj["cnxId"].toString()); //TODO HANDLE return (especily when SQLDriver is not loaded)
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
                bFind = paramId == cipherParameters[i]->id();
                if (!bFind) i++;
            }
            if (bFind)
            {
				//deciphering the ciphered value
				QJsonObject decipheredParam = obj["parameters"].toObject()[paramId].toObject();
				decipherJson(decipheredParam);
                if (cipherParameters[i]->fromVariantMap(decipheredParam.toVariantMap())) 
                {
					iAppliedParametersCount++;
                }
                else
                {
                    qWarning() << "[WARN] Source loading invalid parameter detected for : "
                        << "\nSource type     = " << obj["sourceType"]
                        << "\nConnection Id   = " << obj["cnxId"]
                        << "\nConnection name = " << obj["cnxName"]
                        << "\nCipher Parameter Name  = " << cipherParameters[i]->caption();
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

    int iParamLakeCount = iAppliedParametersCount - (pDataSource->getAllParameters().size() - pDataSource->getVolatilParameters().size());
    if (iParamLakeCount < 0)
    {
        qWarning() << "[WARN] Source loading lake " << iParamLakeCount << " parameter detected for : "
            << "\nSource type     = " << obj["sourceType"]
            << "\nConnection Id   = " << obj["cnxId"]
            << "\nConnection name = " << obj["cnxName"];
    }
    m_sourcesIDs.push_back(obj["cnxId"].toString());
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
	QString cipherValue = po_oJson.value("value").toString();

	int keyLength = ENCRYPTION_ITERATIONS * ceil(std::max(cipherValue.length(), 512 / 8) / QCryptographicHash::hashLength(ENCRYPTION_ALGORITHM)); 
	auto derivedKey = QPasswordDigestor::deriveKeyPbkdf2(ENCRYPTION_ALGORITHM, PW, SALT, ENCRYPTION_ITERATIONS,keyLength );
	auto data = cipherValue.toUtf8();
	for (int i=0; i<data.size(); ++i)
	{
		data[i] = data[i] ^ derivedKey[i];
	}

	po_oJson.insert("value",  QJsonValue::fromVariant(data.toBase64()));
}

void medSourcesLoader::decipherJson(QJsonObject & po_oJson)
{
	QString cipheredValue = po_oJson.value("value").toString();
	
	int keyLength = ENCRYPTION_ITERATIONS * ceil(std::max(cipheredValue.length(), 512 / 8) / QCryptographicHash::hashLength(ENCRYPTION_ALGORITHM));

	auto derivedKey = QPasswordDigestor::deriveKeyPbkdf2(ENCRYPTION_ALGORITHM, PW, SALT, ENCRYPTION_ITERATIONS, keyLength);
	QByteArray data = QByteArray::fromBase64(cipheredValue.toUtf8());
	for (int i = 0; i < data.size(); ++i)
	{
		data[i] = data[i] ^ derivedKey[i];
	}
	QString readableString = QString::fromUtf8(data);
	po_oJson["value"] = readableString;
}

