#include "dcmtkStoreScu.h"

#include <iostream>
#include <algorithm>
#include <iterator>

#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmnet/dicom.h"      /* for DICOM_APPLICATION_REQUESTOR */
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmnet/dcasccfg.h"   /* for class DcmAssociationConfiguration */
#include "dcmtk/dcmnet/dcasccff.h"   /* for class DcmAssociationConfigurationFile */
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcuid.h"     /* for dcmtk version name */
#include "dcmtk/dcmdata/dcostrmz.h"  /* for dcmZlibCompressionLevel */

#include "dcmtkLogger.h"


//---------------------------------------------------------------------------------------------

dcmtkStoreScu::dcmtkStoreScu()
{
    opt_identMode = ASC_USER_IDENTITY_NONE;
    opt_identResponse = OFFalse;
    opt_showPresentationContexts = OFFalse;
    opt_abortAssociation = OFFalse;
    opt_maxReceivePDULength = ASC_DEFAULTMAXPDU;
    opt_maxSendPDULength = 0;
    opt_networkTransferSyntax = EXS_Unknown;
    opt_readMode = ERM_autoDetect;

    opt_scanDir = OFFalse;
    opt_recurse = OFFalse;
    const char *opt_scanPattern = "";

    opt_haltOnUnsuccessfulStore = OFTrue;
    unsuccessfulStoreEncountered = OFFalse;
    lastStatusCode = STATUS_Success;

    opt_proposeOnlyRequiredPresentationContexts = OFFalse;
    opt_combineProposedTransferSyntaxes = OFFalse;

    opt_repeatCount = 1;
    opt_inventPatientCount = 25;
    opt_inventStudyCount = 50;
    opt_inventSeriesCount = 100;
    opt_inventSOPInstanceInformation = OFFalse;
    opt_correctUIDPadding = OFFalse;
    patientNamePrefix = "";   // PatientName is PN (maximum 16 chars)
    patientIDPrefix = "PID_"; // PatientID is LO (maximum 64 chars)
    studyIDPrefix = "SID_";   // StudyID is SH (maximum 16 chars)
    accessionNumberPrefix;   // AccessionNumber is SH (maximum 16 chars)
    opt_secureConnection = OFFalse; /* default: no secure connection */
    opt_configFile = NULL;
    opt_profileName = NULL;

    opt_blockMode = DIMSE_BLOCKING;
    opt_dimse_timeout = 0;
    opt_acse_timeout = 30;
    opt_directory = "";
}

//---------------------------------------------------------------------------------------------

dcmtkStoreScu::~dcmtkStoreScu()
{

}

//---------------------------------------------------------------------------------------------

void dcmtkStoreScu::run()
{
    this->sendStoreRequest();
}

//---------------------------------------------------------------------------------------------

bool dcmtkStoreScu::setScanDirectory(const char* dir)
{
    std::string fullPath(dir);
    // unfortunately we need to convert slashes to backslashes on windows and vice versa on linux
#ifdef HAVE_WINDOWS_H
    std::replace(fullPath.begin(),fullPath.end(),'/','\\');
    std::cout << fullPath << std::endl;
#else
    std::replace(fullPath.begin(),fullPath.end(),'\\','/');
#endif
   
    OFString directory(fullPath.c_str());
    
    // test if the directory exists:
    if (!OFStandard::dirExists(directory))
        return false;

    opt_scanDir = true;
    opt_directory = fullPath.c_str();
    return true;

}

//---------------------------------------------------------------------------------------------

