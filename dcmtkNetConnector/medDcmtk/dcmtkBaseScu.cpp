#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/assoc.h"

#include "dcmtkBaseScu.h"
#include "dcmtkLogger.h"

//---------------------------------------------------------------------------------------------

dcmtkBaseScu::dcmtkBaseScu()
{
    resetDefaultParams();
}

//---------------------------------------------------------------------------------------------

dcmtkBaseScu::~dcmtkBaseScu()
{

}

//---------------------------------------------------------------------------------------------

void dcmtkBaseScu::setQueryLevel(eQueryLevel level)
{
    m_currentQueryLevel = level;

    switch(level)
    {
    case PATIENT:
        this->addQueryAttribute("0008","0052","PATIENT");
        break;
    case STUDY:
        this->addQueryAttribute("0008","0052","STUDY");
        break;
    case SERIES:
        this->addQueryAttribute("0008","0052","SERIES");
        break;
    case IMAGE:
        this->addQueryAttribute("0008","0052","IMAGE");
        break;
    default:
        this->addQueryAttribute("0008","0052","STUDY");
        break;
    }
}

//---------------------------------------------------------------------------------------------

bool dcmtkBaseScu::addQueryAttribute(const char* group, 
                                     const char* elem, 
                                     const char* value)
{
    if ((group == "") || (elem == "") || (value == ""))
        return false;

    std::string cmStr;
    cmStr.append(group);
    cmStr.append(",");
    cmStr.append(elem);
    cmStr.append("=");
    cmStr.append(value);


    processQueryAttribute(cmStr.c_str());
    return true;
}

//---------------------------------------------------------------------------------------------

bool dcmtkBaseScu::addQueryAttribute(const char* key)
{
    if (key == "") 
        return false;

    processQueryAttribute(key);
    return true;
}

//---------------------------------------------------------------------------------------------

void dcmtkBaseScu::setConnectionParams(const char* peerTitle, const char* peerIP, unsigned short peerPort, 
                                       const char* ourTitle, const char* ourIP, unsigned short ourPort)
{

    opt_peerTitle = peerTitle;
    opt_peer = peerIP;
    opt_port = peerPort; 
    opt_ourTitle = ourTitle;
    opt_ourIP = ourIP;
    opt_retrievePort = ourPort;
}

//---------------------------------------------------------------------------------------------

void dcmtkBaseScu::resetDefaultParams()
{
    opt_peerTitle =                 "ANY-SCU";
    opt_peer =                      "localhost";
    opt_port =                      104; 
    opt_ourTitle =                  "DCMTKTOOL";
    opt_ourIP =                     "localhost";
    opt_retrievePort =              105;

    opt_abortAssociation =          OFFalse;
    opt_acse_timeout =              30;
    opt_blockMode =                 DIMSE_BLOCKING;
    opt_cancelAfterNResponses =     -1;
    opt_dimse_timeout =             0;
    opt_extractResponsesToFile =    OFFalse;
    opt_maxReceivePDULength =       ASC_DEFAULTMAXPDU;
    opt_maxSendPDULength=           ASC_DEFAULTMAXPDU;
    opt_networkTransferSyntax =     EXS_Unknown;
    opt_repeatCount =               1;
    opt_secureConnection =          OFFalse; /* default: no secure connection */
    opt_abstractSyntax =            UID_FINDStudyRootQueryRetrieveInformationModel; 
    opt_sleepAfter =                0;
    opt_sleepDuring =               0;

    net =                           NULL; 


}

//---------------------------------------------------------------------------------------------

