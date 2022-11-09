/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataModel.h>
#include <medDataImporter.h>
#include <medDataExporter.h>
#include <medMetaDataKeys.h>
#include <medSettingsManager.h>


#include <medNewLogger.h>
#include <medNotif.h>
#include <medNotifSys.h>

#include <QTemporaryDir>
#include <QModelIndex>


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
            medSourceItemModel * pModel = getModel(index.sourceId());
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
			pDataRes->setMetaData(medMetaDataKeys::SeriesDescription.key(), name);

			//Todo remove the next asap
			QString studyDesc;
			if (hruUriAsList.size() > 2)
			{
				studyDesc = hruUriAsList.at(hruUriAsList.size() - 2);
			}
			pDataRes->setMetaData(medMetaDataKeys::StudyDescription.key(), studyDesc);
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
		connect(pDataRes, &QObject::destroyed, this, &medDataHub::unloadData);
	}

	return pDataRes;
}

QList<medSourceItemModel*> medDataHub::models()
{
    QList<medSourceItemModel*> sourceItemModelListRes;

    for (auto const & sourceId : medSourcesLoader::instance()->sourcesIdList())
    {
        sourceItemModelListRes.push_back(m_sourceIdToModelMap[sourceId]);
    }

    return sourceItemModelListRes;
}

medSourceItemModel * medDataHub::getModel(QString const & pi_sourceIntanceId)
{
    medSourceItemModel * res = nullptr;

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
    m_sourcesHandler->getDirectData(index.sourceId(), index.level() - 1, index.dataId(), data);
    pDataRes = variantToMedAbstractData(data, index);
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
        if (m_sourcesHandler->sourceGlobalInfo(sourceId, bOnline, bLocal, bWritable, bCache))
        {
            if (bLocal)
            {
                getDirectData(index, pDataRes);
            }
            else
            {
                bool bOk = true;                
                int rqstId = m_sourcesHandler->getAsyncData(index, getDataName(index));
                auto pModel = getModel(sourceId);
                pModel->setData(pModel->toIndex(index), DATASTATE_ROLE_DATALOADING, DATASTATE_ROLE);
                int iStatus = waitGetAsyncData(sourceId, rqstId);
                if (iStatus == 0)
                {
                    QVariant data;
                    m_sourcesHandler->getAsyncResults(sourceId, rqstId, data);
                    pDataRes = variantToMedAbstractData(data, index);
                    m_sourcesHandler->removeRequest(sourceId, rqstId);
                }
            }
        }
    }

    return pDataRes;
}

int medDataHub::waitGetAsyncData(const QString &sourceId, int rqstId)
{
    int iRqstStatus = -1;
    
    if (rqstId > 0)
    {
        QTimer timerTimeOut;
        timerTimeOut.setSingleShot(true);
        QEventLoop loop;
        auto timerConnexion = connect(&timerTimeOut, &QTimer::timeout, &loop, [&]() { loop.exit(-12); });
        timerTimeOut.start(100000);

        auto asyncConnexion = connect(m_sourcesHandler, &medSourceHandler::getAsyncStatus, [&](QString p_sourceId, int p_rqstId, medAbstractSource::eRequestStatus status)
        {
            if (p_sourceId == sourceId && p_rqstId == rqstId)
            {
                switch (status)
                {
                    case medAbstractSource::aborted:
                    case medAbstractSource::cnxLost:
                    case medAbstractSource::faild:
                    {
                        loop.exit(-1);
                        break;
                    }
                    case medAbstractSource::finish:
                    {
                        loop.quit();
                        break;
                    }
                    case medAbstractSource::pending:
                    {
                        timerTimeOut.setInterval(100000);
                        break;
                    }
                    default:
                        break;
                }
            }
        });

        iRqstStatus = loop.exec();
    }

    return iRqstStatus;
}

void medDataHub::progress(const QString & sourceId, int rqstId)
{
    //TODO
}

void medDataHub::addSource(QString const & pi_sourceId)
{
    m_sourceIdToModelMap[pi_sourceId] = new medSourceItemModel(this, pi_sourceId);
    emit sourceAdded(pi_sourceId);
}

void medDataHub::removeSource(QString const & pi_sourceId)
{
    delete (m_sourceIdToModelMap.take(pi_sourceId));
    emit sourceRemoved(pi_sourceId);
}

