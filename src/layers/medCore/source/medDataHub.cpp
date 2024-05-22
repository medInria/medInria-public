/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataHub.h>
#include <medDataImporter.h>
#include <medDataExporter.h>
#include <medGlobalDefs.h>
#include <medMetaDataKeys.h>
#include <medSettingsManager.h>

#include <medNewLogger.h>
#include <medNotif.h>
#include <medNotifSys.h>

#include <medVirtualRepresentation.h>

// Pour le Hack
#include <medDataManager.h>
//TODO a vire des que possible

#include <QTemporaryDir>
#include <QModelIndex>

#define WAITER_EXIT_CODE_SUCCESS 0
#define WAITER_EXIT_CODE_ABORT -1
#define WAITER_EXIT_CODE_FAILD -2
#define WAITER_EXIT_CODE_CONN_LOST -3
#define WAITER_EXIT_CODE_TIMEOUT -4

medDataHub *medDataHub::s_instance = nullptr;
medDataHub *medDataHub::instance(QObject *parent)
{
    if (!s_instance)
    {
        s_instance = new medDataHub(parent);
    }
    return s_instance;
}

QString medDataHub::getTmpUuid()
{
    static QAtomicInteger count(100000);
    return QString("med_")+QString::number(QDateTime::currentSecsSinceEpoch()) + QString::number(++count);
}

QString medDataHub::convertToPath(QStringList pi_uri)
{
    QString pathRes;

    auto * pModel = getModel(pi_uri[0]);
    if (pModel != nullptr)
    {
        auto index = pModel->toIndex(pi_uri);
        pathRes = pModel->toPath(index);
    }

    return pathRes;
}


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor Destructor
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
medDataHub::medDataHub(QObject *parent)
{
    setParent(parent);
    m_sourcesHandler = medSourceHandler::instance();
    connect(m_sourcesHandler, &medSourceHandler::sourceAdded,   this, &medDataHub::addSource);
    connect(m_sourcesHandler, &medSourceHandler::sourceRemoved, this, &medDataHub::removeSource);
    connect(m_sourcesHandler, &medSourceHandler::getAsyncStatus, this, &medDataHub::progress);
    connect(m_sourcesHandler, &medSourceHandler::connectionDisconnection, this, &medDataHub::sourceOnlineStatus);    
    m_clock.setInterval(REQUEST_TIME_OUT_PULLING);
    connect(&m_clock, &QTimer::timeout, this, &medDataHub::timeOutWatcher);
    m_clock.start();
}

medDataHub::~medDataHub()
{
}


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



medAbstractData * medDataHub::variantToMedAbstractData(QVariant &data, const medDataIndex & index)
{
	medAbstractData *pDataRes = nullptr;

	if (data.canConvert<medAbstractData *>())
	{
		pDataRes = data.value<medAbstractData *>();
	}
	else if (data.canConvert<QString>())
	{
		pDataRes = medDataImporter::convertSingleDataOnfly(data.toString());
		if (pDataRes)
		{
			pDataRes->setDataIndex(index);
            medSourceModel * pModel = getModel(index.sourceId());
			QModelIndex modelIndex = pModel->toIndex(index);
			pModel->setData(modelIndex, DATASTATE_ROLE_DATALOADED, DATASTATE_ROLE); //Set information on tree about the data is already loaded
            pModel->setData(modelIndex, true, MEDDATA_ROLE);
            QModelIndex parentIndex = modelIndex.parent();
            while (parentIndex.isValid())
            {
                pModel->setData(parentIndex, true, MEDDATA_ROLE);
                parentIndex = parentIndex.parent();
            }

			QString hruUri = pModel->toPath(modelIndex);
			QString name = hruUri.right(hruUri.size() - hruUri.lastIndexOf("\r\n") - 2);
			QStringList hruUriAsList = hruUri.split("\r\n", QString::SkipEmptyParts);
			pDataRes->setExpectedName(name);
			pDataRes->setMetaData(medMetaDataKeys::key("SeriesDescription"), name);

			//Todo remove the next asap
			QString studyDesc;
			if (hruUriAsList.size() > 2)
			{
				studyDesc = hruUriAsList.at(hruUriAsList.size() - 2);
			}
			pDataRes->setMetaData(medMetaDataKeys::key("StudyDescription"), studyDesc);
			//end todo
		}
		else
		{
			qDebug() << "Unable to read data " << data.toString();
		}
	}
	else if (data.canConvert<QByteArray>())
	{
		qDebug() << "Not implemented yet";
	}

	if (pDataRes)
	{
		pDataRes->retain();
		pDataRes->setDataIndex(index);
		m_IndexToData[index] = pDataRes;
		m_IndexToData[index].data();
        getVirtualRepresentation()->addData(index, "", pDataRes);
		connect(pDataRes, &QObject::destroyed, this, &medDataHub::unloadData);
	}
    else
    {
        auto dataName = getDataName(index);
        medNotif::createNotif(notifLevel::warning, QString("Converting ") + dataName, " to load it failed");
    }

	return pDataRes;
}

QList<medSourceModel*> medDataHub::models()
{
    QList<medSourceModel*> sourceItemModelListRes;

    for (auto const & sourceId : medSourcesLoader::instance()->sourcesIdList())
    {
        sourceItemModelListRes.push_back(m_sourceIdToModelMap[sourceId]);
    }

    return sourceItemModelListRes;
}

medSourceModel * medDataHub::getModel(QString const & pi_sourceIntanceId)
{
    medSourceModel * res = nullptr;

    if (m_sourceIdToModelMap.contains(pi_sourceIntanceId))
    {
        res = m_sourceIdToModelMap[pi_sourceIntanceId];
    }

    return res;
}

void medDataHub::expandAll(const QString & sourceInstanceId)
{
    if (m_sourceIdToModelMap.contains(sourceInstanceId))
    {
        m_sourceIdToModelMap[sourceInstanceId]->expandAll();
    }
}

void medDataHub::sourceIsOnline(QString sourceIntanceId)
{
    if (m_sourceIdToModelMap.contains(sourceIntanceId))
    {
        bool bOnline = m_sourcesHandler->connect(sourceIntanceId, true); //TODO remove when 256 will be applied
        m_sourceIdToModelMap[sourceIntanceId]->setOnline(bOnline); //TODO remove when 256 will be applied
    }
}

