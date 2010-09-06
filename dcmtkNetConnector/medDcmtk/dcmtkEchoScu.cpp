#include "dcmtkEchoScu.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */

#include "dcmtkLogger.h"


//---------------------------------------------------------------------------------------------

int dcmtkEchoScu::sendEchoRequest(const char* peerTitle, const char* peerIP, int peerPort, 
                                  const char* ourTitle, const char* ourIP, int ourPort)
{
    opt_peer                = peerIP;
    opt_port                = peerPort;
    opt_peerTitle           = peerTitle;
    opt_ourTitle            = ourTitle;
    

    OFCmdUnsignedInt opt_numXferSyntaxes     = 1;    
    OFCmdUnsignedInt maxXferSyntaxes         = (OFCmdUnsignedInt)(DIM_OF(transferSyntaxes));
    OFCmdUnsignedInt opt_numPresentationCtx  = 1;
 
    DIC_NODENAME localHost;
    DIC_NODENAME peerHost;

 

    /* initialize network, i.e. create an instance of T_ASC_Network*. */
    OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, opt_acse_timeout, &net);
    if (cond.bad()) {
        dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
        return 1;
    }

    /* initialize asscociation parameters, i.e. create an instance of T_ASC_Parameters*. */
    cond = ASC_createAssociationParameters(&params, opt_maxReceivePDULength);
    if (cond.bad()) {
        dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
        return 1;
    }

    /* sets this application's title and the called application's title in the params */
    /* structure. The default values to be set here are "STORESCU" and "ANY-SCP". */
    ASC_setAPTitles(params, opt_ourTitle, opt_peerTitle, NULL);

    /* Set the transport layer type (type of network connection) in the params */
    /* strucutre. The default is an insecure connection; where OpenSSL is  */
    /* available the user is able to request an encrypted,secure connection. */
    cond = ASC_setTransportLayerType(params, opt_secureConnection);
    if (cond.bad()) {
        dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
        return 1;
    }

    /* Figure out the presentation addresses and copy the */
    /* corresponding values into the association parameters.*/
    gethostname(localHost, sizeof(localHost) - 1);
    sprintf(peerHost, "%s:%d", opt_peer, (int)opt_port);
    ASC_setPresentationAddresses(params, localHost, peerHost);

    /* Set the presentation contexts which will be negotiated */
    /* when the network connection will be established */
    int presentationContextID = 1; /* odd byte value 1, 3, 5, .. 255 */
    for (unsigned long ii=0; ii<opt_numPresentationCtx; ii++)
    {
        cond = ASC_addPresentationContext(params, presentationContextID, UID_VerificationSOPClass,
                 transferSyntaxes, (int)opt_numXferSyntaxes);
        presentationContextID += 2;
        if (cond.bad())
        {
            dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
            return 1;
        }
    }

    /* dump presentation contexts if required */
    dcmtkLogger::debugStream() <<"Request Parameters:";
        dcmtkLogger::debugStream() << ASC_dumpParameters(temp_str, params, ASC_ASSOC_RQ);

    /* create association, i.e. try to establish a network connection to another */
    /* DICOM application. This call creates an instance of T_ASC_Association*. */
    dcmtkLogger::infoStream() << "Requesting Association";
    cond = ASC_requestAssociation(net, params, &assoc);
    if (cond.bad()) {
        if (cond == DUL_ASSOCIATIONREJECTED)
        {
            T_ASC_RejectParameters rej;

            ASC_getRejectParameters(params, &rej);
            dcmtkLogger::errorStream() << "Association Rejected:";
            dcmtkLogger::errorStream() << ASC_printRejectParameters(temp_str, &rej);
            return 1;
        } else {
            dcmtkLogger::errorStream() << "Association Request Failed: " << DimseCondition::dump(temp_str, cond);
            return 1;
        }
    }

    /* dump the presentation contexts which have been accepted/refused */
    dcmtkLogger::debugStream() << "Association Parameters Negotiated:";
        dcmtkLogger::debugStream() << ASC_dumpParameters(temp_str, params, ASC_ASSOC_AC);

    /* count the presentation contexts which have been accepted by the SCP */
    /* If there are none, finish the execution */
    if (ASC_countAcceptedPresentationContexts(params) == 0) {
        dcmtkLogger::errorStream() << "No Acceptable Presentation Contexts";
        return 1;
    }

    /* dump general information concerning the establishment of the network connection if required */
    dcmtkLogger::infoStream() << "Association Accepted (Max Send PDV: " << assoc->sendPDVLength << ")";

    /* do the real work, i.e. send a number of C-ECHO-RQ messages to the DICOM application */
    /* this application is connected with and handle corresponding C-ECHO-RSP messages. */
    cond = cecho(assoc, opt_repeatCount, DIMSE_BLOCKING, opt_dimse_timeout);

    /* tear down association, i.e. terminate network connection to SCP */
    if (cond == EC_Normal)
    {
        if (opt_abortAssociation) {
            dcmtkLogger::infoStream() << "Aborting Association";
            cond = ASC_abortAssociation(assoc);
            if (cond.bad())
            {
                dcmtkLogger::errorStream() << "Association Abort Failed: " << DimseCondition::dump(temp_str, cond);
                return 1;
            }
        } else {
            /* release association */
            dcmtkLogger::infoStream() << "Releasing Association";
            cond = ASC_releaseAssociation(assoc);
            if (cond.bad())
            {
                dcmtkLogger::errorStream() << "Association Release Failed: " << DimseCondition::dump(temp_str, cond);
                return 1;
            }
        }
    }
    else if (cond == DUL_PEERREQUESTEDRELEASE)
    {
        dcmtkLogger::errorStream() << "Protocol Error: Peer requested release (Aborting)";
        dcmtkLogger::infoStream() << "Aborting Association";
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            dcmtkLogger::errorStream() << "Association Abort Failed: " << DimseCondition::dump(temp_str, cond);
            return 1;
        }
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
        dcmtkLogger::infoStream() << "Peer Aborted Association";
    }
    else
    {
        dcmtkLogger::errorStream() << "Echo SCU Failed: " << DimseCondition::dump(temp_str, cond);
        dcmtkLogger::infoStream() << "Aborting Association";
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            dcmtkLogger::errorStream() << "Association Abort Failed: " << DimseCondition::dump(temp_str, cond);
            return 1;
        }
    }

    /* destroy the association, i.e. free memory of T_ASC_Association* structure. This */
    /* call is the counterpart of ASC_requestAssociation(...) which was called above. */
    cond = ASC_destroyAssociation(&assoc);
    if (cond.bad()) {
        dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
        return 1;
    }

    /* drop the network, i.e. free memory of T_ASC_Network* structure. This call */
    /* is the counterpart of ASC_initializeNetwork(...) which was called above. */
    cond = ASC_dropNetwork(&net);
    if (cond.bad()) {
        dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
        return 1;
    }

    return 0;
}

