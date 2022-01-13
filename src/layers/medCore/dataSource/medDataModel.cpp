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

#include <QModelIndex>
#include <medDataModelItem.h>

medDataModel::medDataModel(QObject *parent)
{
    m_defaultSource = nullptr;
}

medDataModel::~medDataModel()
{
}

bool medDataModel::setDefaultWorkingSource(unsigned int i)
{
    bool bRes = i < static_cast<unsigned int>(m_sourcesModelMap.size());

    if (bRes)
    {
        auto source = m_sourcesModelMap.keys()[i];
        bool bWritable = source->isWriteable();
        bool bLocal = source->isLocal();
        bool bCached = source->isCached();
        bool bOnline = source->isOnline();

        if (bWritable && (bLocal || (!bLocal && bCached && bOnline)))
        {
            m_defaultSource = source;
        }
        else
        {
            bRes = false;
        }
    }
    return bRes;
}

bool medDataModel::getSourceGlobalInfo(QString const & pi_sourceIntanceId, bool & pi_bOnline, bool & pi_bLocal, bool & pi_bWritable, bool & pi_bCache)
{
    bool bRes = true;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceIntanceId);
    if (pSource)
    {
        pi_bWritable = pSource->isWriteable();
        pi_bLocal    = pSource->isLocal();
        pi_bCache    = pSource->isCached();
        pi_bOnline   = pSource->isOnline();
    }
    else
    {
        bRes = false;
    }

    return bRes;
}

bool medDataModel::getLevelMetaData(QString const & pi_sourceIntanceId, unsigned int pi_uiLevel, QString const & key, levelAttributes & po_entries)
{
    bool bRes = true;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceIntanceId);
    if (pSource)
    {
        QStringList keys = pSource->getMandatoryAttributesKeys(pi_uiLevel);//TODO replace by fetch medSourceItemModel column names
        auto listOfMinimalEntries = pSource->getMinimalEntries(pi_uiLevel, key);

        datasetAttributes entryTmp;
        for (auto &minimalEntry : listOfMinimalEntries)
        {
            entryTmp[keys[0]] = minimalEntry.key;
            entryTmp[keys[1]] = minimalEntry.name;
            entryTmp[keys[2]] = minimalEntry.description;
            po_entries.push_back(entryTmp);
        }

        int i = 0;
        auto listOfMandatoriesEntries = pSource->getMandatoryAttributes(pi_uiLevel, key);
        for (auto &mandatoriesEntries : listOfMandatoriesEntries)
        {
            for (auto & k : mandatoriesEntries.keys())
            {
                datasetAttributes &map = po_entries[i];
                QString val = mandatoriesEntries[k];
                map[k] = val;
            }
            ++i;
        }
    }
    else
    {
        bRes = false;
    }
    return bRes;
}

bool medDataModel::getMandatoryAttributesKeys(QString const & pi_sourceIntanceId, unsigned int pi_uiLevel, QStringList & po_attributes)
{
    bool bRes = true;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceIntanceId);
    if (pSource)
    {
        po_attributes = pSource->getMandatoryAttributesKeys(pi_uiLevel);
    }
    else
    {
        bRes = false;
    }

    return bRes;
}

bool medDataModel::getLevelCount(QString const & pi_sourceIntanceId, unsigned int & po_uiLevelMax)
{
    bool bRes = true;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceIntanceId);
    if (pSource)
    {
        po_uiLevelMax = pSource->getLevelCount();
    }
    else
    {
        bRes = false;
    }

    return bRes;
}

bool medDataModel::getOptionalAttributes(QString const & pi_sourceIntanceId, unsigned int pi_uiLevel, QString const & key, QList<QMap<int, QString>>& po_attributes)
{
    bool bRes = false;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceIntanceId);
    if (pSource)
    {
        //po_attributes = pSource->getAdditionalAttributes(pi_uiLevel, key);
    }
    else
    {
        bRes = false;
    }

    return bRes;
}

QString medDataModel::getInstanceName(QString const & pi_sourceIntanceId)
{
    QString instanceNameRes;
    if (m_sourceIdToInstanceMap.contains(pi_sourceIntanceId))
    {
        instanceNameRes = m_sourceIdToInstanceMap[pi_sourceIntanceId]->getInstanceName();
    }
    return instanceNameRes;
}

QList<medSourceItemModel*> medDataModel::models()
{
    return m_sourcesModelMap.values();
}

medSourceItemModel * medDataModel::getModel(QString const & pi_sourceIntanceId)
{
    medSourceItemModel * res = nullptr;
    if (m_sourceIdToInstanceMap.contains(pi_sourceIntanceId))
    {
        medAbstractSource * source = m_sourceIdToInstanceMap[pi_sourceIntanceId];
        if (m_sourcesModelMap.contains(source))
        {
            res = m_sourcesModelMap[source];
        }
    }
    return res;
}