void medDataHub::getDirectData(const medDataIndex & index, medAbstractData * &pDataRes)
{
    QVariant data;
    auto pModel = getModel(index.sourceId());
    pModel->setData(pModel->toIndex(index), DATASTATE_ROLE_DATALOADING, DATASTATE_ROLE);
    m_sourcesHandler->getDirectData(index.sourceId(), index.level() - 1, index.dataId(), data);
    pDataRes = variantToMedAbstractData(data, index);
    if(pDataRes)
        pModel->setData(pModel->toIndex(index), DATASTATE_ROLE_DATALOADED, DATASTATE_ROLE);
    else
        pModel->setData(pModel->toIndex(index), DATASTATE_ROLE_DATANOTLOADED, DATASTATE_ROLE);

}

void medDataHub::getAsyncData(const medDataIndex & index, medAbstractData * &pDataRes)
{
    bool bOk = true;
    asyncRequest rqst;
    rqst.dataName = getDataName(index);
    rqst.uri = index;
    rqst.type = getRqstType;

    int rqstId = m_sourcesHandler->getAsyncData(index);
    if (rqstId > -1)
    {
        QString sourceId = index.sourceId();
        auto pModel = getModel(sourceId);
        addRequest(sourceId, rqstId, rqst);
        pModel->setData(pModel->toIndex(index), DATASTATE_ROLE_DATALOADING, DATASTATE_ROLE);
        m_rqstToNotifMap[rqst] = medNotif::createNotif(notifLevel::info, QString("Download ") + rqst.dataName, "Data is downloading from " + index.sourceId());
        int iStatus = waitGetAsyncData(sourceId, rqstId);
        switch (iStatus)
        {
            case WAITER_EXIT_CODE_SUCCESS:
            {
                QVariant data;
                m_sourcesHandler->getAsyncResults(sourceId, rqstId, data);
                pDataRes = variantToMedAbstractData(data, index);
                break;
            }
            case WAITER_EXIT_CODE_ABORT:
                pModel->setData(pModel->toIndex(index), "", DATASTATE_ROLE);
                break;
            case WAITER_EXIT_CODE_CONN_LOST:
                break;
            case WAITER_EXIT_CODE_TIMEOUT:
                break;
        }
        removeRequest(sourceId, rqstId);
    }
}
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Datasets handling
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QString medDataHub::getDataName(medDataIndex const & index)
{
    QString nameRes;

    auto pModel = getModel(index.sourceId());
    if (pModel)
    {
        nameRes = pModel->getDataName(index);
    }

    return nameRes;
}

medAbstractData * medDataHub::getData(medDataIndex const & index)
{
    medAbstractData *pDataRes = nullptr;

    if (m_IndexToData.contains(index))
    {
        pDataRes = m_IndexToData[index];
    }
    else
    {
        bool bOnline, bWritable, bLocal, bCache;
        QString sourceId = index.sourceId();
        if (sourceId == "fs")
        {
            auto dataList = loadDataFromPathAsIndex(index);
            if (!dataList.isEmpty())
            {
                pDataRes = dataList[0];
            }
        }
        else if (m_sourcesHandler->sourceGlobalInfo(sourceId, bOnline, bLocal, bWritable, bCache))
        {
            if (bLocal)
            {
                getDirectData(index, pDataRes);
            }
            else
            {
                getAsyncData(index, pDataRes);
            }
        }
        else
        {
            medNotif::createNotif(notifLevel::error, "Data can't be retrieved",
                QString("Unable to retrieve data from source ") + sourceId + QString(" for data index ") + index.asString());
        }
    }

    return pDataRes;
}

QList<medAbstractData*> medDataHub::getDataList(medDataIndex const & index)
{
    QList<medAbstractData *> dataResList;

    if (m_IndexToData.contains(index))
    {
        dataResList << m_IndexToData[index];
    }
    else
    {
       
        QString sourceId = index.sourceId();
        if (sourceId == "fs")
        {
            dataResList << loadDataFromPathAsIndex(index);
        }
        else 
        {
            //TODO
            dataResList << getData(index);
        }
    }

    return dataResList;
}

int medDataHub::waitGetAsyncData(const QString &sourceId, int rqstId)
{
    int iRqstStatus = -1;

    if (m_requestsMap.contains(sourceId) && m_requestsMap[sourceId].contains(rqstId))
    {
        auto &rqst = m_requestsMap[sourceId][rqstId];
        iRqstStatus = rqst.waiter.exec();
    }

    return iRqstStatus;
}