//---------------------------------------------------------------------------------------------

int dcmtkEchoScu::sendEchoRequest()
{
    return this->sendEchoRequest(opt_peerTitle, opt_peer, opt_port, 
                                 opt_ourTitle, opt_ourIP, opt_retrievePort);
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkEchoScu::echoSCU(T_ASC_Association * assoc, T_DIMSE_BlockingMode opt_blockMode, 
                                  int opt_dimse_timeout)
{
    DIC_US msgId = assoc->nextMsgID++;
    DIC_US status;
    DcmDataset *statusDetail = NULL;

    /* dump information if required */
    dcmtkLogger::infoStream() << "Sending Echo Request: MsgID " << msgId;

    /* send C-ECHO-RQ and handle response */
    OFCondition cond = DIMSE_echoUser(assoc, msgId, opt_blockMode, opt_dimse_timeout, &status, &statusDetail);

    /* depending on if a response was received, dump some information */
    if (cond.good()) {
        dcmtkLogger::infoStream() << "Received Echo Response (Status: " << DU_cstoreStatusString(status) << ")";
    } else {
        OFString temp_str;
        dcmtkLogger::errorStream() << "Echo Failed: " << DimseCondition::dump(temp_str, cond);
    }

    /* check for status detail information, there should never be any */
    if (statusDetail != NULL) {
        dcmtkLogger::infoStream() << "Status Detail (should never be any):";
            dcmtkLogger::infoStream() << DcmObject::PrintHelper(*statusDetail);
        delete statusDetail;
    }

    /* return result value */
    return cond;
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkEchoScu::cecho(T_ASC_Association * assoc, unsigned long num_repeat, 
                                T_DIMSE_BlockingMode opt_blockMode, int opt_dimse_timeout)
{
    OFCondition cond = EC_Normal;
    unsigned long n = num_repeat;

    /* as long as no error occured and the counter does not equal 0 */
    /* send an C-ECHO-RQ and handle the response */
    while (cond.good() && n--) cond = echoSCU(assoc, opt_blockMode, opt_dimse_timeout);

    return cond;
}

//---------------------------------------------------------------------------------------------
