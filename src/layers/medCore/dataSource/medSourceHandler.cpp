/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medSourceHandler.h"
//#include <medMetaDataKeys.h>


//#include <medNewLogger.h>
#include <medNotif.h>
//#include <medNotifSys.h>


medSourceHandler *medSourceHandler::s_instance = nullptr;
medSourceHandler *medSourceHandler::instance(QObject *parent)
{
    if (!s_instance)
    {
        s_instance = new medSourceHandler(parent);
    }
    return s_instance;
}


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor Destructor
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
medSourceHandler::medSourceHandler(QObject *parent)
{
    setParent(parent);
    m_defaultSource = nullptr;
}

medSourceHandler::~medSourceHandler()
{
}

medAbstractSource * medSourceHandler::getSource(QString const pi_sourceInstanceId)
{
    return m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
}


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Deal with Default Working Source
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void medSourceHandler::setDefaultWorkingSource(medAbstractSource * pi_pSource)
{
    m_defaultSource = pi_pSource;
}

medAbstractSource * medSourceHandler::getDefaultWorkingSource()
{
    return m_defaultSource;
}

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Deal with sources
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool medSourceHandler::sourceGlobalInfo(QString const & pi_sourceInstanceId, bool & po_bOnline, bool & po_bLocal, bool & po_bWritable, bool & po_bCache)
{
    bool bRes = true;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        po_bWritable = pSource->isWritable();
        po_bLocal    = pSource->isLocal();
        po_bCache    = pSource->isCached();
        po_bOnline   = pSource->isOnline();
    }
    else
    {
        bRes = false;
    }

    return bRes;
}

bool medSourceHandler::attributesForBuildTree(QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QString const & key, levelAttributes & po_entries)
{
    bool bRes = true;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        if (pSource->isFetchByMinimalEntriesOrMandatoryAttributes())
        {
            QStringList keys = pSource->getMandatoryAttributesKeys(pi_uiLevel);//TODO replace by fetch med Source Item Model column names
            auto listOfMinimalEntries = pSource->getMinimalEntries(pi_uiLevel, key);

            datasetAttributes entryTmp;
            for (auto &minimalEntry : listOfMinimalEntries)
            {
                entryTmp[keys[0]] = minimalEntry.key;
                entryTmp[keys[1]] = minimalEntry.name;

                entryTmp[keys[2]] = minimalEntry.description;
                po_entries.push_back(entryTmp);
            }
        }
        else
        {
            po_entries = pSource->getMandatoryAttributes(pi_uiLevel, key);
        }
    }
    else
    {
        bRes = false;
    }
    return bRes;
}

bool medSourceHandler::mandatoriesAttributes(QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QString const & parentKey, levelAttributes & po_entries)
{
    bool bRes = true;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        po_entries = pSource->getMandatoryAttributes(pi_uiLevel, parentKey);
    }
    else
    {
        bRes = false;
    }
    return bRes;
}

bool medSourceHandler::mandatoryAttributesKeys(QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QStringList & po_attributes)
{
    bool bRes = true;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        po_attributes = pSource->getMandatoryAttributesKeys(pi_uiLevel);
        bRes = !po_attributes.isEmpty();
    }
    else
    {
        bRes = false;
    }

    return bRes;
}

int medSourceHandler::push(medDataIndex const & pio_index)
{
    int iRqstIdRes = -1;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pio_index.sourceId());
    if (pSource)
    {
        iRqstIdRes = pSource->push(pio_index.level(), pio_index.dataId());        
    }

    return iRqstIdRes;
}

int medSourceHandler::getAsyncData(medDataIndex const & pi_index, QString dataName)
{
    int iRqstIdRes = -1;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_index.sourceId());
    if (pSource)
    {
        asyncRequest request;
        request.type = asyncRequestType::getRqstType;
        request.uri << pi_index;

        iRqstIdRes = pSource->getAssyncData(pi_index.level() - 1, pi_index.dataId());
        if (iRqstIdRes > 0)
        {
            addRequest(pi_index.sourceId(), iRqstIdRes, request);
            if (dataName.isEmpty())dataName = pi_index.dataId();
            auto notif = medNotif::createNotif(notifLevel::info, QString("Download ") + dataName, "Data is downloading from " + pi_index.sourceId());
            m_rqstToNotifMap[request] = notif;
        }
    }

    return iRqstIdRes;
}