void medDataHub::progress(const QString & sourceId, int rqstId, medAbstractSource::eRequestStatus status)
{
    m_mapsRequestMutex.lock();
    if (m_requestsMap.contains(sourceId) && m_requestsMap[sourceId].contains(rqstId))
    {
        auto &rqst = m_requestsMap[sourceId][rqstId];
        auto pModel = getModel(sourceId);
        switch (status)
        {
            case medAbstractSource::aborted:
            {
                m_rqstToNotifMap[rqst]->update(notifLevel::warning, -1, "Operation aborted by user");
                if (rqst.type == asyncRequestType::getRqstType)
                {
                    pModel->setData(pModel->toIndex(rqst.uri), QVariant(), DATASTATE_ROLE);
                }
                else
                {
                    pModel->setData(pModel->toIndex(rqst.uri), DATASTATE_ROLE_DATANOTSAVED, DATASTATE_ROLE);
                }
                rqst.waiter.exit(WAITER_EXIT_CODE_ABORT);
                break;
            }
            case medAbstractSource::cnxLost:
            {
                m_rqstToNotifMap[rqst]->update(notifLevel::warning, -1, "Operation aborted because connexion to " + m_sourcesHandler->getInstanceName(sourceId) + " is lost");
                if (rqst.type == asyncRequestType::getRqstType)
                {
                    pModel->setData(pModel->toIndex(rqst.uri), DATASTATE_ROLE_DATANOTLOADED, DATASTATE_ROLE);
                }
                else
                {
                    pModel->setData(pModel->toIndex(rqst.uri), DATASTATE_ROLE_DATANOTSAVED, DATASTATE_ROLE);
                }
                rqst.waiter.exit(WAITER_EXIT_CODE_CONN_LOST);
                break;
            }
            case medAbstractSource::faild:
            {
                m_rqstToNotifMap[rqst]->update(notifLevel::warning, -1, "Operation failed");
                if (rqst.type == asyncRequestType::getRqstType)
                {
                    pModel->setData(pModel->toIndex(rqst.uri), DATASTATE_ROLE_DATANOTLOADED, DATASTATE_ROLE);
                }
                else
                {
                    pModel->setData(pModel->toIndex(rqst.uri), DATASTATE_ROLE_DATANOTSAVED, DATASTATE_ROLE);
                }
                rqst.waiter.exit(WAITER_EXIT_CODE_FAILD);
                break;
            }
            case medAbstractSource::finish:
            {
                if (rqst.type == asyncRequestType::getRqstType)
                {
                    pModel->setData(pModel->toIndex(rqst.uri), DATASTATE_ROLE_DATALOADED, DATASTATE_ROLE);
                    m_rqstToNotifMap[rqst]->update(notifLevel::success, -1, "Download succeed");
                    if (rqst.needMedAbstractConversion)
                    {
                        QVariant data;
                        m_sourcesHandler->getAsyncResults(sourceId, rqstId, data);
                        variantToMedAbstractData(data, rqst.uri);
                    }
                }
                else
                {
                    QVariant data;
                    m_sourcesHandler->getAsyncResults(sourceId, rqstId, data);
                    if (data.canConvert<QString>())
                    {
                        auto key = data.toString();
                        pModel->setData(pModel->toIndex(rqst.uri), DATASTATE_ROLE_DATASAVED, DATASTATE_ROLE);
                        pModel->substituteTmpKey(rqst.uri, key);
                        m_rqstToNotifMap[rqst]->update(notifLevel::success, -1, "Save succeed");
                    }
                    else
                    {
                        //TODO ERROR
                    }
                }
                rqst.noLongerValid = true;
                rqst.waiter.exit(WAITER_EXIT_CODE_SUCCESS);
                break;
            }
            case medAbstractSource::pending:
            {
                if (m_rqstToNotifMap.contains(rqst))
                {
                m_rqstToNotifMap[rqst]->update(notifLevel::info, 101);
                rqst.stampTimeout = QDateTime::currentSecsSinceEpoch() + REQUEST_TIME_OUT;
                if (rqst.type == asyncRequestType::getRqstType)
                {
                    pModel->setData(pModel->toIndex(rqst.uri), DATASTATE_ROLE_DATALOADING, DATASTATE_ROLE);
                }
                else
                {
                    pModel->setData(pModel->toIndex(rqst.uri), DATASTATE_ROLE_DATAPUSHING, DATASTATE_ROLE);
                }
                }
                break;
            }
            default:
            {
                qDebug() << "Receive not expected status for source request " << rqstId << " from source id " << sourceId << " with status " << status;
                break;
            }
        }
    }
    else
    {
        qDebug() << "Receive not expected source request update " << sourceId << " " << rqstId << " " << status;
    }
    m_mapsRequestMutex.unlock();
}

void medDataHub::timeOutWatcher()
{
    m_mapsRequestMutex.lock();

    for (auto source : m_requestsMap.keys())
    {
        for (auto rqstId : m_requestsMap[source].keys())
        {
            auto & rqst = m_requestsMap[source][rqstId];
            if (!rqst.noLongerValid && rqst.stampTimeout < QDateTime::currentSecsSinceEpoch())
            {
                auto pModel = getModel(rqst.uri[0]);
                pModel->setData(pModel->toIndex(rqst.uri), rqst.type==getRqstType? DATASTATE_ROLE_DATANOTLOADED : DATASTATE_ROLE_DATANOTSAVED, DATASTATE_ROLE);
                if (m_rqstToNotifMap.contains(rqst))
                {
                    m_rqstToNotifMap[rqst]->update(notifLevel::warning, -1, "Failed due to timeout");
                }

                if (rqst.waiter.isRunning())
                {
                    rqst.waiter.exit(WAITER_EXIT_CODE_TIMEOUT);
                }
                else
                {
                    auto localRqst = m_requestsMap[source].take(rqstId);
                    m_rqstToNotifMap.remove(localRqst);
                }
            }
        }
    }

    m_mapsRequestMutex.unlock();
}

void medDataHub::sourceOnlineStatus(const QString & sourceId, bool status)
{
    auto *pModel = getModel(sourceId);

    if (pModel)
    {    
        if (status && !pModel->currentOnlineStatus())
        {
            pModel->refresh();
        }
        pModel->setOnline(status);
    }
}

QString fileSysPathToIndex(const QString &path, QStringList files)
{
    QString pathTmp = path;
    pathTmp.replace('\\', '/');
    pathTmp.replace('/', "\r\n");
    pathTmp = "fs:" + pathTmp;
    
    if (!files.isEmpty())
    {
        if (!pathTmp.endsWith("\r\n"))
        {
            pathTmp += "\r\n";
        }
        for (QString fileName : files)
        {
            pathTmp += fileName + "|";
        }
        if (pathTmp.endsWith("|"))
        {
            pathTmp = pathTmp.left(pathTmp.size() - 1);
        }
    }
    
    return pathTmp;
}

QStringList indexToFileSysPath(const QString &index)
{
    QStringList pathsRes;
    QString basePath;

    QString uri = index;
    if (uri.startsWith("fs:"))
    {
        int sourceDelimterIndex = uri.indexOf(QString(":"));

        QStringList uriAsList = uri.right(uri.size() - sourceDelimterIndex - 1).split(QString("\r\n"));
        QString lastElement = uriAsList.takeLast();
        QStringList files = lastElement.split('|',QString::SkipEmptyParts);
        basePath = uriAsList.join('/');
        for (QString fileName : files)
        {
            pathsRes << basePath + "/" + fileName;
        }
    }

    return pathsRes;
}

class medConvertLocalFileInThread : public QRunnable
{
public:
    medConvertLocalFileInThread(QString const path, QUuid uuid, medDataHub * hub)
    {
        m_path = path;
        m_uuid = uuid;
        m_pHub = hub;
    }

    void run() override
    {
        medDataIndex index = fileSysPathToIndex(m_path);
        m_pHub->loadDataFromPathAsIndex(index, m_uuid);
    }

    medDataHub * m_pHub;
    QString m_path;
    QUuid   m_uuid;
};

