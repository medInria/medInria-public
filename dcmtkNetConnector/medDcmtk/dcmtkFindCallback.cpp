#include "dcmtkFindCallback.h"

#include "dcmtk/dcmnet/diutil.h"

#include "dcmtkFindDataset.h"
#include "dcmtkLogger.h"

//---------------------------------------------------------------------------------------------

dcmtkFindScuCallback::dcmtkFindScuCallback( OFBool extractResponsesToFile, int cancelAfterNResponses): DcmFindSCUCallback()
                                            , extractResponsesToFile_(extractResponsesToFile)
                                            , cancelAfterNResponses_(cancelAfterNResponses)
{
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

    dcmtkFindDataset* findDs = new dcmtkFindDataset();
    responseIdentifiers->findAndGetString(DcmTagKey(0x0010,0x0010),findDs->patientName);
    responseIdentifiers->findAndGetString(DcmTagKey(0x0010,0x0020),findDs->patientID);
    responseIdentifiers->findAndGetString(DcmTagKey(0x0010,0x0030),findDs->patientBd);
    responseIdentifiers->findAndGetString(DcmTagKey(0x0010,0x0020),findDs->patientID);
    responseIdentifiers->findAndGetString(DcmTagKey(0x0010,0x0040),findDs->patientSex);

    responseIdentifiers->findAndGetString(DcmTagKey(0x0020,0x000D),findDs->studyInstanceUID);
    responseIdentifiers->findAndGetString(DcmTagKey(0x0020,0x0010),findDs->studyID);

    responseIdentifiers->findAndGetString(DcmTagKey(0x0008,0x0050),findDs->accNo);
    responseIdentifiers->findAndGetString(DcmTagKey(0x0008,0x0090),findDs->refPhysician);
    responseIdentifiers->findAndGetString(DcmTagKey(0x0008,0x1030),findDs->studyDescr);
    responseIdentifiers->findAndGetString(DcmTagKey(0x0008,0x0061),findDs->modalitiesInStudy);
    findDs->copy();
    m_ds->push_back(findDs);

   
    /* in case extractResponsesToFile is set the responses shall be extracted to a certain file */
    if (extractResponsesToFile_) {
        char rspIdsFileName[1024];
        sprintf(rspIdsFileName, "rsp%04d.dcm", responseCount);
        DcmFindSCU::writeToFile(rspIdsFileName, responseIdentifiers);
    }

    /* should we send a cancel back ?? */
    if (cancelAfterNResponses_ == responseCount)
    {
        dcmtkLogger::infoStream() << "Sending Cancel Request, MsgID: " << request->MessageID << ", PresID: " << presId_;
        OFCondition cond = DIMSE_sendCancelRequest(assoc_, presId_, request->MessageID);
        if (cond.bad())
        {
            OFString temp_str;
            dcmtkLogger::errorStream() << "Cancel Request Failed: " << DimseCondition::dump(temp_str, cond);
        }
    }
}



//---------------------------------------------------------------------------------------------

void dcmtkFindScuCallback::setResultDataset(std::vector<dcmtkFindDataset*>*  resultSet)
{
    m_ds = resultSet;
}

//---------------------------------------------------------------------------------------------