bool medSourceHandler::getAsyncResults(QString const & pi_sourceInstanceId, int pi_request, QVariant & po_data)
{
    bool bRes = true;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        po_data = pSource->getAsyncResults(pi_request);
    }
    else
    {
        bRes = false;
    }

    return bRes;
}

//bool medSourceHandler::asyncResult(QString const & pi_sourceInstanceId, int pi_iRequest)
//{
//    //TODO
//    return false;
//}


bool medSourceHandler::getLevelDesiredWritable(QString const & pi_sourceInstanceId, int & po_iLevel)
{
    bool bRes = false;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        po_iLevel = pSource->getLevelDesiredWritable();
        bRes = true;
    }

    return bRes;
}

bool medSourceHandler::isLevelWritable(QString const & pi_sourceInstanceId, int pi_iLevel, bool & po_bWritable)
{
    bool bRes = false;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        po_bWritable = pSource->isLevelWritable(static_cast<unsigned int>(pi_iLevel));
        bRes = true;
    }

    return bRes;
}

bool medSourceHandler::levelCount(QString const & pi_sourceInstanceId, unsigned int & po_uiLevelMax)
{
    bool bRes = false;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        po_uiLevelMax = pSource->getLevelCount();
        bRes = true;
    }

    return bRes;
}


bool medSourceHandler::filteringParameters(const QString &pi_sourceInstanceId, QList<medAbstractParameter*> & po_parameters)
{
    bool bRes = false;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        po_parameters = pSource->getFilteringParameters();
        bRes = true;
    }

    return bRes;
}

bool medSourceHandler::getIOInterface(QString const & pi_sourceInstanceId, int & po_iInterface)
{
    bool bRes = false;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        po_iInterface = pSource->getIOInterface();
        bRes = true;
    }

    return bRes;
}

bool medSourceHandler::getTypeAndFormat(QString const & pi_sourceInstanceId, QMap<QString, QStringList>& po_TypeAndFormat)
{
    bool bRes = false;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        po_TypeAndFormat = pSource->getTypeAndFormat();
        bRes = true;
    }

    return bRes;
}

bool medSourceHandler::connect(QString const & pi_sourceInstanceId, bool pi_bOnline)
{
    bool bRes = false;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        bRes = pSource->connect(pi_bOnline);
    }

    return bRes;
}

bool medSourceHandler::optionalAttributes(QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QString const & key, datasetAttributes & po_attributes, datasetAttributes & po_tags)
{
    bool bRes = false;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        medAbstractSource::datasetAttributes4 tmp;
        bRes = pSource->getAdditionalAttributes(pi_uiLevel, key, tmp);
        if (bRes)
        {
            po_attributes = tmp.values;
            po_tags = tmp.tags;
        }
    }

    return bRes;
}

bool medSourceHandler::getDirectData(QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QString const & key, QVariant & po_data)
{
    bool bRes = false;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        po_data = pSource->getDirectData(pi_uiLevel, key);
        bRes = true;
    }

    return bRes;
}

int medSourceHandler::addAssyncData(medDataIndex const & pio_index, QVariant const & pi_data, medAbstractSource::levelMinimalEntries & pio_minimalEntries)
{
    int iRqstIdRes = -1;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pio_index.sourceId());
    if (pSource)
    {
        asyncRequest request;
        request.tmpId = pio_minimalEntries.key;
        request.type = asyncRequestType::addRqstType;
        request.uri << pio_index << pio_minimalEntries.key;

        iRqstIdRes = pSource->addAssyncData(pi_data, pio_minimalEntries, pio_index.level(), pio_index.dataId());
        if (iRqstIdRes > 0)
        {
            addRequest(pio_index.sourceId(), iRqstIdRes, request);
        }
    }

    return iRqstIdRes;
}

