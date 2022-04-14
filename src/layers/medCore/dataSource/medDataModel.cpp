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
    // QMap<QString, QString> substitutionsCharacters;
    // substitutionsCharacters["\\"] = " ";
    // substitutionsCharacters["/"]  = " ";
    // substitutionsCharacters[":"]  = " ";
    // substitutionsCharacters["*"]  = " ";
    // substitutionsCharacters["?"]  = " ";
    // substitutionsCharacters["<"]  = " ";
    // substitutionsCharacters[">"]  = " ";
    // substitutionsCharacters["\""] = " ";
    // substitutionsCharacters["|"]  = " ";
    // substitutionsCharacters["\r"] = " ";
    // substitutionsCharacters["\n"] = " ";

    // m_generalWritingPolicy.setSubstitutionsCharacters(substitutionsCharacters);
}

medDataModel::~medDataModel()
{
    int i = 0;
    ++i;
}




void medDataModel::setDefaultWorkingSource(medAbstractSource * pi_pSource)
{
    m_defaultSource = pi_pSource;
}

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Deal with sources
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool medDataModel::sourceGlobalInfo(QString const & pi_sourceInstanceId, bool & pi_bOnline, bool & pi_bLocal, bool & pi_bWritable, bool & pi_bCache)
{
    bool bRes = true;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
    if (pSource)
    {
        pi_bWritable = pSource->isWritable();
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

bool medDataModel::attributesForBuildTree(QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QString const & key, levelAttributes & po_entries)
{
    bool bRes = true;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceInstanceId);
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

bool medDataModel::mandatoriesAttributes(QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QString const & parentKey, levelAttributes & po_entries)
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

bool medDataModel::mandatoryAttributesKeys(QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QStringList & po_attributes)
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

bool medDataModel::levelCount(QString const & pi_sourceInstanceId, unsigned int & po_uiLevelMax)
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

bool medDataModel::filteringParameters(const QString &pi_sourceInstanceId, QList<medAbstractParameter*> & po_parameters)
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

bool medDataModel::optionalAttributes(QString const & pi_sourceInstanceId, unsigned int pi_uiLevel, QString const & key, datasetAttributes & po_attributes, datasetAttributes & po_tags)
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

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Advanced Accessors
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString medDataModel::getInstanceName(QString const & pi_sourceInstanceId)
{
    QString instanceNameRes;
    if (m_sourceIdToInstanceMap.contains(pi_sourceInstanceId))
    {
        instanceNameRes = m_sourceIdToInstanceMap[pi_sourceInstanceId]->getInstanceName();
    }
    return instanceNameRes;
}

QList<medSourceItemModel*> medDataModel::models()
{
    QList<medSourceItemModel*> sourceItemModelListRes;

    for (auto const & sourceId : m_sourceInstanceIdOrderedList)
    {
        sourceItemModelListRes.push_back(m_sourcesModelMap[m_sourceIdToInstanceMap[sourceId]]);
    }

    return sourceItemModelListRes;
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

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Datasets handling
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
                if (pDataTmp)
                {
                    pDataTmp->setDataIndex(index);
                    QModelIndex modelIndex = m_sourcesModelMap[pSource]->toIndex(index.asString());
                    QString hruUri = m_sourcesModelMap[pSource]->toHumanReadableUri(modelIndex);
                    QString name = hruUri.right(hruUri.size() - hruUri.lastIndexOf("\r\n") - 2);
                    QStringList hruUriAsList = hruUri.split("\r\n", QString::SkipEmptyParts);
                    pDataTmp->setExpectedName(name);
                    pDataTmp->setMetaData(medMetaDataKeys::SeriesDescription.key(), name);
                    QString studyDesc;
                    if (hruUriAsList.size() > 2)
                    {
                        studyDesc = hruUriAsList.at(hruUriAsList.size()-2);
                    }
                    pDataTmp->setMetaData(medMetaDataKeys::StudyDescription.key(), studyDesc);
                }
                else
                {
                    qDebug()<<"Unable to read data "<<data.toString();
                }
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
        //TODO
    }

    return metaRes;
}

bool medDataModel::getDataNames(QStringList uri, QStringList &names)
{
    bool bRes = false;

    if (!uri.isEmpty())
    {
        medSourceItemModel* pModel = getModel(uri[0]);
        if (pModel)
        {
            bRes = pModel->getDataNames(uri, names);
        }
    }

    return bRes;
}


QUuid medDataModel::writeResultsHackV3(medAbstractData &data, bool originSrc)
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
            pi_sourceId = getSourceToWrite(pi_UriOfRelatedData[0])->getInstanceId();
        }
        else
        {
            pi_sourceId = getDefaultWorkingSource()->getInstanceId();
        }

        pi_writingPolicyData.baseName = data.metadata(medMetaDataKeys::SeriesDescription.key());

        writeResults(pi_sourceId, pi_pData, pi_UriOfRelatedData, pi_sugestedPath, pi_writingPolicyData, nullptr);
    }
    return QUuid();
}

