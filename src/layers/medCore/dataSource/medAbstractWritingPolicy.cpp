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



int medAbstractWritingPolicy::levelToWrite(QStringList pi_dstUri)
{
    int iRes = -1;

    auto * pSource = medSourcesLoader::instance()->getSource(pi_dstUri[0]);
    if (pSource)
    {
        iRes = static_cast<int>(pSource->getLevelDesiredWritable());
    }

    return iRes;//TODO ADD DISAMBIGOUS HUMAN URI VS URI
}

bool medAbstractWritingPolicy::islevelWritable(QStringList pi_dstUri)
{
    bool bRes = false;

    auto * pSource = medSourcesLoader::instance()->getSource(pi_dstUri[0]);
    if (pSource)
    {
        bRes = pSource->isLevelWritable(pi_dstUri.size());
    }

    return bRes; //TODO ADD DISAMBIGOUS HUMAN URI VS URI
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