bool medSourceHandler::addDirectData(QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QString const & parentKey, QVariant const & pi_data, medAbstractSource::levelMinimalEntries & pio_minimalEntries)
{
    bool bRes = false;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        bRes = pSource->addDirectData(pi_data, pio_minimalEntries, pi_uiLevel, parentKey);
    }

    return bRes;
}

bool medSourceHandler::createFolder(QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QString const & parentKey, medAbstractSource::levelMinimalEntries & pio_minimalEntries, medAbstractSource::datasetAttributes4 const & pi_attributes)
{
    bool bRes = false;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        bRes = pSource->createFolder(pio_minimalEntries, pi_attributes, pi_uiLevel, parentKey);
    }

    return bRes;
}

bool medSourceHandler::commitData(QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QString const & parentKey, QVariant const & pi_data, medAbstractSource::levelMinimalEntries & pio_minimalEntries)
{
    bool bRes = false;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        bRes = pSource->commitData(pi_uiLevel, pio_minimalEntries, pi_uiLevel, parentKey);
    }

    return bRes;
}

bool medSourceHandler::abortRequest(QString const & pi_sourceInstanceId, int pi_iRequest)
{
    bool bRes = false;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        pSource->abort(pi_iRequest);
        bRes = true;
    }

    return bRes;
}


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// sources handling
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void medSourceHandler::addSource(medAbstractSource * pi_source)
{
    if (pi_source)
    {
        auto instanceId = pi_source->getInstanceId();
        m_sourceInstanceIdOrderedList.push_back(instanceId);
        m_sourceIdToInstanceMap[instanceId] = pi_source;

        //auto tester = new QAbstractItemModelTester(m_sourcesModelMap[pi_source], QAbstractItemModelTester::FailureReportingMode::Fatal, this);
        QObject::connect(pi_source, &medAbstractSource::progress, this, &medSourceHandler::progress);
        emit sourceAdded(instanceId);
    }
}


void medSourceHandler::removeSource(medAbstractSource * pi_source)
{
    if (pi_source)
    {
        QString instanceId = pi_source->getInstanceId();
        m_sourceIdToInstanceMap.remove(instanceId);
        emit sourceRemoved(instanceId);
    }
}

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Advanced Accessors
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QString medSourceHandler::getInstanceName(QString const & pi_sourceInstanceId)
{
    QString instanceNameRes;
    if (m_sourceIdToInstanceMap.contains(pi_sourceInstanceId))
    {
        instanceNameRes = m_sourceIdToInstanceMap[pi_sourceInstanceId]->getInstanceName();
    }
    return instanceNameRes;
}


medAbstractSource * medSourceHandler::getSourceToWrite(QString pi_sourceIdDst)
{
    medAbstractSource * pSource = nullptr;

    if (pi_sourceIdDst.isEmpty())
    {
        pSource = medSourceHandler::instance()->getDefaultWorkingSource();
        pi_sourceIdDst = pSource->getInstanceId();
    }
    else
    {
        pSource = medSourcesLoader::instance()->getSource(pi_sourceIdDst);
    }

    return pSource;
}


void medSourceHandler::sourceIsOnline(QString sourceIntanceId)
{
    if (m_sourceIdToInstanceMap.contains(sourceIntanceId))
    {
        auto pSource = m_sourceIdToInstanceMap[sourceIntanceId];
        bool bOnline = pSource->connect(true); //TODO remove when 256 will be applied
        //m_sourcesModelMap[pSource]->setOnline(bOnline); //TODO remove when 256 will be applied
        //pSource->connect(true);

        //TODO REDO
    }
}

bool medSourceHandler::addRequest(QString sourceIntanceId, int pi_request, asyncRequest & request)
{

    bool bRes = false;

    if (m_sourceIdToInstanceMap.contains(sourceIntanceId) && !m_requestsMap[sourceIntanceId].contains(pi_request))
    {
        m_requestsMap[sourceIntanceId][pi_request] = request;
        bRes = true;
    }

    return bRes;
}

bool medSourceHandler::getRequest(QString sourceIntanceId, int pi_request, asyncRequest & request)
{
    bool bRes = false;

    if (m_requestsMap.contains(sourceIntanceId) && m_requestsMap[sourceIntanceId].contains(pi_request))
    {
        request = m_requestsMap[sourceIntanceId][pi_request];
        bRes = true;
    }

    return bRes;
}

