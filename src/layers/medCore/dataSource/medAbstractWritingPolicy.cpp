/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>

#include <medCoreExport.h>
#include <medDataModel.h>



int medAbstractWritingPolicy::levelToWrite(QString pi_sourceId)
{
    int iRes = -1;

    auto * pSource = medDataModel::instance()->getSourceToWrite(pi_sourceId);
    if (pSource)
    {
        iRes = static_cast<int>(pSource->getLevelDesiredWritable());
    }

    return iRes;
}

bool medAbstractWritingPolicy::islevelWritable(QString pi_sourceId, unsigned int pi_uiLevel)
{
    bool bRes = false;

    auto * pSource = medDataModel::instance()->getSourceToWrite(pi_sourceId);
    if (pSource)
    {
        bRes = pSource->isLevelWritable(pi_uiLevel);
    }

    return bRes;
}

QStringList medAbstractWritingPolicy::basePathDstFromOrigin(QString sourceDstId, QStringList pi_originPath)
{
    pi_originPath.pop_back();
    pi_originPath[0] = sourceDstId;

    return pi_originPath;
}


//bool /*virtual bool medAbstractProcess::*/ writeResults(medAbstractData *data, QString const & pi_baseName, QStringList pi_relativeDirDst, QString pi_prefix, QString pi_suffix, QMap<QString, QString> pi_metaData)
//{
//	bool bRes = true;
//	
//	medDataModel::instance()->write(pi_pData, pi_hPath, pi_pSourceDst);
//
//    return bRes;
//}
//
//
//medAbstractWritingPolicy* medAbstractProcess::getWPolicy(medAbstractSource *pi_pSourceDst)
//{
//  medAbstractWritingPolicy* writePolicyRes = nullptr;
//
//  if (pi_pSourceDst)
//  {
//      writePolicyRes = d->writePolicy;
//	
//      if(writePolicyRes == nullptr)
//	  {
//          writePolicyRes = medDataModel::instance()->getWPolicy(pi_pSourceDst);
//	  }
//  }
//
//	
//	return writePolicyRes;
//}