class medCopyLocalFileInThread : public QRunnable
{
public:
    medCopyLocalFileInThread(medDataIndex srcIndex, medDataIndex destinationIndex, medDataHub * hub)
    {
        m_index = srcIndex;
        m_destinationIndex = destinationIndex;
        m_pHub = hub;
    }

    void run() override
    {
        auto dstSourceId = m_destinationIndex.sourceId();
        medSourceModel *pModelDst = m_pHub->getModel(dstSourceId);
        medAbstractData * data = m_pHub->getData(m_index);
        if (data)
        {
            QString name = (m_index.sourceId()=="fs") ? 
                med::smartBaseName(indexToFileSysPath(m_index.asString())[0]) :
                m_pHub->getDataName(m_index);


            auto dstSourceId = m_destinationIndex.sourceId();
            medSourceModel *pModelDst = m_pHub->getModel(dstSourceId);
            auto indexDst = pModelDst->toIndex(m_destinationIndex);
            auto pathDst = pModelDst->toPath(indexDst);

            if (pathDst.isEmpty())
            {
                QString sugestedPath;
                medWritingPolicyData writingPolicyData;
                writingPolicyData.baseName = name;
                m_pHub->writeResults(dstSourceId, data, m_index, sugestedPath, writingPolicyData, nullptr);
            }
            else
            {
                QStringList destinationIndexAsLst = m_destinationIndex;
                m_pHub->saveData(data, name, destinationIndexAsLst);
            }
        }
        else
        {
            medNotif::createNotif(notifLevel::warning, QString("Converting file ") + m_index.asString(), " failed");
        }
    }

    medDataHub * m_pHub;
    medDataIndex m_index;
    medDataIndex m_destinationIndex;
};

void medDataHub::loadDataFromLocalFileSystem(QString const path, QUuid uuid)
{
    auto runner = new medConvertLocalFileInThread(path, uuid, this);
    runner->setAutoDelete(true);
    QThreadPool::globalInstance()->start(runner);
}

void medDataHub::addSource(QString const & pi_sourceId)
{
    m_sourceIdToModelMap[pi_sourceId] = new medSourceModel(this, pi_sourceId);
    emit sourceAdded(pi_sourceId);
}

void medDataHub::removeSource(QString const & pi_sourceId)
{
    delete (m_sourceIdToModelMap.take(pi_sourceId));
    emit sourceRemoved(pi_sourceId);
}

medSourceHandler::datasetAttributes medDataHub::getMetaData(medDataIndex const & index)
{
    medSourceHandler::datasetAttributes metaRes;

    auto *pModel = getModel(index.sourceId());
    if (pModel)
    {
        auto modelIndex = pModel->toIndex(index);
        auto x = pModel->getMendatoriesMetaData(modelIndex);
        for (auto & valueKey : x.values.keys())
        {
            metaRes.values[valueKey] = x.values[valueKey];
        }
        for (auto & tagKey : x.tags.keys())
        {
            metaRes.tags[tagKey] = x.tags[tagKey];
        }
    }

    return metaRes;
}

medSourceHandler::datasetAttributes medDataHub::getOptionalMetaData(medDataIndex const & index)
{
    medSourceHandler::datasetAttributes metaRes;

    auto *pModel = getModel(index.sourceId());
    if (pModel)
    {
        auto modelIndex = pModel->toIndex(index);
        medSourceHandler::datasetAttributes attributes;
        bool ok = pModel->getAdditionnalMetaData(modelIndex, attributes);
        if (ok)
        {
            if (attributes.values.isEmpty())
            {
                m_sourcesHandler->optionalAttributes(index.sourceId(),index.level()-1, index.dataId(), metaRes);
                pModel->setAdditionnalMetaData(modelIndex, metaRes);
            }
            else
            {
                metaRes = attributes;
            }
        }

    }   
    return metaRes;
}

bool medDataHub::getChildrenNames(medDataIndex index, QStringList &names)
{
    bool bRes = false;

    if (!index.isV2())
    {
        medSourceModel* pModel = getModel(index.sourceId());
        if (pModel)
        {
            bRes = pModel->getChildrenNames(index, names);
        }
    }

    return bRes;
}