bool medDataModel::saveData(medAbstractData *pi_pData, QString const &pi_baseName, QStringList &pio_uri)
{
    bool bRes = false;

    if (!pio_uri.isEmpty())
    {
        medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pio_uri[0]);
        medSourceItemModel* pModel = getModel(pio_uri[0]);
        if (pSource && pModel)
        {
            auto typeAndFormat = pSource->getTypeAndFormat();
            auto bLocal = pSource->isLocal();
            auto bCached = pSource->isCached();

            QVariant data;
            medAbstractSource::levelMinimalEntries minimalEntries;
            minimalEntries.name = pi_baseName;
            unsigned int uiLevel = pio_uri.size() - 1;
            QString parentKey = pio_uri.last();

            QTemporaryDir tmpDir;
            if (pSource->getIOInterface() == IO_FILE)
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
                            pModel->fetch(pio_uri);
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
                        data.setValue(fullTmpPath);
                    }
                }
            }
            else if (pSource->getIOInterface() == IO_MEDABSTRACTDATA)
            {
                data.setValue(pi_pData);
            }
            else if (pSource->getIOInterface() == IO_STREAM)
            {
                qDebug() << "Not Implemented yet. For now, data is an invalid QVariant";
            }
            
            if (data.isValid())
            {
                if (bLocal)
                {
                    bRes = pSource->addDirectData(data, minimalEntries, uiLevel, parentKey);
                    if (bRes)
                    {
                        pModel->fetch(pio_uri);
                        pio_uri.push_back(minimalEntries.key);
                    }
                }
                else if (bCached)
                {
                    bRes = pSource->commitData(data, minimalEntries, uiLevel, parentKey);
                }
                else
                {
                    bRes = pSource->addAssyncData(data, minimalEntries, uiLevel, parentKey);
                }
            }

        }
    }

    return bRes;
}




