/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifndef DCMFINDCALLBACKS_H
#define DCMFINDCALLBACKS_H

#include <QObject>
#include <QList>
#include <QMap>

#include <dcmtk/dcmnet/dfindscu.h>

class dcmFindCallbacks : public QObject, public DcmFindSCUCallback
{
    Q_OBJECT
public:
    dcmFindCallbacks(){};
    dcmFindCallbacks(QMap<DcmTagKey, QString> filters);
    
    void callback(T_DIMSE_C_FindRQ *request, int &responseCount, T_DIMSE_C_FindRSP *rsp, DcmDataset *responseIdentifiers) override;
    QList<QMap<DcmTagKey, QString>> getResponsesList();

private:
    QMap<DcmTagKey, QString> tags; // DICOM tag keys and their possible values used in the FIND query
    QList<QMap<DcmTagKey, QString>> responsesList; // retrieve tag keys values into a list representing all patients, studies, or series found according to level
};

#endif //DCMFINDCALLBACKS_H

/**
 * @fn void dcmFindCallbacks::callback(T_DIMSE_C_FindRQ *request, int &responseCount, T_DIMSE_C_FindRSP *rsp, DcmDataset *responseIdentifiers)
 * @brief retrieve values of the DICOM tag keys returned by the FIND query response 
 * @details filled responsesList list 
 * @param request [in] [int request] FIND request
 * @param responseCount [in] [out request] the request number
 * @param rsp [in] [out request] FIND response
 * @param responseIdentifiers [in] [out request] content including tag keys values
*/

/**
 * @fn QList<QMap<DcmTagKey, QString>> dcmFindCallbacks::getResponsesList()
 * @return a QList containing the DICOM tag keys values for each instance found
*/