medDataHub::datasetAttributes medDataHub::getMetaData(medDataIndex const & index)
{
    datasetAttributes metaRes;

    auto *pModel = getModel(index.sourceId());
    if (pModel)
    {
        auto modelIndex = pModel->toIndex(index);
        metaRes = pModel->getMendatoriesMetaData(modelIndex);
    }

    return metaRes;
}

bool medDataHub::getChildrenNames(medDataIndex index, QStringList &names)
{
    bool bRes = false;

    if (!index.isV2())
    {
        medSourceItemModel* pModel = getModel(index.sourceId());
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
            pi_sourceId = m_sourcesHandler->getDefaultWorkingSource()->getInstanceId();
        }

        pi_writingPolicyData.baseName = data.metadata(medMetaDataKeys::SeriesDescription.key());

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
        medSourceItemModel* pModel = getModel(sourceId);

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
                        if (bCache) pModel->setData(pModel->toIndex(pio_uri), DATASTATE_ROLE_DATACOMMITED, DATASTATE_ROLE);
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
                    bRes = warpAddAsync(minimalEntries, pio_uri, pModel, uiLevel, parentKey, pi_pData, data);
                }
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

bool medDataHub::warpAddAsync(medAbstractSource::levelMinimalEntries &minimalEntries, QStringList & pio_uri, medSourceItemModel * pModel, unsigned int uiLevel, QString &parentKey, medAbstractData * pi_pData, QVariant &data)
{
    bool bRes = false;

    int rqstId = -1;

    medSourceItemModel::asyncRequest request;

    minimalEntries.key = getTmpUuid();
    request.tmpId = minimalEntries.key;
    request.type = medSourceItemModel::asyncRequestType::addRqstType;
    request.uri << pio_uri << minimalEntries.key;

    bRes = pModel->addEntry(minimalEntries.key, minimalEntries.name, minimalEntries.description, uiLevel, parentKey);
    if (bRes)
    {
        m_IndexToData[request.uri] = pi_pData;
        rqstId = m_sourcesHandler->addAssyncData(pio_uri, data, minimalEntries);
        pModel->setData(pModel->toIndex(request.uri), DATASTATE_ROLE_DATACOMMITED, DATASTATE_ROLE);
        if (rqstId > 0)
        {
            //m_IndexToData[pio_uri] = pi_pData;
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
                        medAbstractSource::datasetAttributes4 dataSetAttributes;
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


bool medDataHub::fetchData(medDataIndex const & index)
{
    bool bRes = false;

    auto sourceId = index.sourceId();
    //medAbstractSource *pSource = getSource(sourceId);
    medSourceItemModel *pModel = getModel(sourceId);
    medDataModelItem *pItem = pModel->getItem(index);
    if (pModel)
    {
        int iRequestId = m_sourcesHandler->getAsyncData(index, pItem->data(1).toString());
        if (iRequestId > -1)
        {
            pModel->setData(pModel->toIndex(index), DATASTATE_ROLE_DATALOADING, DATASTATE_ROLE);

            //int  iNotif = medNotifSys::infoWithProgress("Fetch data " + pItem->data(1).toString(), "The data " + pItem->data(1).toString() + " is fetch from " + pSource->getInstanceName());
            auto toto = medNotif::createNotif(notifLevel::info, "Fetch data " + pItem->data(1).toString(), "The data " + pItem->data(1).toString() + " is fetch from " + m_sourcesHandler->getInstanceName(sourceId), -1, 0);
            mInfo << "Fetch data " << pItem->data(1).toString() << "have the Id of notification \r\n";
            toto->setExtraTrigger1Label("Cancel");

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

bool medDataHub::pushData(medDataIndex const & index)
{
    bool bRes = false;

    auto sourceId = index.sourceId();
    medSourceItemModel *pModel = getModel(sourceId);
    medDataModelItem *pItem = pModel->getItem(index);
    if (pModel)
    {
        int iRequestId = m_sourcesHandler->push(index);
        if (iRequestId > -1)
        {
            medSourceItemModel::asyncRequest request;
            request.tmpId = pItem->iid();
            request.type = medSourceItemModel::asyncRequestType::addRqstType;
            request.uri = pItem->uri();

            // TODO bRes = pModel->addRequest(iRequestId, request);
            if (bRes)
            {
                pModel->setData(pModel->toIndex(index), DATASTATE_ROLE_DATAPUSHING, DATASTATE_ROLE);

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
    }
    else
    {
        //TODO log
        //TODO notify
    }

    return bRes;
}


