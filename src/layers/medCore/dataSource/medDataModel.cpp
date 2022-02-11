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

#include <QTemporaryDir>
#include <QModelIndex>

medDataModel *medDataModel::s_instance = nullptr;
medDataModel *medDataModel::instance(QObject *parent)
{
    if (!s_instance)
    {
        s_instance = new medDataModel(parent);
    }
    return s_instance;
}

medDataModel::medDataModel(QObject *parent)
{
    setParent(parent);
    m_defaultSource = nullptr;
}

medDataModel::~medDataModel()
{
    int i = 0;
    ++i;
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

bool medDataModel::sourceGlobalInfo(QString const & pi_sourceIntanceId, bool & pi_bOnline, bool & pi_bLocal, bool & pi_bWritable, bool & pi_bCache)
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

bool medDataModel::attributesForBuildTree(QString const & pi_sourceIntanceId, unsigned int pi_uiLevel, QString const & key, levelAttributes & po_entries)
{
    bool bRes = true;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceIntanceId);
    if (pSource)
    {
        if (pSource->isFetchByMinimalEntriesOrMandatoryAttributes())
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

bool medDataModel::mandatoriesAttributes(QString const & pi_sourceIntanceId, unsigned int pi_uiLevel, QString const & parentKey, levelAttributes & po_entries)
{
    bool bRes = true;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceIntanceId);
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

bool medDataModel::mandatoryAttributesKeys(QString const & pi_sourceIntanceId, unsigned int pi_uiLevel, QStringList & po_attributes)
{
    bool bRes = true;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceIntanceId);
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

bool medDataModel::levelCount(QString const & pi_sourceIntanceId, unsigned int & po_uiLevelMax)
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

QList<medAbstractParameter *> medDataModel::filteringParameters(const QString &pi_sourceInstanceId)
{
    QList<medAbstractParameter *> paramRes;
    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        paramRes = pSource->getFilteringParameters();
    }
    return paramRes;
}

bool medDataModel::optionalAttributes(QString const & pi_sourceIntanceId, unsigned int pi_uiLevel, QString const & key, datasetAttributes & po_attributes, datasetAttributes & po_tags)
{
    bool bRes = false;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceIntanceId);
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

//TODO19
//bool medDataModel::getOptionalAttributes(QString const & pi_sourceIntanceId, unsigned int pi_uiLevel, QString const & key, QList<QMap<int, QString>>& po_attributes)
//{
//    bool bRes = true;
//
//    //medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceIntanceId);
//    //if (pSource)
//    //{
//    //    QMap<int, QString> roleValueRes;
//    //    auto attributes4level = pSource->getAdditionalAttributes(pi_uiLevel, key);
//    //    for (auto attributeEntr : attributes4level)
//    //    {
//    //        auto attrKeys = attribute.keys();
//    //        for (auto k : attrKeys)
//    //        {
//    //            roleValueRes[1001] = k;
//    //            roleValueRes[1002] = attrKeys[k];
//    //        }
//    //        
//    //    }
//    //}
//    //else
//    //{
//    //    bRes = false;
//    //}
//
//    return bRes;
//}



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
            auto pSource = m_sourceIdToInstanceMap[source];
            QVariant data = pSource->getDirectData(
                    index.uri().size() - 2, index.uri()[index.uri().size() - 1]);
            medAbstractData *pDataTmp = nullptr;
            if (data.canConvert<medAbstractData *>())
            {
                pDataTmp = data.value<medAbstractData *>();
            }
            else if (data.canConvert<QString>())
            {
                pDataTmp = medDataImporter::convertSingleDataOnfly(data.toString());
                pDataTmp->setDataIndex(index);
                QModelIndex modelIndex = m_sourcesModelMap[pSource]->toIndex(index.asString());
                QString hruUri = m_sourcesModelMap[pSource]->toHumanReadableUri(modelIndex);
                QString name = hruUri.right(hruUri.size()-hruUri.lastIndexOf("\r\n")-2);
                pDataTmp->setExpectedName(name);
                pDataTmp->setMetaData(medMetaDataKeys::SeriesDescription.key(), name);
            }
            else if (data.canConvert<QByteArray>())
            {
                qDebug()<<"Not implemented yet";
            }

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
            metaRes = model->getMendatoriesMetaData(index);
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

QUuid medDataModel::saveData(medAbstractData &data)
{
    QStringList parentUri = data.dataIndex().uri();
//    if (!data.parentData().isEmpty())
//    {
//        auto parentData = data.parentData()[0];
//        parentUri = parentData->dataIndex().uri();
//        parentUri.pop_back();
//    }

    auto pSource = m_sourceIdToInstanceMap[parentUri[0]];
    QTemporaryDir tmpDir;
    if (tmpDir.isValid())
    {
        QString fullTmpPath = tmpDir.path() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "");/*+ dataName*/;
        bool bWritableData = medDataExporter::convertSingleDataOnfly(&data, fullTmpPath);
        if (bWritableData)
        {
            QVariant dataset;
            dataset.setValue(fullTmpPath);

            QString key = pSource->addData(dataset, parentUri, data.getExpectedName());
            // ////////////////////////////////////////////////////////////////////////
            // Refresh dataModelElement
            if (!key.isEmpty())
            {
                m_sourcesModelMap[pSource]->fetch(parentUri);
            }
            else
            {
//                sourceIsOnline(splittedUri[0]);
            }
            parentUri.append(key);
            data.dataIndex().setUri(parentUri);

        }

    }

    return QUuid();
}

