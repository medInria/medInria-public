/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include<medDataModel.h>

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

bool medDataModel::getLevelMetaData(QString const & pi_sourceIntanceId, unsigned int pi_uiLevel, QString const & key, QVariantList & po_entries)
{
    bool bRes = true;

    medAbstractSource* pSource = m_sourceIdToInstanceMap.value(pi_sourceIntanceId);
    if (pSource)
    {
        auto listOfMinimalEntries = pSource->getMinimalEntries(pi_uiLevel, key);
        for (auto &minimalEntry : listOfMinimalEntries)
        {
            po_entries.append(QStringList({ minimalEntry.key, minimalEntry.name, minimalEntry.description }));
        }
    }
    else
    {
        bRes = false;
    }
    return bRes;
}

bool medDataModel::getLevelAttributes(QString const & pi_sourceIntanceId, unsigned int pi_uiLevel, QStringList & po_attributes)
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

medDataModelElement * medDataModel::getModel(QString const & pi_sourceIntanceId)
{
    return m_sourcesModelMap.first(); //TODO
}


void medDataModel::addSource(medAbstractSource * pi_source)
{
    if (pi_source)
    {
        auto instanceId = pi_source->getInstanceId();
        m_sourceIdToInstanceMap[instanceId] = pi_source;
        m_sourcesModelMap[pi_source] = new medDataModelElement(this, instanceId);
    }
}


void medDataModel::removeSource(medAbstractSource * pi_source)
{
    if (pi_source)
    {
        m_sourceIdToInstanceMap.remove(pi_source->getInstanceId());
        delete m_sourcesModelMap.take(pi_source);
    }
}

void medDataModel::addData(medAbstractData * pi_dataset, QString uri)
{
}

void medDataModel::addData(medDataIndex * pi_datasetIndex, QString uri)
{
    //addData(getAbstractDataFromIndex(pi_datasetIndex), uri);
}