int dcmtkBaseScu::CreateAssociation(OFCondition &cond, T_ASC_Network* net, T_ASC_Parameters* params,
                                    T_ASC_Association* assoc)
{
    /* create association */
    dcmtkLogger::info("Requesting Association");
    cond = ASC_requestAssociation(net, params, &assoc);
    if (cond.bad()) {
        if (cond == DUL_ASSOCIATIONREJECTED) {
            T_ASC_RejectParameters rej;

            ASC_getRejectParameters(params, &rej);
            dcmtkLogger::error("Association Rejected:");
            dcmtkLogger::error( ASC_printRejectParameters(temp_str, &rej));
            return 1;
        } else {
            dcmtkLogger::error("Association Request Failed:");
            dcmtkLogger::error( DimseCondition::dump(temp_str, cond));
            return 1;
        }
    }
    /* what has been accepted/refused ? */
    dcmtkLogger::debug("Association Parameters Negotiated:");
    dcmtkLogger::debug(ASC_dumpParameters(temp_str, params, ASC_ASSOC_AC));

    if (ASC_countAcceptedPresentationContexts(params) == 0) {
        dcmtkLogger::error( "No Acceptable Presentation Contexts");
        return 1;
    }

    dcmtkLogger::infoStream() << "Association Accepted (Max Send PDV: " << assoc->sendPDVLength << ")";

    return 0;
}

//---------------------------------------------------------------------------------------------

int dcmtkBaseScu::ReleaseAssociation(OFCondition &cond)
{
        /* tear down association */
    if (cond == EC_Normal)
    {
        if (opt_abortAssociation) {
            dcmtkLogger::info( "Aborting Association");
            cond = ASC_abortAssociation(assoc);
            if (cond.bad()) {
                 dcmtkLogger::errorStream() << "Association Abort Failed: " << DimseCondition::dump(temp_str, cond);
                return 1;
            }
        } else {
            /* release association */
            dcmtkLogger::info( "Releasing Association");
            cond = ASC_releaseAssociation(assoc);
            if (cond.bad())
            {
                dcmtkLogger::error("Association Release Failed:");
                dcmtkLogger::error( DimseCondition::dump(temp_str, cond));
                return 1;
            }
        }
    }
    else if (cond == DUL_PEERREQUESTEDRELEASE)
    {
        dcmtkLogger::error("Protocol Error: Peer requested release (Aborting)");
        dcmtkLogger::info("Aborting Association");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            dcmtkLogger::errorStream() << "Association Abort Failed: " << DimseCondition::dump(temp_str, cond);
            return 1;
        }
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
        dcmtkLogger::info("Peer Aborted Association");
    }
    else
    {
        dcmtkLogger::errorStream() << "Move SCU Failed: " << DimseCondition::dump(temp_str, cond);
        dcmtkLogger::info("Aborting Association");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            dcmtkLogger::errorStream() << "Association Abort Failed: " << DimseCondition::dump(temp_str, cond);
            return 1;
        }
    }

    cond = ASC_destroyAssociation(&assoc);
    if (cond.bad()) {
        dcmtkLogger::error(DimseCondition::dump(temp_str, cond));
        return 1;
    }
    cond = ASC_dropNetwork(&net);
    if (cond.bad()) {
        dcmtkLogger::error(DimseCondition::dump(temp_str, cond));
        return 1;
    }

    return 0;
}

//---------------------------------------------------------------------------------------------

void dcmtkBaseScu::clearAllQueryAttributes()
{
    dcmtkLogger::warning("Base class called, concrete seems not implemented!");
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkBaseScu::addPresentationContext(T_ASC_Parameters *params,
                                                T_ASC_PresentationContextID pid,
                                                const char* abstractSyntax,
                                                E_TransferSyntax preferredTransferSyntax)
{
    const char* transferSyntaxes[] = { NULL, NULL, NULL };
    int numTransferSyntaxes = 0;

    switch (preferredTransferSyntax) {
    case EXS_LittleEndianImplicit:
        /* we only support Little Endian Implicit */
        transferSyntaxes[0]  = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 1;
        break;
    case EXS_LittleEndianExplicit:
        /* we prefer Little Endian Explicit */
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
    case EXS_BigEndianExplicit:
        /* we prefer Big Endian Explicit */
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
    default:
        /* We prefer explicit transfer syntaxes.
         * If we are running on a Little Endian machine we prefer
         * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
         */
        if (gLocalByteOrder == EBO_LittleEndian)  /* defined in dcxfer.h */
        {
            transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        } else {
            transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        }
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
    }

    return ASC_addPresentationContext(
        params, pid, abstractSyntax,
        transferSyntaxes, numTransferSyntaxes);
}

//---------------------------------------------------------------------------------------------