bool medDataHub::writeResults(QString pi_sourceId, medAbstractData * pi_pData, QStringList pi_UriOfRelatedData, QString pi_sugestedPath, medWritingPolicyData & pi_writingPolicyData, medAbstractWritingPolicy * pi_pWritingPolicy)
{
    bool bRes = true;

    auto * pWp = pi_pWritingPolicy;

    if (pWp == nullptr)
    {
        pWp = m_sourcesHandler->getSourceWPolicy(pi_sourceId);

        if (pWp == nullptr)
        {
            pWp = m_sourcesHandler->getGeneralWPolicy();
        }
    }

    QMap<QString, QString> metaData;//pi_pData->metaData(); //TODO
    QString computedName = pWp->computeName(pi_writingPolicyData.baseName, pi_writingPolicyData.prefix, pi_writingPolicyData.suffix, metaData);

    unsigned int levelCount = 0;
    QString pathOfRelatedData;
    m_sourcesHandler->levelCount(pi_UriOfRelatedData[0], levelCount);
    if (levelCount != 0)
    {
        pathOfRelatedData = convertToPath(pi_UriOfRelatedData); // to path of pi_URIOfRelatedData
    }

    QString path;
    QStringList uri;
    QMap<int, QString> knownKeys;


    auto originPath = pathOfRelatedData.split("\r\n", QString::SkipEmptyParts);
    auto sugestedPath = pi_sugestedPath.split("\r\n", QString::SkipEmptyParts);

    QString suggestedSourceId;
    // ////////////////////////////////////////////////////////////////////////////////////////
    // If we write in the same source as origin, we already known the keys used to build the path, then we initialize knownKeys
    if (!pi_UriOfRelatedData.isEmpty())
    {
        suggestedSourceId = pi_UriOfRelatedData.takeFirst();
        if (pi_sourceId == suggestedSourceId)
        {
            for (int i = 0; i < pi_UriOfRelatedData.size(); ++i)
            {
                knownKeys[i] = pi_UriOfRelatedData[i];
            }
        }
    }
    // ////////////////////////////////////////////////////////////////////////////////////////


    // ////////////////////////////////////////////////////////////////////////////////////////
    // Check la coherence de la proposition par rapport a l'URI
    auto limite = std::min(originPath.size(), sugestedPath.size());
    for (int i = 0; i < limite; ++i)
    {
        bRes = bRes && (originPath[i] == sugestedPath[i]);
    }
    if (bRes)
    {
        if (originPath.size() > sugestedPath.size())
        {
            path = originPath.join("\r\n");
        }
        else
        {
            path = sugestedPath.join("\r\n");
        }
        // ////////////////////////////////////////////////////////////////////////////////////////

        // ////////////////////////////////////////////////////////////////////////////////////////
        // We compute path and call createPath
        //int levelToWrite = m_sourceIdToInstanceMap[suggestedSourceId]->getLevelDesiredWritable();
        int levelToWriteSuggestedSource = -1;
        int levelToWriteOriginSource = -1;
        m_sourcesHandler->getLevelDesiredWritable(suggestedSourceId, levelToWriteSuggestedSource);
        m_sourcesHandler->getLevelDesiredWritable(pi_sourceId, levelToWriteOriginSource);
        if (levelToWriteSuggestedSource == -1 && levelToWriteOriginSource != -1)
        {
            path = pWp->computePath(suggestedSourceId, path, metaData);
        }
        else
        {
            path = pWp->computePath(pi_sourceId, path, metaData);
        }
        bRes = createPath(pi_sourceId, path.split("\r\n", QString::SkipEmptyParts), uri, knownKeys);
        // ////////////////////////////////////////////////////////////////////////////////////////

        // ////////////////////////////////////////////////////////////////////////////////////////
        // Check if level is writable in destination source
        int levelToWriteData = uri.size() - 1; // We remove of the size the 1st element (sourceId) 

        // ////////////////////////////////////////////////////////////////////////////////////////
        // If bRes true then write
        bool bWritable;
        bRes = bRes && m_sourcesHandler->isLevelWritable(pi_sourceId, levelToWriteData, bWritable);
        if (bRes && bWritable)
        {
            QStringList names;
            getChildrenNames(uri, names);
            pWp->incrementName(computedName, names);
            if (saveData(pi_pData, computedName, uri))
            {
                pi_pData->setDataIndex(uri);
            }
            else
            {
                //TODO log
                //TODO notif 
            }
        }
        else
        {
            //TODO log
            //TODO notif 
        }
        // ////////////////////////////////////////////////////////////////////////////////////////
    }

    return bRes;
}



QUuid medDataHub::writeResultsHackV3(medAbstractData &data, bool originSrc)
{
    QString pi_sourceId;
    medAbstractData * pi_pData = &data;
    QStringList pi_UriOfRelatedData;
    QString pi_sugestedPath;
    medWritingPolicyData pi_writingPolicyData;

    if (!data.parentData().isEmpty() && data.parentData()[0]->dataIndex().isV2())
    {
        pi_UriOfRelatedData = data.parentData()[0]->dataIndex().uri();
        if (originSrc)
        {
            pi_sourceId = m_sourcesHandler->getSourceToWrite(pi_UriOfRelatedData[0])->getInstanceId();
        }
        else
        {
            auto defaultSource = m_sourcesHandler->getDefaultWorkingSource();
            if (defaultSource)
            {
                pi_sourceId = defaultSource->getInstanceId();
            }
            else
            {
                medNotif::createNotif(notifLevel::error, "No default source detected",
                                      "Unable to save data in default source if not defined.\n"
                                      "Please define default source in sources settings menu.");
                return QUuid();
            }
        }

        pi_writingPolicyData.baseName = data.fecthMetaData("SeriesDescription");

        writeResults(pi_sourceId, pi_pData, pi_UriOfRelatedData, pi_sugestedPath, pi_writingPolicyData, nullptr);
    }
    return QUuid();
}

bool medDataHub::saveData(medAbstractData *pi_pData, QString const &pi_baseName, QStringList &pio_uri)
{
    bool bRes = false;

    if (!pio_uri.isEmpty())
    {
        QString sourceId = pio_uri[0];
        medSourceModel* pModel = getModel(sourceId);

        bool bOnline, bWritable, bLocal, bCache;
        if (m_sourcesHandler->sourceGlobalInfo(sourceId, bOnline, bLocal, bWritable, bCache))    
        {            
            medAbstractSource::levelMinimalEntries minimalEntries;
            minimalEntries.name = pi_baseName;
            unsigned int uiLevel = pio_uri.size() - 1;
            QString parentKey = pio_uri.last();

            QVariant data = prepareDataToWrite(sourceId, pi_pData, pio_uri);
            
            if (data.isValid())
            {
                if (bLocal || bCache)
                {
                    bRes = bLocal? m_sourcesHandler->addDirectData(sourceId, uiLevel, parentKey, data, minimalEntries) : m_sourcesHandler->commitData(sourceId, uiLevel, parentKey, data, minimalEntries);
                    if (bRes)
                    {
                        pModel->fetch(pio_uri);
                        pio_uri.push_back(minimalEntries.key);
                        if (bCache)
                        {
                            pModel->setData(pModel->toIndex(pio_uri), DATASTATE_ROLE_DATACOMMITTED, DATASTATE_ROLE);
                            qDebug() << "Data " << pi_pData->name() << " committed on " << sourceId;
                        }
                        else
                        {
                            pModel->setData(pModel->toIndex(pio_uri), DATASTATE_ROLE_DATASAVED, DATASTATE_ROLE);
                            qDebug() << "Data " << pi_pData->name() << " saved on " << sourceId;
                        }
                    }
                    else
                    {
                        qDebug() << "Data " << pi_pData->name() << " NOT saved on " << sourceId;
                        pModel->setData(pModel->toIndex(pio_uri), DATASTATE_ROLE_DATANOTSAVED, DATASTATE_ROLE);
                        //TODO notif
                    }
                }
                else
                {
                    bRes = warpAddAsync(minimalEntries, pio_uri, pModel, uiLevel, parentKey, pi_pData, data);
                }
            }
            else
            {
                //TODO log
                //TODO notif
            }
        }

        auto index = pModel->toIndex(pio_uri);
        while (index.isValid())
        {
            m_sourceIdToModelMap[sourceId]->setData(index, true, MEDDATA_ROLE);
            index = index.parent();
        }
    }

    return bRes;
}