medAbstractData * medDataModel::getData(medDataIndex const & index)
{
    medAbstractData *pDataRes = nullptr;

    if (m_IndexToData.contains(index))
    {
        pDataRes = m_IndexToData[index];
    }
    else
    {
        QString source = index.uri()[0];
        if (m_sourceIdToInstanceMap.contains(source))
        {
            QString pathTmp = m_sourceIdToInstanceMap[source]->getDirectData(index.uri().size()-2, index.uri()[index.uri().size()-1]);
            medAbstractData *pDataTmp = medDataImporter::convertSingleDataOnfly(pathTmp);
            if (pDataTmp)
            {
                pDataTmp->retain();
                pDataTmp->setDataIndex(index);
                m_IndexToData[index] = pDataTmp;
                m_IndexToData[index].data();
                pDataRes = pDataTmp;
                connect(pDataTmp, &QObject::destroyed, this, &medDataModel::removeConvertedData);
            }
        }
    }

    return pDataRes;
}

medDataModel::datasetAttributes medDataModel::getMetaData(QModelIndex const & index)
{
    datasetAttributes metaRes;

    if (index.isValid())
    {
        QAbstractItemModel *modelTmp = const_cast<QAbstractItemModel *> (index.model());
        medSourceItemModel *model = dynamic_cast<medSourceItemModel *> (modelTmp);
        if (model)
        {
            metaRes = model->getMetaData(index);
        }
    }
    else
    {

    }

    return metaRes;
}


void medDataModel::addSource(medAbstractSource * pi_source)
{
    if (pi_source)
    {
        auto instanceId = pi_source->getInstanceId();
        m_sourceIdToInstanceMap[instanceId] = pi_source;
        m_sourcesModelMap[pi_source] = new medSourceItemModel(this, instanceId);
        auto tester = new QAbstractItemModelTester(m_sourcesModelMap[pi_source], QAbstractItemModelTester::FailureReportingMode::Fatal, this);
    }
}


void medDataModel::removeSource(medAbstractSource * pi_source)
{
    if (pi_source)
    {
        QString instanceId = pi_source->getInstanceId();
        m_sourceIdToInstanceMap.remove(instanceId);
        delete m_sourcesModelMap.take(pi_source);
        emit sourceRemoved(instanceId);
    }
}

void medDataModel::addData(medAbstractData * pi_dataset, QString uri)
{
    QStringList splittedUri;
    int sourceDelimPos = uri.indexOf(':');
    splittedUri.append(uri.left(sourceDelimPos));
    QString sourceUri = uri.right(uri.size() - sourceDelimPos - 1);
    splittedUri.append(sourceUri.split('/'));
 
    //TODO verifier la presence dans la map
    auto pSource = m_sourceIdToInstanceMap[splittedUri[0]];
    // ////////////////////////////////////////////////////////////////////////
    // Adding dataset to the source
    bool bContinue = pSource->addData(pi_dataset, sourceUri);

    // ////////////////////////////////////////////////////////////////////////
    // Refresh dataModelElement
    if (bContinue)
    {
        m_sourcesModelMap[pSource]->fetch(sourceUri.left(sourceUri.lastIndexOf('/')));
    }
    else
    {
        sourceIsOnline(splittedUri[0]);
    }
}

void medDataModel::addData(medDataIndex * pi_datasetIndex, QString uri)
{
    //addData(getAbstractDataFromIndex(pi_datasetIndex), uri);
}

void medDataModel::refresh(QString uri)
{
    QStringList splittedUri;
    int sourceDelimPos = uri.indexOf(':');
    splittedUri.append(uri.left(sourceDelimPos));
    QString sourceUri = uri.right(uri.size() - sourceDelimPos - 1);
    splittedUri.append(sourceUri.split('/'));

    auto pSource = m_sourceIdToInstanceMap[splittedUri[0]];
    m_sourcesModelMap[pSource]->fetch(sourceUri);
}

void medDataModel::sourceIsOnline(QString sourceIntanceId)
{
    if (m_sourceIdToInstanceMap.contains(sourceIntanceId))
    {
        auto pSource = m_sourceIdToInstanceMap[sourceIntanceId];
        bool bOnline = pSource->connect(true); //TODO remove when 256 will be applied
        m_sourcesModelMap[pSource]->setOnline(bOnline); //TODO remove when 256 will be applied
        //pSource->connect(true);
    }
}

void medDataModel::removeConvertedData(QObject * obj)
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
}
