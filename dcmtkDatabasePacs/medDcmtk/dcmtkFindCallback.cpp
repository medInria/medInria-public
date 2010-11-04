#include "dcmtkFindCallback.h"

#include "dcmtk/dcmnet/diutil.h"

#include "dcmtkResultDataset.h"
#include "dcmtkLogger.h"
#include "dcmtkNode.h"

//---------------------------------------------------------------------------------------------

dcmtkFindScuCallback::dcmtkFindScuCallback( OFBool extractResponsesToFile, int cancelAfterNResponses): DcmFindSCUCallback()
                                            , extractResponsesToFile_(extractResponsesToFile)
                                            , cancelAfterNResponses_(cancelAfterNResponses)
{
    m_cancelRequest = false;
}

//---------------------------------------------------------------------------------------------

void dcmtkFindScuCallback::callback(T_DIMSE_C_FindRQ *request, int responseCount, T_DIMSE_C_FindRSP *rsp,
                                    DcmDataset *responseIdentifiers)
 {
    /* dump delimiter */
    dcmtkLogger::infoStream() << "---------------------------";

    /* dump response number */
    dcmtkLogger::infoStream() << "Find Response: " << responseCount << " (" << DU_cfindStatusString(rsp->DimseStatus) << ")";

    /* dump data set which was received */
    dcmtkLogger::infoStream() << DcmObject::PrintHelper(*responseIdentifiers);

    dcmtkResultDataset* findDs = new dcmtkResultDataset;

    // get the basic key values
    const char* szhelper;
    responseIdentifiers->findAndGetString(DcmTagKey(0x0020,0x000D),szhelper);
    findDs->setStudyInstanceUID(szhelper);

    responseIdentifiers->findAndGetString(DcmTagKey(0x0020,0x000E),szhelper);
    findDs->setSeriesInstanceUID(szhelper);

    responseIdentifiers->findAndGetString(DcmTagKey(0x0008,0x0018),szhelper);
    findDs->setSOPInstanceUID(szhelper);


    // check what keys are requested and add them to the container
    dcmtkKey* reqKey = m_keyCont->getFirst();

    while (m_keyCont->isValid())
    {
        responseIdentifiers->findAndGetString(DcmTagKey(reqKey->group,reqKey->elem),szhelper);
        if (szhelper != NULL)
        {
            dcmtkKey *key = new dcmtkKey;
            key->group = reqKey->group;
            key->elem  = reqKey->elem;
            key->value = szhelper;
            findDs->getKeyContainer()->add(key);
        }
        reqKey = m_keyCont->getNext();
    }

    m_ds->add(findDs);

    /* should we send a cancel back ?? */
    if ((cancelAfterNResponses_ == responseCount) || (m_cancelRequest) )
    {
        dcmtkLogger::infoStream() << "Sending Cancel Request, MsgID: " << request->MessageID << ", PresID: " << presId_;
        OFCondition cond = DIMSE_sendCancelRequest(assoc_, presId_, request->MessageID);
        if (cond.bad())
        {
            OFString temp_str;
            dcmtkLogger::errorStream() << "Cancel Request Failed: " << DimseCondition::dump(temp_str, cond);
        }
        m_cancelRequest = false;
    }
}



//---------------------------------------------------------------------------------------------

void dcmtkFindScuCallback::setResultDatasetContainer(dcmtkContainer<dcmtkResultDataset*>*  dataCont)
{
    m_ds = dataCont;
}

//---------------------------------------------------------------------------------------------

void dcmtkFindScuCallback::setKeyContainer(dcmtkContainer<dcmtkKey*>* cont )
{
    m_keyCont = cont;
}

void dcmtkFindScuCallback::sendCancelRequest( )
{
    m_cancelRequest = true;
}

//---------------------------------------------------------------------------------------------