int dcmtkStoreScu::sendStoreRequest(const char* peerTitle, const char* peerIP, int peerPort, 
                        const char* ourTitle, const char* ourIP, int ourPort)
{
  OFOStringStream optStream;

  opt_peer = peerIP;
  opt_port = peerPort;
  opt_peerTitle = peerTitle;
  opt_ourTitle = ourTitle;

  OFList<OFString> fileNameList;       // list of files to transfer to SCP
  OFList<OFString> sopClassUIDList;    // the list of SOP classes
  OFList<OFString> sopInstanceUIDList; // the list of SOP instances

  DIC_NODENAME localHost;
  DIC_NODENAME peerHost;
  T_ASC_Association *assoc;
  DcmAssociationConfiguration asccfg;  // handler for association configuration profiles

  OFString temp_str;



    /* create list of input files */
    dcmtkLogger::infoStream() << "determining input files ...";
    
    /* iterate over all input filenames/directories */    
    if (OFStandard::dirExists(opt_directory))
      {
        if (opt_scanDir)
            OFStandard::searchDirectoryRecursively(opt_directory, inputFiles, "", "" /*dirPrefix*/, opt_recurse);
        else
            dcmtkLogger::warningStream() << "This is not a valid directory: " << opt_directory;
      } 
  
    /* check whether there are any input files at all */
    if (inputFiles.empty())
    {
      dcmtkLogger::errorStream() << "no input files to be sent";
      return 1;
    }

    /* check input files */
    OFString errormsg;
    DcmFileFormat dfile;
    char sopClassUID[128];
    char sopInstanceUID[128];
    OFBool ignoreName;
    const char *currentFilename = NULL;
    OFListIterator(OFString) if_iter = inputFiles.begin();
    OFListIterator(OFString) if_last = inputFiles.end();
    
    dcmtkLogger::infoStream() << "checking input files ...";
    /* iterate over all input filenames */
    while (if_iter != if_last)
    {
      ignoreName = OFFalse;
      currentFilename = (*if_iter).c_str();
      if (OFStandard::fileExists(currentFilename))
      {
        if (opt_proposeOnlyRequiredPresentationContexts)
        {
          if (!findSOPClassAndInstanceInFile(currentFilename, sopClassUID, sopInstanceUID))
          {
            ignoreName = OFTrue;
            errormsg = "missing SOP class (or instance) in file: ";
            errormsg += currentFilename;
            if (opt_haltOnUnsuccessfulStore)
            {
              dcmtkLogger::errorStream() << errormsg;
              return 1;
            }
            else
              dcmtkLogger::warningStream() << errormsg << ", ignoring file";
          }
          else if (!dcmIsaStorageSOPClassUID(sopClassUID))
          {
            ignoreName = OFTrue;
            errormsg = "unknown storage SOP class in file: ";
            errormsg += currentFilename;
            errormsg += ": ";
            errormsg += sopClassUID;
            if (opt_haltOnUnsuccessfulStore)
            {
              dcmtkLogger::errorStream() << errormsg;
              return 1;
            }
            else
              dcmtkLogger::warningStream() << errormsg << ", ignoring file";
          }
          else
          {
            sopClassUIDList.push_back(sopClassUID);
            sopInstanceUIDList.push_back(sopInstanceUID);
          }
        }
        if (!ignoreName) fileNameList.push_back(currentFilename);
      }
      else
      {
        errormsg = "cannot access file: ";
        errormsg += currentFilename;
        if (opt_haltOnUnsuccessfulStore)
        {
          dcmtkLogger::errorStream() << errormsg;
          return 1;
        }
        else
          dcmtkLogger::warningStream() << errormsg << ", ignoring file";
      }
      ++if_iter;
    }


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

    /* Configure User Identity Negotiation*/
    if (opt_identMode != ASC_USER_IDENTITY_NONE)
    {
      cond = configureUserIdentityRequest(params);
      if (cond.bad())
        return 1;
    }

    if (opt_profileName)
    {
      /* perform name mangling for config file key */
      OFString sprofile;
      const char *c = opt_profileName;
      while (*c)
      {
        if (!isspace(*c)) sprofile += (char) (toupper(*c));
        ++c;
      }

      /* set presentation contexts as defined in config file */
      cond = asccfg.setAssociationParameters(sprofile.c_str(), *params);
    }
    else
    {
      /* Set the presentation contexts which will be negotiated */
      /* when the network connection will be established */
      cond = addStoragePresentationContexts(params, sopClassUIDList);
    }

    if (cond.bad()) {
      dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
      return 1;
    }

    /* dump presentation contexts if required */
    if (opt_showPresentationContexts)
    {
      dcmtkLogger::infoStream() << "Request Parameters:";
          dcmtkLogger::infoStream() << ASC_dumpParameters(temp_str, params, ASC_ASSOC_RQ);
    }
    else
    {
      dcmtkLogger::debugStream() << "Request Parameters:";
        dcmtkLogger::debugStream() << ASC_dumpParameters(temp_str, params, ASC_ASSOC_RQ);
    }

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
        return 1;
      } else {
        dcmtkLogger::errorStream() << "Association Request Failed: " << DimseCondition::dump(temp_str, cond);
        return 1;
      }
    }

    /* dump the connection parameters if in debug mode*/
    dcmtkLogger::debugStream() << ASC_dumpConnectionParameters(temp_str, assoc);

    /* dump the presentation contexts which have been accepted/refused */
    if (opt_showPresentationContexts)
    {
      dcmtkLogger::infoStream() << "Association Parameters Negotiated:";
        dcmtkLogger::infoStream() << ASC_dumpParameters(temp_str, params, ASC_ASSOC_AC);
    }
    else
    {
      dcmtkLogger::debugStream() << "Association Parameters Negotiated:";
        dcmtkLogger::debugStream() << ASC_dumpParameters(temp_str, params, ASC_ASSOC_AC);
    }

    /* count the presentation contexts which have been accepted by the SCP */
    /* If there are none, finish the execution */
    if (ASC_countAcceptedPresentationContexts(params) == 0) {
      dcmtkLogger::errorStream() << "No Acceptable Presentation Contexts";
      return 1;
    }

    /* check user authentication response (if applicable) */
    cond = checkUserIdentityResponse(params);
    if (cond.bad())
    {
      dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
      return 1;
    }
    /* dump general information concerning the establishment of the network connection if required */
    dcmtkLogger::infoStream() << "Association Accepted (Max Send PDV: " << assoc->sendPDVLength << ")";

    /* do the real work, i.e. for all files which were specified in the */
    /* command line, transmit the encapsulated DICOM objects to the SCP. */
    cond = EC_Normal;
    OFListIterator(OFString) iter = fileNameList.begin();
    OFListIterator(OFString) enditer = fileNameList.end();

    while ((iter != enditer) && cond.good())
    {
      cond = cstore(assoc, *iter);
      ++iter;
    }

    /* tear down association, i.e. terminate network connection to SCP */
    if (cond == EC_Normal)
    {
      if (opt_abortAssociation) {
        dcmtkLogger::infoStream() << "Aborting Association";
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
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
      dcmtkLogger::errorStream() << "Store SCU Failed: " << DimseCondition::dump(temp_str, cond);
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


    int exitCode = 0;
    if (opt_haltOnUnsuccessfulStore && unsuccessfulStoreEncountered) {
      if (lastStatusCode == STATUS_Success) {
        // there must have been some kind of general network error
        exitCode = 0xff;
      } else {
        exitCode = (lastStatusCode >> 8); // only the least significant byte is relevant as exit code
      }
    }

    return exitCode;
}

//---------------------------------------------------------------------------------------------

int dcmtkStoreScu::sendStoreRequest()
{
    return this->sendStoreRequest(opt_peerTitle, opt_peer, opt_port, 
                                  opt_ourTitle, opt_ourIP, opt_retrievePort);
}

//---------------------------------------------------------------------------------------------

OFBool dcmtkStoreScu::isaListMember(OFList<OFString> &lst, OFString &s)
{
  OFListIterator(OFString) cur = lst.begin();
  OFListIterator(OFString) end = lst.end();

  OFBool found = OFFalse;
  while (cur != end && !found) {
    found = (s == *cur);
    ++cur;
  }

  return found;
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkStoreScu::addPresentationContext(T_ASC_Parameters *params, int presentationContextId,
                                                  const OFString &abstractSyntax, const OFString &transferSyntax,
                                                  T_ASC_SC_ROLE proposedRole)
{
  const char *c_p = transferSyntax.c_str();
  OFCondition cond = ASC_addPresentationContext(params, presentationContextId,
    abstractSyntax.c_str(), &c_p, 1, proposedRole);
  return cond;
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkStoreScu::addPresentationContext(T_ASC_Parameters *params, int presentationContextId,
                                                  const OFString &abstractSyntax, const OFList<OFString> &transferSyntaxList,
                                                  T_ASC_SC_ROLE proposedRole)
{
  // create an array of supported/possible transfer syntaxes
  const char **transferSyntaxes = new const char*[transferSyntaxList.size()];
  int transferSyntaxCount = 0;
  OFListConstIterator(OFString) s_cur = transferSyntaxList.begin();
  OFListConstIterator(OFString) s_end = transferSyntaxList.end();
  while (s_cur != s_end) {
    transferSyntaxes[transferSyntaxCount++] = (*s_cur).c_str();
    ++s_cur;
  }

  OFCondition cond = ASC_addPresentationContext(params, presentationContextId,
    abstractSyntax.c_str(), transferSyntaxes, transferSyntaxCount, proposedRole);

  delete[] transferSyntaxes;
  return cond;
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkStoreScu::addStoragePresentationContexts(T_ASC_Parameters *params,
                                                          OFList<OFString> &sopClasses)
{
  /*
   * Each SOP Class will be proposed in two presentation contexts (unless
   * the opt_combineProposedTransferSyntaxes global variable is true).
   * The command line specified a preferred transfer syntax to use.
   * This prefered transfer syntax will be proposed in one
   * presentation context and a set of alternative (fallback) transfer
   * syntaxes will be proposed in a different presentation context.
   *
   * Generally, we prefer to use Explicitly encoded transfer syntaxes
   * and if running on a Little Endian machine we prefer
   * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
   * Some SCP implementations will just select the first transfer
   * syntax they support (this is not part of the standard) so
   * organise the proposed transfer syntaxes to take advantage
   * of such behaviour.
   */

  // Which transfer syntax was preferred on the command line
  OFString preferredTransferSyntax;
  if (opt_networkTransferSyntax == EXS_Unknown) {
    /* gLocalByteOrder is defined in dcxfer.h */
    if (gLocalByteOrder == EBO_LittleEndian) {
      /* we are on a little endian machine */
      preferredTransferSyntax = UID_LittleEndianExplicitTransferSyntax;
    } else {
      /* we are on a big endian machine */
      preferredTransferSyntax = UID_BigEndianExplicitTransferSyntax;
    }
  } else {
    DcmXfer xfer(opt_networkTransferSyntax);
    preferredTransferSyntax = xfer.getXferID();
  }

  OFListIterator(OFString) s_cur;
  OFListIterator(OFString) s_end;

  OFList<OFString> fallbackSyntaxes;
  // - If little endian implicit is preferred, we don't need any fallback syntaxes
  //   because it is the default transfer syntax and all applications must support it.
  // - If MPEG2 MP@ML is preferred, we don't want to propose any fallback solution
  //   because this is not required and we cannot decompress the movie anyway.
  if ((opt_networkTransferSyntax != EXS_LittleEndianImplicit) &&
      (opt_networkTransferSyntax != EXS_MPEG2MainProfileAtMainLevel))
  {
    fallbackSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
    fallbackSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
    fallbackSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
    // Remove the preferred syntax from the fallback list
    fallbackSyntaxes.remove(preferredTransferSyntax);
  }

  // create a list of transfer syntaxes combined from the preferred and fallback syntaxes
  OFList<OFString> combinedSyntaxes;
  s_cur = fallbackSyntaxes.begin();
  s_end = fallbackSyntaxes.end();
  combinedSyntaxes.push_back(preferredTransferSyntax);
  while (s_cur != s_end)
  {
    if (!isaListMember(combinedSyntaxes, *s_cur)) combinedSyntaxes.push_back(*s_cur);
    ++s_cur;
  }

  if (!opt_proposeOnlyRequiredPresentationContexts) {
    // add the (short list of) known storage SOP classes to the list
    // the array of Storage SOP Class UIDs comes from dcuid.h
    for (int i = 0; i < numberOfDcmShortSCUStorageSOPClassUIDs; i++)
      sopClasses.push_back(dcmShortSCUStorageSOPClassUIDs[i]);
  }

  // thin out the SOP classes to remove any duplicates
  OFList<OFString> sops;
  s_cur = sopClasses.begin();
  s_end = sopClasses.end();
  while (s_cur != s_end) {
    if (!isaListMember(sops, *s_cur)) {
      sops.push_back(*s_cur);
    }
    ++s_cur;
  }

  // add a presentations context for each SOP class / transfer syntax pair
  OFCondition cond = EC_Normal;
  int pid = 1; // presentation context id
  s_cur = sops.begin();
  s_end = sops.end();
  while (s_cur != s_end && cond.good()) {

    if (pid > 255) {
      dcmtkLogger::errorStream() << "Too many presentation contexts";
      return ASC_BADPRESENTATIONCONTEXTID;
    }

    if (opt_combineProposedTransferSyntaxes) {
      cond = addPresentationContext(params, pid, *s_cur, combinedSyntaxes);
      pid += 2;   /* only odd presentation context id's */
    } else {

      // SOP class with preferred transfer syntax
      cond = addPresentationContext(params, pid, *s_cur, preferredTransferSyntax);
      pid += 2;   /* only odd presentation context id's */

      if (fallbackSyntaxes.size() > 0) {
        if (pid > 255) {
          dcmtkLogger::errorStream() << "Too many presentation contexts";
          return ASC_BADPRESENTATIONCONTEXTID;
        }

        // SOP class with fallback transfer syntax
        cond = addPresentationContext(params, pid, *s_cur, fallbackSyntaxes);
        pid += 2; /* only odd presentation context id's */
      }
    }
    ++s_cur;
  }

  return cond;
}

//---------------------------------------------------------------------------------------------

int dcmtkStoreScu::secondsSince1970()
{
  time_t t = time(NULL);
  return (int)t;
}

//---------------------------------------------------------------------------------------------

OFString dcmtkStoreScu::intToString(int i)
{
  char numbuf[32];
  sprintf(numbuf, "%d", i);
  return numbuf;
}

//---------------------------------------------------------------------------------------------

OFString dcmtkStoreScu::makeUID(OFString basePrefix, int counter)
{
  OFString prefix = basePrefix + "." + intToString(counter);
  char uidbuf[65];
  OFString uid = dcmGenerateUniqueIdentifier(uidbuf, prefix.c_str());
  return uid;
}

//---------------------------------------------------------------------------------------------

OFBool dcmtkStoreScu::updateStringAttributeValue(DcmItem *dataset, const DcmTagKey &key, OFString &value)
{
  DcmStack stack;
  DcmTag tag(key);

  OFCondition cond = EC_Normal;
  cond = dataset->search(key, stack, ESM_fromHere, OFFalse);
  if (cond != EC_Normal) {
    dcmtkLogger::errorStream() << "updateStringAttributeValue: cannot find: " << tag.getTagName()
         << " " << key << ": " << cond.text();
    return OFFalse;
  }

  DcmElement *elem = (DcmElement *)stack.top();

  DcmVR vr(elem->ident());
  if (elem->getLength() > vr.getMaxValueLength()) {
    dcmtkLogger::errorStream() << "updateStringAttributeValue: INTERNAL ERROR: " << tag.getTagName()
         << " " << key << ": value too large (max " << vr.getMaxValueLength()
         << ") for " << vr.getVRName() << " value: " << value;
    return OFFalse;
  }

  cond = elem->putOFStringArray(value);
  if (cond != EC_Normal) {
    dcmtkLogger::errorStream() << "updateStringAttributeValue: cannot put string in attribute: " << tag.getTagName()
         << " " << key << ": " << cond.text();
    return OFFalse;
  }

  return OFTrue;
}

//---------------------------------------------------------------------------------------------

void dcmtkStoreScu::replaceSOPInstanceInformation(DcmDataset *dataset)
{
  OFCmdUnsignedInt patientCounter = 0;
  OFCmdUnsignedInt studyCounter = 0;
  OFCmdUnsignedInt seriesCounter = 0;
  OFCmdUnsignedInt imageCounter = 0;
  OFString seriesInstanceUID;
  OFString seriesNumber;
  OFString studyInstanceUID;
  OFString studyID;
  OFString accessionNumber;
  OFString patientID;
  OFString patientName;

  if (seriesInstanceUID.length() == 0) seriesInstanceUID=makeUID(SITE_SERIES_UID_ROOT, (int)seriesCounter);
  if (seriesNumber.length() == 0) seriesNumber = intToString((int)seriesCounter);
  if (studyInstanceUID.length() == 0) studyInstanceUID = makeUID(SITE_STUDY_UID_ROOT, (int)studyCounter);
  if (studyID.length() == 0) studyID = studyIDPrefix + intToString((int)secondsSince1970()) + intToString((int)studyCounter);
  if (accessionNumber.length() == 0) accessionNumber = accessionNumberPrefix + intToString(secondsSince1970()) + intToString((int)studyCounter);
  if (patientID.length() == 0) patientID = patientIDPrefix + intToString(secondsSince1970()) + intToString((int)patientCounter);
  if (patientName.length() == 0) patientName = patientNamePrefix + intToString(secondsSince1970()) + intToString((int)patientCounter);

  if (imageCounter >= opt_inventSeriesCount) {
    imageCounter = 0;
    seriesCounter++;
    seriesInstanceUID = makeUID(SITE_SERIES_UID_ROOT, (int)seriesCounter);
    seriesNumber = intToString((int)seriesCounter);
  }
  if (seriesCounter >= opt_inventStudyCount) {
    seriesCounter = 0;
    studyCounter++;
    studyInstanceUID = makeUID(SITE_STUDY_UID_ROOT, (int)studyCounter);
    studyID = studyIDPrefix + intToString(secondsSince1970()) + intToString((int)studyCounter);
    accessionNumber = accessionNumberPrefix + intToString(secondsSince1970()) + intToString((int)studyCounter);
  }
  if (studyCounter >= opt_inventPatientCount) {
    // we create as many patients as necessary */
    studyCounter = 0;
    patientCounter++;
    patientID = patientIDPrefix + intToString(secondsSince1970()) + intToString((int)patientCounter);
    patientName = patientNamePrefix + intToString(secondsSince1970()) + intToString((int)patientCounter);
  }

  OFString sopInstanceUID = makeUID(SITE_INSTANCE_UID_ROOT, (int)imageCounter);
  OFString imageNumber = intToString((int)imageCounter);

  dcmtkLogger::infoStream() << "Inventing Identifying Information ("
         << "pa" << patientCounter << ", st" << studyCounter
         << ", se" << seriesCounter << ", im" << imageCounter << "):";
  dcmtkLogger::infoStream() << "  PatientName=" << patientName;
  dcmtkLogger::infoStream() << "  PatientID=" << patientID;
  dcmtkLogger::infoStream() << "  StudyInstanceUID=" << studyInstanceUID;
  dcmtkLogger::infoStream() << "  StudyID=" << studyID;
  dcmtkLogger::infoStream() << "  SeriesInstanceUID=" << seriesInstanceUID;
  dcmtkLogger::infoStream() << "  SeriesNumber=" << seriesNumber;
  dcmtkLogger::infoStream() << "  SOPInstanceUID=" << sopInstanceUID;
  dcmtkLogger::infoStream() << "  ImageNumber=" << imageNumber;

  updateStringAttributeValue(dataset, DcmTagKey(0x0010, 0x0010), patientName);
  updateStringAttributeValue(dataset, DCM_PatientID, patientID);
  updateStringAttributeValue(dataset, DCM_StudyInstanceUID, studyInstanceUID);
  updateStringAttributeValue(dataset, DCM_StudyID, studyID);
  updateStringAttributeValue(dataset, DCM_SeriesInstanceUID, seriesInstanceUID);
  updateStringAttributeValue(dataset, DCM_SeriesNumber, seriesNumber);
  updateStringAttributeValue(dataset, DCM_SOPInstanceUID, sopInstanceUID);
  updateStringAttributeValue(dataset, DCM_InstanceNumber, imageNumber);

  imageCounter++;
}

//---------------------------------------------------------------------------------------------

void dcmtkStoreScu::progressCallback(void * /*callbackData*/, T_DIMSE_StoreProgress *progress,
                             T_DIMSE_C_StoreRQ * req)
{
  if (progress->state == DIMSE_StoreBegin)
  {
    OFString str;
    dcmtkLogger::debugStream() << DIMSE_dumpMessage(str, *req, DIMSE_OUTGOING);
  }

  // TODO find substitute here - mk
  /*
  // We can't use oflog for the pdu output, but we use a special logger for
  // generating this output. If it is set to level "INFO" we generate the
  // output, if it's set to "DEBUG" then we'll assume that there is debug output
  // generated for each PDU elsewhere.
  OFLogger progressLogger = OFLog::getLogger("MedInria storescu.progress");
  if (progressLogger.getChainedLogLevel() == OFLogger::INFO_LOG_LEVEL) {
    switch (progress->state) {
      case DIMSE_StoreBegin:
        COUT << "XMIT: "; break;
      case DIMSE_StoreEnd:
        COUT << OFendl; break;
      default:
        COUT << "."; break;
    }
    COUT.flush();
  }
  */
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkStoreScu::storeSCU(T_ASC_Association *assoc, const char *fname)
{
  DIC_US msgId = assoc->nextMsgID++;
  T_ASC_PresentationContextID presID;
  T_DIMSE_C_StoreRQ req;
  T_DIMSE_C_StoreRSP rsp;
  DIC_UI sopClass;
  DIC_UI sopInstance;
  DcmDataset *statusDetail = NULL;

  unsuccessfulStoreEncountered = OFTrue; // assumption

  dcmtkLogger::infoStream() << "Sending file: " << fname;

  /* read information from file. After the call to DcmFileFormat::loadFile(...) the information */
  /* which is encapsulated in the file will be available through the DcmFileFormat object. */
  /* In detail, it will be available through calls to DcmFileFormat::getMetaInfo() (for */
  /* meta header information) and DcmFileFormat::getDataset() (for data set information). */
  DcmFileFormat dcmff;
  OFCondition cond = dcmff.loadFile(fname, EXS_Unknown, EGL_noChange, DCM_MaxReadLength, opt_readMode);

  /* figure out if an error occured while the file was read*/
  if (cond.bad()) {
    dcmtkLogger::errorStream() << "Bad DICOM file: " << fname << ": " << cond.text();
    return cond;
  }

  /* if required, invent new SOP instance information for the current data set (user option) */
  if (opt_inventSOPInstanceInformation) {
    replaceSOPInstanceInformation(dcmff.getDataset());
  }

  /* figure out which SOP class and SOP instance is encapsulated in the file */
  if (!DU_findSOPClassAndInstanceInDataSet(dcmff.getDataset(),
    sopClass, sopInstance, opt_correctUIDPadding)) {
      dcmtkLogger::errorStream() << "No SOP Class or Instance UID in file: " << fname;
      return DIMSE_BADDATA;
  }

  /* figure out which of the accepted presentation contexts should be used */
  DcmXfer filexfer(dcmff.getDataset()->getOriginalXfer());

  /* special case: if the file uses an unencapsulated transfer syntax (uncompressed
   * or deflated explicit VR) and we prefer deflated explicit VR, then try
   * to find a presentation context for deflated explicit VR first.
   */
  if (filexfer.isNotEncapsulated() &&
    opt_networkTransferSyntax == EXS_DeflatedLittleEndianExplicit)
  {
    filexfer = EXS_DeflatedLittleEndianExplicit;
  }

  if (filexfer.getXfer() != EXS_Unknown) presID = ASC_findAcceptedPresentationContextID(assoc, sopClass, filexfer.getXferID());
  else presID = ASC_findAcceptedPresentationContextID(assoc, sopClass);
  if (presID == 0) {
    const char *modalityName = dcmSOPClassUIDToModality(sopClass);
    if (!modalityName) modalityName = dcmFindNameOfUID(sopClass);
    if (!modalityName) modalityName = "unknown SOP class";
    dcmtkLogger::errorStream() << "No presentation context for: (" << modalityName << ") " << sopClass;
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  }

  T_ASC_PresentationContext pc;
  ASC_findAcceptedPresentationContext(assoc->params, presID, &pc);
  DcmXfer netTransfer(pc.acceptedTransferSyntax);

  /* if required, dump general information concerning transfer syntaxes */
  DcmXfer fileTransfer(dcmff.getDataset()->getOriginalXfer());
  dcmtkLogger::infoStream() << "Transfer Syntax: " << dcmFindNameOfUID(fileTransfer.getXferID())
        << " -> " << dcmFindNameOfUID(netTransfer.getXferID());
  

  /* prepare the transmission of data */
  bzero((char *)&req, sizeof(req));
  req.MessageID = msgId;
  strcpy(req.AffectedSOPClassUID, sopClass);
  strcpy(req.AffectedSOPInstanceUID, sopInstance);
  req.DataSetType = DIMSE_DATASET_PRESENT;
  req.Priority = DIMSE_PRIORITY_LOW;

  /* if required, dump some more general information */
  dcmtkLogger::infoStream() << "Sending Store Request: MsgID " << msgId << ", ("
    << dcmSOPClassUIDToModality(sopClass, "OT") << ")";

  /* finally conduct transmission of data */
  cond = DIMSE_storeUser(assoc, presID, &req,
    NULL, dcmff.getDataset(), progressCallback, NULL,
    opt_blockMode, opt_dimse_timeout,
			 &rsp, &statusDetail, NULL, OFStandard::getFileSize(fname));

  /*
   * If store command completed normally, with a status
   * of success or some warning then the image was accepted.
   */
  if (cond == EC_Normal && (rsp.DimseStatus == STATUS_Success || DICOM_WARNING_STATUS(rsp.DimseStatus))) {
    unsuccessfulStoreEncountered = OFFalse;
  }

  /* remember the response's status for later transmissions of data */
  lastStatusCode = rsp.DimseStatus;

  /* dump some more general information */
  if (cond == EC_Normal)
  {
    OFString str;
    dcmtkLogger::infoStream() << "Received Store Response";
    dcmtkLogger::errorStream() << DIMSE_dumpMessage(str, rsp, DIMSE_INCOMING, NULL, presID);
  }
  else
  {
    OFString temp_str;
    dcmtkLogger::errorStream() << "Store Failed, file: " << fname << ":";
        dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
  }

  /* dump status detail information if there is some */
  if (statusDetail != NULL) {
    dcmtkLogger::debugStream() << "Status Detail:";
        dcmtkLogger::debugStream() << DcmObject::PrintHelper(*statusDetail);
    delete statusDetail;
  }
  /* return */
  return cond;
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkStoreScu::cstore(T_ASC_Association *assoc, const OFString &fname)
{
  OFCondition cond = EC_Normal;

  /* opt_repeatCount specifies how many times a certain file shall be processed */
  int n = (int)opt_repeatCount;

  /* as long as no error occured and the counter does not equal 0 */
  while ((cond.good()) && n-- && !(opt_haltOnUnsuccessfulStore && unsuccessfulStoreEncountered))
  {
    /* process file (read file, send C-STORE-RQ, receive C-STORE-RSP) */
    cond = storeSCU(assoc, fname.c_str());
  }

  // we don't want to return an error code if --no-halt was specified.
  if (!opt_haltOnUnsuccessfulStore)
  {
    cond = EC_Normal;
  }

  /* return result value */
  return cond;
}

//---------------------------------------------------------------------------------------------

OFBool dcmtkStoreScu::findSOPClassAndInstanceInFile(const char *fname, char *sopClass, 
                                                           char *sopInstance)
{
    DcmFileFormat ff;
    if (!ff.loadFile(fname, EXS_Unknown, EGL_noChange, DCM_MaxReadLength, opt_readMode).good())
        return OFFalse;

    /* look in the meta-header first */
    OFBool found = DU_findSOPClassAndInstanceInDataSet(ff.getMetaInfo(), sopClass, sopInstance, opt_correctUIDPadding);

    if (!found)
      found = DU_findSOPClassAndInstanceInDataSet(ff.getDataset(), sopClass, sopInstance, opt_correctUIDPadding);

    return found;
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkStoreScu::configureUserIdentityRequest(T_ASC_Parameters *params)
{
  OFCondition cond = EC_Normal;
  switch (opt_identMode)
  {
    case ASC_USER_IDENTITY_USER:
    {
      cond = ASC_setIdentRQUserOnly(params, opt_user, opt_identResponse);
      return cond;
    }
    case ASC_USER_IDENTITY_USER_PASSWORD:
    {
      cond = ASC_setIdentRQUserPassword(params, opt_user, opt_password, opt_identResponse);
      return cond;
    }
    case ASC_USER_IDENTITY_KERBEROS:
    case ASC_USER_IDENTITY_SAML:
    {
      OFFile identFile;
      if (!identFile.fopen(opt_identFile.c_str(), "rb"))
      {
        OFString openerror;
        identFile.getLastErrorString(openerror);
        dcmtkLogger::errorStream() << "Unable to open Kerberos or SAML file: " << openerror;
        return EC_IllegalCall;
      }
      // determine file size
      offile_off_t result = identFile.fseek(0, SEEK_END);
      if (result != 0)
        return EC_IllegalParameter;
      offile_off_t filesize = identFile.ftell();
      identFile.rewind();
      if (filesize > 65535)
      {
        dcmtkLogger::infoStream() << "Kerberos or SAML file is larger than 65535 bytes, bytes after that position are ignored";
        filesize = 65535;
      }

      char *buf = new char[(unsigned int)filesize];
      size_t bytesRead = identFile.fread(buf, 1, (size_t)filesize);
      identFile.fclose();
      if (bytesRead == 0)
      {
        dcmtkLogger::errorStream() << "Unable to read Kerberos or SAML info from file: File empty?";
        delete[] buf;
        return EC_IllegalCall;
      }
      if (opt_identMode == ASC_USER_IDENTITY_KERBEROS)
        cond = ASC_setIdentRQKerberos(params, buf, bytesRead, opt_identResponse);
      else
        cond = ASC_setIdentRQSaml(params, buf, bytesRead, opt_identResponse);
      delete[] buf;
      break;
    }
    default:
    {
      cond = EC_IllegalCall;
    }
  }
  if (cond.bad())
  {
    OFString temp_str;
    dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
  }
  return cond;
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkStoreScu::checkUserIdentityResponse(T_ASC_Parameters *params)
{
  if (params == NULL)
    return ASC_NULLKEY;

  /* So far it is only checked whether a requested, positive response was
     actually received */

  // In case we sent no user identity request, there are no checks at all
  if ((opt_identMode == ASC_USER_IDENTITY_NONE) || (!opt_identResponse))
    return EC_Normal;

  // If positive response was requested, we expect a corresponding response
  if ((opt_identMode == ASC_USER_IDENTITY_USER) || (opt_identMode == ASC_USER_IDENTITY_USER_PASSWORD))
  {
    UserIdentityNegotiationSubItemAC *rsp = params->DULparams.ackUserIdentNeg;
    if (rsp == NULL)
    {
      dcmtkLogger::errorStream() << "User Identity Negotiation failed: Positive response requested but none received";
      return ASC_USERIDENTIFICATIONFAILED;
    }
  }
  return EC_Normal;
}

//---------------------------------------------------------------------------------------------