bool medSourceHandler::removeRequest(QString sourceIntanceId, int pi_request)
{
    bool bRes = false;

    if (m_requestsMap.contains(sourceIntanceId) && m_requestsMap[sourceIntanceId].contains(pi_request))
    {
        bRes = m_requestsMap[sourceIntanceId].remove(pi_request);
    }

    return bRes;
}

void medSourceHandler::progress(int pi_iRequest, medAbstractSource::eRequestStatus status)
{
    auto *pSource = static_cast<medAbstractSource*>(sender());
    //auto *pItemModel = m_sourcesModelMap.value(pSource);
    asyncRequest request;

    if (pSource && getRequest(pSource->getInstanceId(), pi_iRequest, request))
    {
        switch (request.type)
        {
            case asyncRequestType::getRqstType:  //getAssyncData
            {
                switch (status)
                {
                    case medAbstractSource::aborted:
                        break;
                    case medAbstractSource::cnxLost:
                        break;
                    case medAbstractSource::faild:
                    {
                        if (m_rqstToNotifMap.contains(request))
                        {
                            m_rqstToNotifMap[request]->update(notifLevel::error, 101);
                        }
                        break;
                    }
                    case medAbstractSource::finish:
                    {
                        if (m_rqstToNotifMap.contains(request))
                        {
                            m_rqstToNotifMap[request]->update(notifLevel::success, -1, "Download succeed");
                        }
                        break;
                    }
                    case medAbstractSource::pending:
                    {
                        if (m_rqstToNotifMap.contains(request))
                        {
                            m_rqstToNotifMap[request]->update(notifLevel::info, 101);
                        }
                        break;
                    }
                    default:
                        break;
                }

                emit getAsyncStatus(pSource->getInstanceId(), pi_iRequest, status);
                break;
            }
            case asyncRequestType::addRqstType: //addAssyncData
            {
                switch (status)
                {
                    case medAbstractSource::aborted:
                        break;
                    case medAbstractSource::cnxLost:
                        break;
                    case medAbstractSource::faild:
                        break;
                    case medAbstractSource::finish:
                    {
                        //QVariant data = pSource->getAsyncResults(pi_iRequest);
                        //if (data.canConvert<QString>())
                        //{
                        //    auto index = pItemModel->toIndex(request.uri);
                        //    if (index.isValid())
                        //    {
                        //        replaceTmpId(pItemModel, index, data, request);
                        //        //notif
                        //        //log
                        //    }
                        //    else
                        //    {
                        //        //error no longer present
                        //    }
                        //}
                        //else
                        //{
                        //    //error
                        //}
                        break;
                    }
                    case medAbstractSource::pending:
                        break;
                    default:
                        break;
                }
                break;
            }
            default:
            {
                qDebug() << "medDataModel::progress receives signal  with requestId " << QString::number(pi_iRequest) << " with unknown request type " << QString::number(request.type) << " .";
                break;
            }
        }
    }
    else
    {
        qDebug() << "medDataModel::progress receives signal from unknown source or non source object";
    }
}

medAbstractWritingPolicy * medSourceHandler::getSourceWPolicy(QString pi_sourceId)
{
    medAbstractWritingPolicy* writePolicyRes = nullptr;

    medAbstractSource *pSourceDst = getSourceToWrite(pi_sourceId);
    if (pSourceDst)
    {
        writePolicyRes = pSourceDst->getWritingPolicy();
    }

    return writePolicyRes;
}

//bool medSourceHandler::asyncRequest::operator<(asyncRequest const & other)
//{
//    return true;// this->uri < other.uri;
//}
//
//bool medSourceHandler::asyncRequest::operator>(asyncRequest const & other)
//{
//    return false; // this->uri > other.uri;
//}
//
//bool medSourceHandler::asyncRequest::operator==(asyncRequest const & other)
//{
//    return true;// type == other.type && tmpId == other.tmpId && uri == other.uri;
//}

//bool operator<(asyncRequest const & a, asyncRequest const & b)
//{
//    return false;
//}
