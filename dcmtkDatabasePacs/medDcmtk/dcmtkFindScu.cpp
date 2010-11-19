#include "dcmtkFindScu.h"

#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdicent.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcpath.h"
#include "dcmtk/ofstd/ofconapp.h"

#include "dcmtkResultDataset.h"
#include "dcmtkFindCallback.h"
#include "dcmtkLogger.h"

#include "dcmtkNode.h"

//---------------------------------------------------------------------------------------------

dcmtkFindScu::dcmtkFindScu()
{

    opt_abortAssociation = OFFalse;
    opt_acse_timeout = 30;
    opt_blockMode = DIMSE_BLOCKING;
    opt_cancelAfterNResponses = -1;
    opt_dimse_timeout = 0;
    opt_extractResponsesToFile = OFFalse;
    opt_maxReceivePDULength = ASC_DEFAULTMAXPDU;
    opt_networkTransferSyntax = EXS_Unknown;
    opt_repeatCount = 1;
    opt_secureConnection = OFFalse; /* default: no secure connection */
    opt_abstractSyntax = UID_FINDStudyRootQueryRetrieveInformationModel; 
    findCallback = NULL;
}

//---------------------------------------------------------------------------------------------

dcmtkFindScu::~dcmtkFindScu()
{

}

//---------------------------------------------------------------------------------------------

int dcmtkFindScu::sendFindRequest(const char* peerTitle, const char* peerIP, unsigned int peerPort, 
                                  const char* ourTitle, const char* ourIP, unsigned int ourPort)
{
    emit progressed(50);

    opt_ourTitle = ourTitle;
    opt_peer = peerIP;
    opt_peerTitle = peerTitle;
    opt_port = peerPort;


    /*
    ** Don't let dcmdata remove tailing blank padding or perform other
    ** maipulations.  We want to see the real data.
    */
    dcmEnableAutomaticInputDataCorrection.set(OFFalse);

    if (overrideKeys.empty())
    {
        dcmtkLogger::errorStream() << "No keys have been added!";
    }


    // initialize network
    OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, opt_acse_timeout, &net);
    if (cond.bad()) 
    {
        dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
        return 1;
    }


    // add node to result container
    dcmtkNode* node = new dcmtkNode;
    node->setTitle(peerTitle);
    node->setIp(peerIP);
    node->setPort(peerPort);
    
    //own callback
    findCallback = new dcmtkFindScuCallback(opt_extractResponsesToFile, opt_cancelAfterNResponses);
    findCallback->setResultDatasetContainer(node->getResultDatasetContainer());
    findCallback->setKeyContainer(&m_keyContainer);

    // do the main work: negotiate network association, perform C-FIND transaction,
    // process results, and finally tear down the association.
    cond = performQuery(
      opt_peer,
      opt_port,
      opt_ourTitle,
      opt_peerTitle,
      opt_abstractSyntax,
      opt_networkTransferSyntax,
      opt_blockMode,
      opt_dimse_timeout,
      opt_maxReceivePDULength,
      opt_secureConnection,
      opt_abortAssociation,
      opt_repeatCount,
      opt_extractResponsesToFile,
      opt_cancelAfterNResponses,
      &overrideKeys,     
      findCallback,
      &fileNameList);

    m_resContainer.add(node);

    if (cond.bad()) dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);

    // destroy network structure
    cond = ASC_dropNetwork(&net);

    if (cond.bad()) dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);

    emit progressed(100);
    emit finished();

    delete findCallback;
    findCallback = NULL;
    return 0;
}

//---------------------------------------------------------------------------------------------

int dcmtkFindScu::sendFindRequest()
{
    return this->sendFindRequest(opt_peerTitle, opt_peer, opt_port, 
                                 opt_ourTitle, opt_ourIP, opt_retrievePort);
}

//---------------------------------------------------------------------------------------------

bool dcmtkFindScu::processQueryAttribute(const char* key)
{
    OFString ovKey(key);
    overrideKeys.push_back(ovKey);
    return true;
}

//---------------------------------------------------------------------------------------------

