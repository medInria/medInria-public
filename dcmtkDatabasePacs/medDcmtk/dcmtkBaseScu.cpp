#include "dcmtkBaseScu.h"

#include "dcmtk/dcmnet/assoc.h"

#include "dcmtkResultDataset.h"
#include "dcmtkNode.h"
#include "dcmtkLogger.h"

#include <sstream>
#include <iostream>

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
    szPeerTitle = peerTitle;
    szPeerIP = peerIP;
    szOurTitle = ourTitle;
    szOurIP = ourIP;
    opt_peerTitle = szPeerTitle.c_str();
    opt_peer = szPeerIP.c_str();
    opt_port = peerPort; 
    opt_ourTitle = szOurTitle.c_str();
    opt_ourIP = szOurIP.c_str();
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
    opt_dimse_timeout =             10;
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
    m_lastCondition =               EC_Normal;
    m_assocExists =                 false;
    m_keepAssocOpen =               false;
    m_cancelRqst =                  false;
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkBaseScu::createAssociation()
{
    if (m_assocExists) return m_lastCondition;

    params = NULL;
    assoc = NULL;

    DIC_NODENAME localHost;
    DIC_NODENAME peerHost;


    QuerySyntax querySyntax[3] =  {
        { UID_FINDPatientRootQueryRetrieveInformationModel,
        UID_MOVEPatientRootQueryRetrieveInformationModel },
        { UID_FINDStudyRootQueryRetrieveInformationModel,
        UID_MOVEStudyRootQueryRetrieveInformationModel },
        { UID_FINDPatientStudyOnlyQueryRetrieveInformationModel,
        UID_MOVEPatientStudyOnlyQueryRetrieveInformationModel }
    };


    /* network for move request and responses */
    T_ASC_NetworkRole role = (opt_retrievePort > 0) ? NET_ACCEPTORREQUESTOR : NET_REQUESTOR;
    OFCondition cond = ASC_initializeNetwork(role, OFstatic_cast(int, opt_retrievePort), opt_acse_timeout, &net);
    if (cond.bad())
    {
        dcmtkLogger::errorStream() << "cannot create network: " << DimseCondition::dump(temp_str, cond);
        return cond;
    }

    /* set up main association */
    cond = ASC_createAssociationParameters(&params, opt_maxPDU);
    if (cond.bad()) {
        dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
        return cond;
    }
    ASC_setAPTitles(params, opt_ourTitle, opt_peerTitle, NULL);

    gethostname(localHost, sizeof(localHost) - 1);
    sprintf(peerHost, "%s:%d", opt_peer, (int)opt_port);
    ASC_setPresentationAddresses(params, localHost, peerHost);

    /*
    * We also add a presentation context for the corresponding
    * find sop class.
    */
    cond = addPresentationContext(params, 1,
        querySyntax[opt_queryModel].findSyntax, opt_networkTransferSyntax);

    cond = addPresentationContext(params, 3,
        querySyntax[opt_queryModel].moveSyntax, opt_networkTransferSyntax);
    if (cond.bad()) {
        dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
        return cond;
    }

    dcmtkLogger::debugStream() << "Request Parameters:"; 
    dcmtkLogger::debugStream() << ASC_dumpParameters(temp_str, params, ASC_ASSOC_RQ);

    /* create association */
    dcmtkLogger::info("Requesting Association");
    cond = ASC_requestAssociation(net, params, &assoc);
    if (cond.bad()) {
        if (cond == DUL_ASSOCIATIONREJECTED) {
            T_ASC_RejectParameters rej;

            ASC_getRejectParameters(params, &rej);
            dcmtkLogger::error("Association Rejected:");
            dcmtkLogger::error( ASC_printRejectParameters(temp_str, &rej));
            return cond;
        } else {
            dcmtkLogger::error("Association Request Failed:");
            dcmtkLogger::error( DimseCondition::dump(temp_str, cond));
            return cond;
        }
    }
    /* what has been accepted/refused ? */
    dcmtkLogger::debug("Association Parameters Negotiated:");
    dcmtkLogger::debug(ASC_dumpParameters(temp_str, params, ASC_ASSOC_AC));

    if (ASC_countAcceptedPresentationContexts(params) == 0) {
        dcmtkLogger::error( "No Acceptable Presentation Contexts");
        return cond;
    }

    dcmtkLogger::infoStream() << "Association Accepted (Max Send PDV: " << assoc->sendPDVLength << ")";

    m_lastCondition = cond;
    m_assocExists = true;

    return cond;
}

//---------------------------------------------------------------------------------------------

int dcmtkBaseScu::releaseAssociation(OFCondition &cond)
{
    // return if manual release of association is set to true
    if (m_keepAssocOpen) return 0;

    // return if no assoc exists
    if (!m_assocExists) return 0;

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

    m_assocExists = false;
    return 0;
}

//---------------------------------------------------------------------------------------------

void dcmtkBaseScu::releaseAssociation()
{
    if (m_keepAssocOpen)
    {
        m_keepAssocOpen = false;
        releaseAssociation(m_lastCondition);
        m_keepAssocOpen = true;
    }
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

dcmtkContainer<dcmtkNode*>* dcmtkBaseScu::getNodeContainer()
{
    return &m_resContainer;
}

//---------------------------------------------------------------------------------------------

bool dcmtkBaseScu::addQueryAttribute(int group, int elem, const char* value)
{
    std::stringstream groupSs;
    std::stringstream elemSs;
    groupSs << std::hex << group;
    elemSs  << std::hex << elem;

    // store in container
    dcmtkKey* keyobj = new dcmtkKey;
    keyobj->group = group;
    keyobj->elem = elem;
    keyobj->value = value;
    m_keyContainer.add(keyobj);

    return addQueryAttribute(groupSs.str().c_str(), elemSs.str().c_str(), value);

}

//---------------------------------------------------------------------------------------------

void dcmtkBaseScu::keepAssociationOpen( bool flag )
{
    m_keepAssocOpen = flag;
}

//---------------------------------------------------------------------------------------------

void dcmtkBaseScu::sendCancelRequest()
{
    m_cancelRqst = true;
}

//---------------------------------------------------------------------------------------------

