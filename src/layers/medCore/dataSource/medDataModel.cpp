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

bool medDataModel::getSourceGlobalInfo(QString const & pi_sourceIntanceId, QStringList & pi_columnName, bool & pi_bOnline, bool & pi_bWritable, bool & pi_bCache)
{
    return false;
}

bool medDataModel::getInfoLevel(QString const & pi_sourceIntanceId, unsigned int pi_uiLevel, QVariantList & po_entries)
{
    return false;
}

void medDataModel::addSource(medAbstractSource * pi_source)
{
}


void medDataModel::removeSource(medAbstractSource * pi_source)
{
}

void medDataModel::addData(medAbstractData * pi_dataset, QString uri)
{
}

void medDataModel::addData(medDataIndex * pi_datasetIndex, QString uri)
{
    //addData(getAbstractDataFromIndex(pi_datasetIndex), uri);
}