void medDataModel::addData(medAbstractData * pi_dataset, QString uri)
{
//    QStringList splittedUri;
//    int sourceDelimPos = uri.indexOf(':');
//    splittedUri.append(uri.left(sourceDelimPos));
//    QString sourceUri = uri.right(uri.size() - sourceDelimPos - 1);
//    splittedUri.append(sourceUri.split('/'));
//
//    //TODO verifier la presence dans la map
//    auto pSource = m_sourceIdToInstanceMap[splittedUri[0]];
//    // ////////////////////////////////////////////////////////////////////////
//    // Adding dataset to the source
//    QVariant dataset;
//    dataset.setValue(pi_dataset);
//    datasetAttributes mandatoryAttributes;
//    medAbstractSource::datasetAttributes4 additionalAttributes;
//    //TODO get mandatory & additional attributes for dataset ?
//    QString key = pSource->addData(dataset, sourceUri, "new data segmented");
//
//    // ////////////////////////////////////////////////////////////////////////
//    // Refresh dataModelElement
//    if (!key.isEmpty())
//    {
//        m_sourcesModelMap[pSource]->fetch(sourceUri.left(sourceUri.lastIndexOf('/')));
//    }
//    else
//    {
//        sourceIsOnline(splittedUri[0]);
//    }
}

void medDataModel::addData(medDataIndex * pi_datasetIndex, QString uri)
{
    //addData(getAbstractDataFromIndex(pi_datasetIndex), uri);
}

void medDataModel::refresh(QString uri)
{
//    QStringList splittedUri;
//    int sourceDelimPos = uri.indexOf(':');
//    splittedUri.append(uri.left(sourceDelimPos));
//    QString sourceUri = uri.right(uri.size() - sourceDelimPos - 1);
//    splittedUri.append(sourceUri.split('/'));
//
//    auto pSource = m_sourceIdToInstanceMap[splittedUri[0]];
//    m_sourcesModelMap[pSource]->fetch(sourceUri);
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

void medDataModel::expandAll(const QString &sourceInstanceId)
{
    if (m_sourceIdToInstanceMap.contains(sourceInstanceId))
    {
        auto pSource = m_sourceIdToInstanceMap[sourceInstanceId];
        m_sourcesModelMap[pSource]->expandAll();
    }
}