bool medDataModel::writeResults(QString pi_sourceId, medAbstractData * pi_pData, QStringList pi_UriOfRelatedData, QString pi_sugestedPath, medWritingPolicyData & pi_writingPolicyData, medAbstractWritingPolicy * pi_pWritingPolicy)
{
    bool bRes = true;

    auto * pWp = pi_pWritingPolicy;

    if (pWp == nullptr)
    {
        pWp = getSourceWPolicy(pi_sourceId);

        if (pWp == nullptr)
        {
            pWp = getGeneralWPolicy();
        }
    }

    QMap<QString, QString> metaData;//pi_pData->metaData(); //TODO
    QString computedName = pWp->computeName(pi_writingPolicyData.baseName, pi_writingPolicyData.prefix, pi_writingPolicyData.suffix, metaData);

    auto srcOri = m_sourceIdToInstanceMap.value(pi_UriOfRelatedData[0]);
    QString pathOfRelatedData;
    if (srcOri && srcOri->getLevelCount() != 0)
    {
        pathOfRelatedData = convertToPath(pi_UriOfRelatedData); // to path of pi_URIOfRelatedData
    }
    // QString pathOfRelatedData = convertToPath(pi_UriOfRelatedData); // to path of pi_URIOfRelatedData
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
    // Check la coherence de la proposition par rapport � l'URI
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
        auto pSource = m_sourceIdToInstanceMap[pi_sourceId];
        int levelToWrite = m_sourceIdToInstanceMap[suggestedSourceId]->getLevelDesiredWritable();
        if (pSource->getLevelDesiredWritable()==-1 && levelToWrite!=-1)
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
        // si bRes true alors write
        if (bRes && pSource->isLevelWritable(levelToWriteData))
        {
            QStringList names;
            getDataNames(uri, names);
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

bool medDataModel::createPath(QString pi_sourceId, QStringList pi_folders, QStringList &po_uri, QMap<int, QString> pi_knownKeys)
{
    bool bRes = true;

    po_uri.clear();

    auto * pModel = getModel(pi_sourceId);
    medAbstractSource   * pSource = m_sourceIdToInstanceMap.value(pi_sourceId);
    if (pModel && pSource)
    {
        po_uri.push_front(pi_sourceId);

        for (unsigned int i = 0; (i < pi_folders.size() && bRes); ++i)
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
                    if (keyTmp.isEmpty() && pSource->isLevelWritable(i))
                    {
                        medAbstractSource::levelMinimalEntries lme;
                        medAbstractSource::datasetAttributes4 dataSetAttributes;
                        lme.name = pi_folders[i];
                        bRes = pSource->createFolder(lme, dataSetAttributes, i, po_uri.last());
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

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// sources handling
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void medDataModel::addSource(medAbstractSource * pi_source)
{
    if (pi_source)
    {
        auto instanceId = pi_source->getInstanceId();
        m_sourceInstanceIdOrderedList.push_back(instanceId);
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

medAbstractSource * medDataModel::getDefaultWorkingSource()
{
    return m_defaultSource;
}

medAbstractSource * medDataModel::getSourceToWrite(QString pi_sourceIdDst)
{
    medAbstractSource * pSource = nullptr;

    if (pi_sourceIdDst.isEmpty())
    {
        pSource = medDataModel::instance()->getDefaultWorkingSource();
        pi_sourceIdDst = pSource->getInstanceId();
    }
    else
    {
        pSource = medSourcesLoader::instance()->getSource(pi_sourceIdDst);
    }

    return pSource;
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






QString medDataModel::uriAsString(QStringList pi_uri)
{
    QString uriRes;

    if (pi_uri.size() > 0)
    {
        uriRes = pi_uri.takeFirst() + ':';
        uriRes += pi_uri.join("\r\n");
    }

    return uriRes;
}

QStringList medDataModel::uriAsList(QString pi_uri)
{
    QStringList uriRes;

    int sourceDelimterIndex = pi_uri.indexOf(QString(":"));
    if (sourceDelimterIndex > -1)
    {
        uriRes = pi_uri.right(pi_uri.size() - sourceDelimterIndex - 1).split(QString("\r\n"));
        uriRes.push_front(pi_uri.left(sourceDelimterIndex));
    }
    else
    {
        uriRes = pi_uri.split(QString("\r\n"));
        uriRes.push_front("");
    }
    
    return uriRes;
}

QString medDataModel::convertToPath(QStringList pi_uri)
{
    QString pathRes;

    auto * pModel = getModel(pi_uri[0]);
    if (pModel != nullptr)
    {
        auto index = pModel->toIndex(pi_uri);
        pathRes = pModel->toHumanReadableUri(index);
    }

    return pathRes;
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

void medDataModel::progress(int pi_iRequest, medAbstractSource::eRequestStatus status)
{

}

void medDataModel::expandAll(const QString &sourceInstanceId)
{
    if (m_sourceIdToInstanceMap.contains(sourceInstanceId))
    {
        auto pSource = m_sourceIdToInstanceMap[sourceInstanceId];
        m_sourcesModelMap[pSource]->expandAll();
    }
}

medAbstractWritingPolicy * medDataModel::getSourceWPolicy(QString pi_sourceId)
{
    medAbstractWritingPolicy* writePolicyRes = nullptr;

    medAbstractSource *pSourceDst = getSourceToWrite(pi_sourceId);
    if (pSourceDst)
    {
        writePolicyRes = pSourceDst->getWritingPolicy();
    }

    return writePolicyRes;
}

//medAbstractWritingPolicy * medDataModel::getWPolicy(medAbstractWritingPolicy * pi_policy, QString pi_sourceId)
//{
//    medAbstractWritingPolicy* writePolicyRes = pi_policy;
//    if (writePolicyRes == nullptr)
//    {
//        medAbstractSource *pSourceDst = getSourceToWrite(pi_sourceId);
//        if (pSourceDst)
//        {
//            writePolicyRes = pSourceDst->getWritingPolicy();
//
//            if (writePolicyRes == nullptr)
//            {
//                writePolicyRes = &m_generalWritingPolicy;
//            }
//        }
//    }
//    return writePolicyRes;
//}