bool medDataHub::warpAddAsync(medAbstractSource::levelMinimalEntries &minimalEntries, QStringList & pio_uri, medSourceModel * pModel, unsigned int uiLevel, QString &parentKey, medAbstractData * pi_pData, QVariant &data)
{
    bool bRes = false;

    int rqstId = -1;

    asyncRequest request;

    minimalEntries.key = getTmpUuid();
    request.tmpId = minimalEntries.key;
    request.type = asyncRequestType::addRqstType;
    request.uri << pio_uri << minimalEntries.key;
    request.dataName = minimalEntries.name;
    bRes = pModel->addEntry(minimalEntries.key, minimalEntries.name, minimalEntries.description, uiLevel, parentKey);
    if (bRes)
    {
        QString sourceId = pModel->getSourceIntanceId();
        m_IndexToData[request.uri] = pi_pData;
        getVirtualRepresentation()->addGeneratedData(pi_pData, request.dataName);
        rqstId = m_sourcesHandler->addAssyncData(pio_uri, data, minimalEntries);
        if (rqstId > 0)
        {
            addRequest(sourceId, rqstId, request);
            pModel->setData(pModel->toIndex(request.uri), DATASTATE_ROLE_DATACOMMITTED, DATASTATE_ROLE);
            m_rqstToNotifMap[request] = medNotif::createNotif(notifLevel::info, "Save data " + request.dataName, "The data " + request.dataName + " is saving on " + m_sourcesHandler->getInstanceName(sourceId), -1, 0);
            //TODO log
        }
        else
        {
            //TODO log
            //TODO notify
        }
    }
    else
    {
        //TODO log
        //TODO notify
    }

    return bRes;
}

QVariant medDataHub::prepareDataToWrite(QString &sourceId, medAbstractData * &pi_pData, QStringList & pio_uri)
{
    QVariant dataRes;

    int iIOInterface = 0;
    QMap<QString, QStringList> typeAndFormat;
    m_sourcesHandler->getTypeAndFormat(sourceId, typeAndFormat);
    m_sourcesHandler->getIOInterface(sourceId, iIOInterface);

    QTemporaryDir tmpDir;
    tmpDir.setAutoRemove(false);
    if (iIOInterface == IO_FILE)
    {
        if (tmpDir.isValid())
        {
            bool bWritableData = false;
            QString fullTmpPath = tmpDir.path() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "");
            QString dataType = pi_pData->identifier();

            auto types = typeAndFormat.keys();
            QStringList exts;
            for (int i = 0; i < types.size(); ++i)
            {
                if (dataType.contains(types[i]))
                {
                    getModel(sourceId)->fetch(pio_uri);
                    exts = typeAndFormat[types[i]];
                    break;
                }
            }

            if (exts.isEmpty())
            {
                bWritableData = medDataExporter::convertSingleDataOnfly(pi_pData, fullTmpPath);
            }
            else
            {
                bWritableData = medDataExporter::convertSingleDataOnfly(pi_pData, fullTmpPath, exts);
            }
            if (bWritableData)
            {
                dataRes.setValue(fullTmpPath);
            }
        }
    }
    else if (iIOInterface == IO_MEDABSTRACTDATA)
    {
        dataRes.setValue(pi_pData);
    }
    else if (iIOInterface == IO_STREAM)
    {
        qDebug() << "Not Implemented yet. For now, data is an invalid QVariant";
    }

    return dataRes;
}


bool medDataHub::createPath(QString pi_sourceId, QStringList pi_folders, QStringList &po_uri, QMap<int, QString> pi_knownKeys)
{
    bool bRes = true;

    po_uri.clear();

    auto * pModel = getModel(pi_sourceId);
    if (pModel)
    {
        po_uri.push_front(pi_sourceId);

        for (unsigned int i = 0; (i < static_cast<unsigned int>(pi_folders.size()) && bRes); ++i)
        {
            if (pi_knownKeys.contains(i))
            {
                po_uri.push_back(pi_knownKeys[i]);
                bRes = pModel->toIndex(po_uri).isValid();
            }
            else
            {
                QString keyTmp = pModel->keyForPath(po_uri, pi_folders[i]);

                if (keyTmp.isEmpty())
                {
                    pModel->fetch(po_uri);
                    keyTmp = pModel->keyForPath(po_uri, pi_folders[i]);
                    bool bWritable = m_sourcesHandler->isLevelWritable(pi_sourceId, i, bWritable) && bWritable;
                    if (keyTmp.isEmpty() && bWritable)
                    {
                        medAbstractSource::levelMinimalEntries lme;
                        medAbstractSource::datasetAttributes dataSetAttributes;
                        lme.name = pi_folders[i];
                        bRes = m_sourcesHandler->createFolder(pi_sourceId, i, po_uri.last(), lme, dataSetAttributes);
                        if (bRes)
                        {
                            keyTmp = lme.key;
                        }
                    }
                    if (!keyTmp.isEmpty())
                    {
                        pModel->fetch(po_uri);
                        po_uri.push_back(keyTmp);
                    }
                    else
                    {
                        bRes = false;
                    }
                }
                else
                {
                    pModel->fetch(po_uri);
                    po_uri.push_back(keyTmp);
                }
            }
        }
    }
    else
    {
        qDebug() << "Can't create path " + pi_folders.join('/');
        medNotif::createNotif(notifLevel::warning, "Can't create path", "Can't create path " + pi_folders.join('/'));
    }

    return bRes;
}


void medDataHub::refresh(medDataIndex pi_index)
{
    auto *pModel = getModel(pi_index.sourceId());
    if (pModel)
    {
        pModel->refresh(pModel->toIndex(pi_index));
    }
}


void medDataHub::unloadData(QObject * obj)
{
    medAbstractData* data = static_cast<medAbstractData*>(obj);
    auto liste1 = m_IndexToData.values();
    QList<medAbstractData*> liste2;
    for (auto pData : liste1)
    {
        liste2.push_back(pData.data());
    }

    if (liste2.contains(data))
    {
        dtkSmartPointer<medAbstractData> sptr(data);
        
        m_IndexToData.key(sptr);
    }
    //TODO Continue with non medAbstractData
}


/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/
bool medDataHub::copyData(medDataIndex indexToCpy, medDataIndex destinationIndex)
{
    auto runner = new medCopyLocalFileInThread(indexToCpy, destinationIndex, this);
    runner->setAutoDelete(true);
    QThreadPool::globalInstance()->start(runner);

    return true;
}

