/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QDebug>

#include "dcmFindCallbacks.h"

#include <dcmtk/dcmdata/dcdeftag.h>

dcmFindCallbacks::dcmFindCallbacks(QMap<DcmTagKey, QString> filters)
{
    tags = filters;
} 

void dcmFindCallbacks::callback(T_DIMSE_C_FindRQ *request, int &responseCount, T_DIMSE_C_FindRSP *rsp, DcmDataset *responseIdentifiers)
{
    OFString str;
    DIMSE_dumpMessage(str, *rsp, DIMSE_INCOMING);
    qDebug() << "Find response" << responseCount << ":" << str.c_str();
    
    OFString value;
    QMap<DcmTagKey, QString> responses;

    for (DcmTagKey key : tags.keys())
    {
        responseIdentifiers->findAndGetOFString(key, value);
        responses.insert(key, value.c_str());
        
        // Display values retrieved by C-FIND for DICOM tags
        // qDebug() << key.toString().c_str() << "=" << value.c_str();
    }

    responsesList.append(responses);
}

QList<QMap<DcmTagKey, QString>> dcmFindCallbacks::getResponsesList()
{
    return responsesList;
}