void dcmtkFindScu::clearAllQueryAttributes()
{
    overrideKeys.clear();
    m_resContainer.clear();
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkFindScu::performQuery(
    const char *peer,
    unsigned int port,
    const char *ourTitle,
    const char *peerTitle,
    const char *abstractSyntax,
    E_TransferSyntax preferredTransferSyntax,
    T_DIMSE_BlockingMode blockMode,
    int dimse_timeout,
    Uint32 maxReceivePDULength,
    OFBool secureConnection,
    OFBool abortAssociation,
    unsigned int repeatCount,
    OFBool extractResponsesToFile,
    int cancelAfterNResponses,
    OFList<OFString> *overrideKeys,
    DcmFindSCUCallback *callback,
    OFList<OFString> *fileNameList)
{
    T_ASC_Association *assoc = NULL;
    T_ASC_Parameters *params = NULL;
    DIC_NODENAME localHost;
    DIC_NODENAME peerHost;
    OFString temp_str;

    /* initialize asscociation parameters, i.e. create an instance of T_ASC_Parameters*. */
    OFCondition cond = ASC_createAssociationParameters(&params, maxReceivePDULength);
    if (cond.bad()) return cond;

    /* sets this application's title and the called application's title in the params */
    /* structure. The default values to be set here are "STORESCU" and "ANY-SCP". */
    ASC_setAPTitles(params, ourTitle, peerTitle, NULL);

    /* Set the transport layer type (type of network connection) in the params */
    /* structure. The default is an insecure connection; where OpenSSL is  */
    /* available the user is able to request an encrypted,secure connection. */
    cond = ASC_setTransportLayerType(params, secureConnection);
    if (cond.bad()) return cond;

    /* Figure out the presentation addresses and copy the */
    /* corresponding values into the association parameters.*/
    gethostname(localHost, sizeof(localHost) - 1);
    sprintf(peerHost, "%s:%d", peer, (int)port);
    ASC_setPresentationAddresses(params, localHost, peerHost);

    /* Set the presentation contexts which will be negotiated */
    /* when the network connection will be established */
    cond = addPresentationContext(params, 1, abstractSyntax, preferredTransferSyntax);

    if (cond.bad()) return cond;

    /* dump presentation contexts if required */
    dcmtkLogger::debugStream() << "Request Parameters:";
        dcmtkLogger::debugStream() << ASC_dumpParameters(temp_str, params, ASC_ASSOC_RQ);

    /* create association, i.e. try to establish a network connection to another */
    /* DICOM application. This call creates an instance of T_ASC_Association*. */
    dcmtkLogger::infoStream() << "Requesting Association";

    cond = ASC_requestAssociation(net, params, &assoc);

    if (cond.bad()) {
        if (cond == DUL_ASSOCIATIONREJECTED) {
            T_ASC_RejectParameters rej;
            ASC_getRejectParameters(params, &rej);

            dcmtkLogger::errorStream() << "Association Rejected:";
                dcmtkLogger::errorStream() << ASC_printRejectParameters(temp_str, &rej);
            return cond;
        } else {
            dcmtkLogger::errorStream() << "Association Request Failed: " << DimseCondition::dump(temp_str, cond);
            return cond;
        }
    }

    /* dump the presentation contexts which have been accepted/refused */
    dcmtkLogger::debugStream() << "Association Parameters Negotiated:";
        dcmtkLogger::debugStream() << ASC_dumpParameters(temp_str, params, ASC_ASSOC_AC);

    /* count the presentation contexts which have been accepted by the SCP */
    /* If there are none, finish the execution */
    if (ASC_countAcceptedPresentationContexts(params) == 0) {
        dcmtkLogger::errorStream() << "No Acceptable Presentation Contexts";
        return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
    }

    /* dump general information concerning the establishment of the network connection if required */
    dcmtkLogger::infoStream() << "Association Accepted (Max Send PDV: " << assoc->sendPDVLength << ")";

    /* do the real work, i.e. for all files which were specified in the command line, send a */
    /* C-FIND-RQ to the other DICOM application and receive corresponding response messages. */
    cond = EC_Normal;
    if ((fileNameList == NULL) || fileNameList->empty())
    {
        /* no files provided on command line */
        cond = findSCU(assoc, NULL, repeatCount, abstractSyntax, blockMode, dimse_timeout, extractResponsesToFile, cancelAfterNResponses, overrideKeys, callback);
    } else {
      OFListIterator(OFString) iter = fileNameList->begin();
      OFListIterator(OFString) enditer = fileNameList->end();
      while ((iter != enditer) && cond.good())
      {
          cond = findSCU(assoc, (*iter).c_str(), repeatCount, abstractSyntax, blockMode, dimse_timeout, extractResponsesToFile, cancelAfterNResponses, overrideKeys, callback);
          ++iter;
      }
    }

    /* tear down association, i.e. terminate network connection to SCP */
    if (cond == EC_Normal)
    {
        if (abortAssociation) {
            dcmtkLogger::infoStream() << "Aborting Association";
            cond = ASC_abortAssociation(assoc);
            if (cond.bad()) {
                dcmtkLogger::errorStream() << "Association Abort Failed: " << DimseCondition::dump(temp_str, cond);
                return cond;
            }
        } else {
            /* release association */
            dcmtkLogger::infoStream() << "Releasing Association";
            cond = ASC_releaseAssociation(assoc);
            if (cond.bad())
            {
                dcmtkLogger::errorStream() << "Association Release Failed: " << DimseCondition::dump(temp_str, cond);
                return cond;
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
            return cond;
        }
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
        dcmtkLogger::infoStream() << "Peer Aborted Association";
    }
    else
    {
        dcmtkLogger::errorStream() << "Find SCU Failed: " << DimseCondition::dump(temp_str, cond);
        dcmtkLogger::infoStream() << "Aborting Association";
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            dcmtkLogger::errorStream() << "Association Abort Failed: " << DimseCondition::dump(temp_str, cond);
            return cond;
        }
    }

    /* destroy the association, i.e. free memory of T_ASC_Association* structure. This */
    /* call is the counterpart of ASC_requestAssociation(...) which was called above. */
    cond = ASC_destroyAssociation(&assoc);
    return cond;
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkFindScu::findSCU( T_ASC_Association * assoc,
                                    const char *fname,
                                    int repeatCount,
                                    const char *abstractSyntax,
                                    T_DIMSE_BlockingMode blockMode,
                                    int dimse_timeout,
                                    OFBool extractResponsesToFile,
                                    int cancelAfterNResponses,
                                    OFList<OFString> *overrideKeys,
                                    DcmFindSCUCallback *callback) const
{
     /*
     * This function will read all the information from the given file
     * (this information specifies a search mask), figure out a corresponding
     * presentation context which will be used to transmit a C-FIND-RQ message
     * over the network to the SCP, and it will finally initiate the transmission
     * of data to the SCP.
     *
     * Parameters:
     *   assoc - [in] The association (network connection to another DICOM application).
     *   fname - [in] Name of the file which shall be processed.
     */

    T_ASC_PresentationContextID presId;
    T_DIMSE_C_FindRQ req;
    T_DIMSE_C_FindRSP rsp;
    DcmFileFormat dcmff;
    OFString temp_str;

    /* if there is a valid filename */
    OFCondition cond;
    if (fname != NULL) {

        /* read information from file (this information specifies a search mask). After the */
        /* call to DcmFileFormat::read(...) the information which is encapsulated in the file */
        /* will be available through the DcmFileFormat object. In detail, it will be available */
        /* through calls to DcmFileFormat::getMetaInfo() (for meta header information) and */
        /* DcmFileFormat::getDataset() (for data set information). */
        cond = dcmff.loadFile(fname);

        /* figure out if an error occured while the file was read*/
        if (cond.bad()) {
            dcmtkLogger::errorStream() << "Bad DICOM file: " << fname << ": " << cond.text();
            return cond;
        }
    }

    /* replace specific keys by those in overrideKeys */
    OFListConstIterator(OFString) path = overrideKeys->begin();
    OFListConstIterator(OFString) endOfList = overrideKeys->end();
    DcmDataset* dset = dcmff.getDataset();
    DcmPathProcessor proc;
    /* disable item wildcards since they don't make sense for Q/R or worklist
       where always a single item is sent in the query. Further, do not check
       for private reservations in query dataset.
     */
    proc.setItemWildcardSupport(OFFalse);
    proc.checkPrivateReservations(OFFalse);
    while (path != endOfList)
    {
        cond = proc.applyPathWithValue(dset, *path);
        if (cond.bad())
        {
            dcmtkLogger::errorStream() << "Bad override key/path: " << *path << ": " << cond.text();
            return cond;
        }
        path++;
    }

    /* figure out which of the accepted presentation contexts should be used */
    presId = ASC_findAcceptedPresentationContextID(assoc, abstractSyntax);

    if (presId == 0) {
        dcmtkLogger::errorStream() << "No presentation context";
        return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
    }

    /* repeatCount specifies how many times a certain file shall be processed */
    int n = repeatCount;

    /* prepare C-FIND-RQ message */
    bzero((char*)&req, sizeof(req));
    strcpy(req.AffectedSOPClassUID, abstractSyntax);
    req.DataSetType = DIMSE_DATASET_PRESENT;
    req.Priority = DIMSE_PRIORITY_LOW;

    /* prepare the callback data */
    DcmFindSCUDefaultCallback defaultCallback(extractResponsesToFile, cancelAfterNResponses);
    if (callback == NULL) callback = &defaultCallback;
    callback->setAssociation(assoc);
    callback->setPresentationContextID(presId);

    /* as long as no error occured and the counter does not equal 0 */
    cond = EC_Normal;
    while (cond.good() && n--)
    {
        DcmDataset *statusDetail = NULL;

        /* complete preparation of C-FIND-RQ message */
        req.MessageID = assoc->nextMsgID++;

        /* if required, dump some more general information */
        dcmtkLogger::infoStream() << DIMSE_dumpMessage(temp_str, req, DIMSE_OUTGOING, NULL, presId);
        dcmtkLogger::infoStream() << "Find SCU Request Identifiers:";
            dcmtkLogger::infoStream() << DcmObject::PrintHelper(*dset);

        /* finally conduct transmission of data */
        OFCondition cond = DIMSE_findUser(assoc, presId, &req, dset,
            progressCallback, callback, blockMode, dimse_timeout,
            &rsp, &statusDetail);

        /* dump some more general information */
        if (cond.good()) {
            dcmtkLogger::infoStream() << DIMSE_dumpMessage(temp_str, rsp, DIMSE_INCOMING);
            if (rsp.DimseStatus != STATUS_Success) {
                    dcmtkLogger::errorStream() << "Find Response: " << DU_cfindStatusString(rsp.DimseStatus);
            }
        } else {
            if (fname) {
                dcmtkLogger::errorStream() << "Find Failed, file: " << fname << ":";
            } else {
                dcmtkLogger::errorStream() << "Find Failed, query keys:";
                dcmtkLogger::errorStream() << DcmObject::PrintHelper(dcmff);
            }
            dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
        }

        /* dump status detail information if there is some */
        if (statusDetail != NULL) {
            dcmtkLogger::warningStream() << "Status Detail:";
                 dcmtkLogger::warningStream() << DcmObject::PrintHelper(*statusDetail);
            delete statusDetail;
        }
    }

    /* return */
    return cond;
}

//---------------------------------------------------------------------------------------------

void dcmtkFindScu::progressCallback(void *callbackData,
                                    T_DIMSE_C_FindRQ *request,
                                    int responseCount,
                                    T_DIMSE_C_FindRSP *rsp,
                                    DcmDataset *responseIdentifiers)
{
    DcmFindSCUDefaultCallback *callback = OFreinterpret_cast(DcmFindSCUDefaultCallback *, callbackData);
    if (callback) callback->callback(request, responseCount, rsp, responseIdentifiers);
}

//---------------------------------------------------------------------------------------------

void dcmtkFindScu::run()
{
    sendFindRequest();
}

//---------------------------------------------------------------------------------------------

void dcmtkFindScu::sendCancelRequest()
{
    if (findCallback) 
        findCallback->sendCancelRequest();
}

//---------------------------------------------------------------------------------------------