bool medDataHub::fetchData(medDataIndex const & index)
{
    bool bRes = false;

    auto sourceId = index.sourceId();

    medSourceModel *pModel = getModel(sourceId);
    medSourceModelItem *pItem = pModel->getItem(index);
    if (pModel)
    {
        int iRequestId = m_sourcesHandler->getAsyncData(index);
        if (iRequestId > -1)
        {
            asyncRequest rqst;
            rqst.dataName = getDataName(index);
            rqst.uri = index;
            rqst.type = getRqstType;
            rqst.needMedAbstractConversion = true;

            addRequest(sourceId, iRequestId, rqst);

            pModel->setData(pModel->toIndex(index), DATASTATE_ROLE_DATALOADING, DATASTATE_ROLE);
            m_rqstToNotifMap[rqst] = medNotif::createNotif(notifLevel::info, "Fetch data " + rqst.dataName, "The data " + rqst.dataName + " is fetch from " + m_sourcesHandler->getInstanceName(sourceId), -1, 0);
            mInfo << "Fetch data " << rqst.dataName << "have the Id of notification \r\n" << iRequestId;
        }
        else
        {
            mInfo << "Fetch data " << getDataName(index) << " with the uri " << index << " is not possible";
            medNotif::createNotif(notifLevel::info, "Fetch data " + getDataName(index), "The data " + getDataName(index) + " can't be fetch from " + m_sourcesHandler->getInstanceName(sourceId), -1, 0);
        }
    }
    else
    {
        mInfo << "Fetch data with the uri " << index << " is impossible with unknown source";
        medNotif::createNotif(notifLevel::info, "Fetch data is not possible from unknown source", "Fetch data is not possible from unknown source");
    }

    return bRes;
}

bool medDataHub::pushData(medDataIndex const & index)
{
    bool bRes = false;

    auto sourceId = index.sourceId();
    medSourceModel *pModel = getModel(sourceId);
    medSourceModelItem *pItem = pModel->getItem(index);
    if (pModel)
    {
        int iRequestId = m_sourcesHandler->push(index);
        if (iRequestId > -1)
        {
            asyncRequest request;
            request.tmpId = pItem->iid();
            request.type = addRqstType;
            request.uri = pItem->uri();
            request.dataName = getDataName(index);

            addRequest(sourceId, iRequestId, request);
            pModel->setData(pModel->toIndex(index), DATASTATE_ROLE_DATAPUSHING, DATASTATE_ROLE);
            m_rqstToNotifMap[request] = medNotif::createNotif(notifLevel::info, "Save data " + request.dataName, "The data " + request.dataName + " is saving on " + m_sourcesHandler->getInstanceName(sourceId), -1, 0);
                

            //int  iNotif = medNotifSys::infoWithProgress( "Pushing data " + pItem->data(1).toString(), "The data " + pItem->data(1).toString() + " is pushing to " + m_sourcesHandler->getInstanceName(sourceId));
            //mInfo << "Pushing data " << pItem->data(1).toString() << "have the Id of notification " << iNotif << "\r\n";
            //TODO log
            //TODO notify
        }
        else
        {
            //TODO log
            //TODO notify
        }
    }
    else
    {
        //TODO log
        //TODO notify
    }

    return bRes;
}




void medDataHub::addRequest(QString sourceId, int requestId, asyncRequest & rqst)
{
    m_mapsRequestMutex.lock();
    m_requestsMap[sourceId][requestId] = rqst;
    m_mapsRequestMutex.unlock();
}

void medDataHub::removeRequest(QString sourceId, int rqstId)
{
    m_mapsRequestMutex.lock();
    if (m_requestsMap.contains(sourceId) && m_requestsMap[sourceId].contains(rqstId))
    {
        if (m_requestsMap[sourceId][rqstId].waiter.isRunning())
        {
            m_requestsMap[sourceId][rqstId].waiter.exit(WAITER_EXIT_CODE_ABORT);
        }
        m_rqstToNotifMap.remove(m_requestsMap[sourceId][rqstId]);
        m_requestsMap[sourceId].remove(rqstId);
        if (m_requestsMap[sourceId].isEmpty())
        {
            m_requestsMap.remove(sourceId);
        }
    }
    m_mapsRequestMutex.unlock();
}

void medDataHub::releaseRequest()
{
    m_mapsRequestMutex.unlock();
}

int findFirstDifference(const QString& str1, const QString& str2)
{
    // Iterate through the shorter of the two strings
    for (int i = 0; i < std::min(str1.size(), str2.size()); ++i)
    {
        if (str1[i] != str2[i])
        {
            return i;
        }
    }

    // If no difference is found within the shorter string's length
    // the longer string has extra characters at the end
    if (str1.size() != str2.size())
    {
        return std::min(str1.size(), str2.size());
    }

    // Strings are equal
    return std::min(str1.size(), str2.size());
}




QList<medAbstractData *> medDataHub::loadDataFromPathAsIndex(medDataIndex index, QUuid uuid)
{
    QList<medAbstractData *> dataResList;

    if (m_IndexToData.contains(index))
    {
        dataResList << m_IndexToData[index];
        emit dataLoaded(index);
        medDataManager::instance()->medDataHubRelay(index, uuid);
        return dataResList;
    }
    
    std::shared_ptr<medNotif> notif = medNotif::createNotif(notifLevel::info, QString("Load File ") + path, " from local file system", -1, -1);
    

    QStringList paths = indexToFileSysPath(index.asString());
    if (QFileInfo(paths[0]).exists())
        {
        std::shared_ptr<medNotif> notif = medNotif::createNotif(notifLevel::info, QString("Load File ") + paths[0], " from local file system", -1, -1);

        medDataImporter importer;
        dataResList << (paths.size()==1 ? importer.convertMultipData(paths[0]) : importer.convertMultipData(paths));

        if (!dataResList.isEmpty())
        {
            for (auto & data : dataResList)
            {
                auto fileList = importer.getPaths(data);
                QStringList relFileList;

                QString commonPath = computeRootPathOfListPath(fileList, relFileList);
                auto index = fileSysPathToIndex(commonPath, relFileList);

                data->setDataIndex(index);
                m_IndexToData[index] = data;
                m_IndexToData[index].data();

                getVirtualRepresentation()->addData(index, importer.getVolumeId(data), data);
                emit dataLoaded(index);

                if (dataResList.size() > 1) medNotif::createNotif(notifLevel::success, importer.getVolumeId(data) + " is loaded", " from local file system", -1);

                medDataManager::instance()->medDataHubRelay(index, uuid);
            }
                notif->update(notifLevel::success, -1, QString("Success"));
            }
            else
            {
                notif->update(notifLevel::warning, -2, QString("Failure"));
            }
        }
    else
        {
        medNotif::createNotif(notifLevel::warning, QString("Load unexist file ") + paths[0], " is impossible.", -2);
    }

    return dataResList;
}


QString computeRootPathOfListPath(QStringList &fileList, QStringList &relativePathList)
{
    QString rootPath = fileList[0];

    int x = 0;
    for (int i = 1; i < fileList.size(); i++)
    {
        x = findFirstDifference(rootPath, fileList[i]);
        rootPath = rootPath.left(x);
            }

    if (rootPath[rootPath.size() - 1] != '/')
    {
        x = rootPath.lastIndexOf('/') + 1;
        rootPath = rootPath.left(x);
        }

    for (auto aFilePath : fileList)
    {
        relativePathList << aFilePath.right(aFilePath.size() - x);
    }

    return  rootPath;
}

QList< medDataIndex > medDataHub::getSubData(medDataIndex const & index)
{
    QList< medDataIndex > listRes;
    QString sourceId = index.sourceId();

    if (sourceId == "fs")
    {
        QStringList dirPaths;
        QStringList paths = indexToFileSysPath(index.asString());
        for (QString path : paths)
        {
            QFileInfo fi(path);
            if (fi.isDir())
            {
                dirPaths << path;
            }
        }

        QMap<QString, QString> volumePathsMap;
        for (QString dirPath : dirPaths)
        {
            QFileInfo fi(dirPath);
            if (fi.isDir())
            {
                QDir dir(dirPath);
                QStringList subPathsFiles = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
                QStringList subPathsDirs  = dir.entryList(QDir::Dirs  | QDir::NoDotAndDotDot);
                for (int i = 0; i < subPathsFiles.size(); ++i) { subPathsFiles[i] = dirPath + '/' + subPathsFiles[i]; }
                for (int i = 0; i < subPathsDirs.size(); ++i) { subPathsDirs[i] = dirPath + '/' + subPathsDirs[i]; }
                detectVolume(subPathsFiles, volumePathsMap);
                for (QString const & index : volumePathsMap)
                {
                    listRes.append(index);
                }
                for (QString const & subPathsDir : subPathsDirs)
                {
                    listRes.append(fileSysPathToIndex(subPathsDir));
                }
            }
        }
    }
    else
    {
        medSourceModel * pModel = getModel(sourceId);
    if (pModel)
    {
        QModelIndex modelIndexParent = pModel->toIndex(index);
        for (int i = 0; i < pModel->rowCount(modelIndexParent); ++i)
        {
            auto sonIndex = pModel->index(i, 0, modelIndexParent);
            listRes << pModel->dataIndexFromModelIndex(sonIndex);
        }        
    }
    }

    return listRes;
}

int  medDataHub::getDataType(medDataIndex const & index)
{
    int iRes = -1;

    QString sourceId = index.sourceId();
    if (sourceId == "fs")
    {
        QModelIndex modelIndex = m_virtualRepresentation->getModelIndex(index);
        if (modelIndex.isValid()) // is into virtual representation
        {
        iRes = m_virtualRepresentation->data(modelIndex, DATATYPE_ROLE).toInt();
    }
        else // is not into the virtual representation, supposed is directly in file system
        {
            getDataTypeFS(index, iRes);
        }
    }
    else
    {
        medSourceModel * pModel = getModel(sourceId);
        if (pModel)
        {
            QModelIndex modelIndex = pModel->toIndex(index);
            if (modelIndex.isValid())
            {
                iRes = pModel->getDataType(modelIndex);
            }
            else
            {
                if (pModel->recursiveFetch(index))
                {
                    QModelIndex modelIndex = pModel->toIndex(index);
                    if (modelIndex.isValid())
                    {
            iRes = pModel->getDataType(modelIndex);
        }
    }
            }
        }
    }
    return iRes;
}

void medDataHub::getDataTypeFS(const medDataIndex & index, int &iRes)
{
    auto paths = indexToFileSysPath(index.asString());

    if (paths.size() == 1)
    {
        QFileInfo fi(paths[0]);
        if (fi.exists())
        {
            if (fi.isFile())
            {
                iRes = DATATYPE_ROLE_DATASET;
            }
            if (fi.isDir())
            {
                QDir dir(paths[0]);
                int fileCount = dir.entryList(QDir::Files).size();
                int dirCount  = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot).size();
                if (fileCount >  0 && dirCount == 0) iRes = DATATYPE_ROLE_DATASET;
                if (fileCount == 0 && dirCount >  0) iRes = DATATYPE_ROLE_FOLDER;
                if (fileCount >  0 && dirCount >  0) iRes = DATATYPE_ROLE_BOTH;
            }
        }
    }
    else
    {
        bool isFolder = false;
        bool isDataset = false;
        int  i = 0;

        while (i < paths.size() && (!isFolder || !isDataset))
        {
            auto & path = paths[i];
            QFileInfo fi(paths[i]);
            if (fi.exists())
            {
                isDataset = isDataset || fi.isFile();
                isFolder = isFolder || fi.isDir();
            }
            ++i;
        }

        if (!isFolder &&  isDataset) iRes = DATATYPE_ROLE_DATASET;
        if ( isFolder && !isDataset) iRes = DATATYPE_ROLE_FOLDER;
        if ( isFolder &&  isDataset) iRes = DATATYPE_ROLE_BOTH;
    }
}

void detectVolume(QStringList paths, QMap<QString, QString> & volumePathsMap)
{
    medDataImporter importer;

    auto dataList = importer.convertMultipData(paths);
    for (auto & data : dataList)
    {
        auto volumeId = importer.getVolumeId(data);
        auto pathsList = importer.getPaths(data);

        QStringList relFileList;
        QString commonPath = computeRootPathOfListPath(pathsList, relFileList);
        auto index = fileSysPathToIndex(commonPath, relFileList);

        volumePathsMap[volumeId] = index;
    }
}