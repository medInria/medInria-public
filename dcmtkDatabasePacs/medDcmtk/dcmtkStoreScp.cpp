#include "dcmtkStoreScp.h"

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofdatime.h"
#include "dcmtk/dcmnet/dicom.h"         /* for DICOM_APPLICATION_ACCEPTOR */
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmnet/dcasccfg.h"      /* for class DcmAssociationConfiguration */
#include "dcmtk/dcmnet/dcasccff.h"      /* for class DcmAssociationConfigurationFile */
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcuid.h"        /* for dcmtk version name */
#include "dcmtk/dcmdata/dcdeftag.h"     /* for DCM_StudyInstanceUID */
#include "dcmtk/dcmdata/dcostrmz.h"     /* for dcmZlibCompressionLevel */

#include "dcmtkLogger.h"

#define PATH_PLACEHOLDER "#p"
#define FILENAME_PLACEHOLDER "#f"
#define CALLING_AETITLE_PLACEHOLDER "#a"
#define CALLED_AETITLE_PLACEHOLDER "#c"

//---------------------------------------------------------------------------------------------

dcmtkStoreScp::dcmtkStoreScp()
{
    opt_showPresentationContexts = OFFalse;
    opt_uniqueFilenames = OFFalse;
    opt_fileNameExtension;
    opt_timeNames = OFFalse;
    timeNameCounter = -1;   // "serial number" to differentiate between files with same timestamp
    opt_refuseAssociation = OFFalse;
    opt_rejectWithoutImplementationUID = OFFalse;
    opt_sleepAfter = 0;
    opt_sleepDuring = 0;
    opt_maxPDU = ASC_DEFAULTMAXPDU;
    opt_useMetaheader = OFTrue;
    opt_acceptAllXfers = OFFalse;
    opt_networkTransferSyntax = EXS_Unknown;
    opt_writeTransferSyntax = EXS_Unknown;
    opt_groupLength = EGL_recalcGL;
    opt_sequenceType = EET_ExplicitLength;
    opt_paddingType = EPD_withoutPadding;
    opt_filepad = 0;
    opt_itempad = 0;
    opt_compressionLevel = 0;
    opt_bitPreserving = OFFalse;
    opt_ignore = OFFalse;
    opt_abortDuringStore = OFFalse;
    opt_abortAfterStore = OFFalse;
    opt_promiscuous = OFFalse;
    opt_correctUIDPadding = OFFalse;
    opt_inetd_mode = OFFalse;
    callingAETitle;       // calling application entity title will be stored here
    lastCallingAETitle;
    calledAETitle;        // called application entity title will be stored here
    lastCalledAETitle;
    opt_secureConnection = OFFalse;    // default: no secure connection
    opt_outputDirectory = ".";         // default: output directory equals "."
    opt_sortStudyMode = ESM_StudyInstanceUID;      // default: no sorting
    opt_sortStudyDirPrefix = "";     // default: no directory prefix
    opt_execOnReception = NULL;        // default: don't execute anything on reception
    opt_execOnEndOfStudy = "TRUE";       // default: don't execute anything on end of study
    opt_renameOnEndOfStudy = OFFalse;  // default: don't rename any files on end of study
    opt_endOfStudyTimeout = 3;        // default: no end of study timeout
    endOfStudyThroughTimeoutEvent = OFTrue;
    opt_configFile = NULL;
    opt_profileName = NULL;
    opt_blockMode = DIMSE_BLOCKING;
    opt_dimse_timeout = 0;
    opt_acse_timeout = 30;

}

//---------------------------------------------------------------------------------------------

int dcmtkStoreScp::startService()
{
    return startService(opt_respondingAETitle, "", opt_port);
}

//---------------------------------------------------------------------------------------------

int dcmtkStoreScp::startService(const char* ourTitle, const char* ourIP, unsigned int ourPort)
{
    m_stop = false;
    opt_port = ourPort;
    opt_respondingAETitle = ourTitle;

    T_ASC_Network *net;
    DcmAssociationConfiguration asccfg;



#ifdef HAVE_GETEUID
  /* if port is privileged we must be as well */
  if (opt_port < 1024)
  {
    if (geteuid() != 0)
    {
      dcmtkLogger::errorStream() <<  "cannot listen on port " << opt_port << ", insufficient privileges";
      return 1;
    }
  }
#endif


  /* if the output directory does not equal "." (default directory) */
  if (opt_outputDirectory != ".")
  {
    /* if there is a path separator at the end of the path, get rid of it */
    OFStandard::normalizeDirName(opt_outputDirectory, opt_outputDirectory);

    /* check if the specified directory exists and if it is a directory.
     * If the output directory is invalid, dump an error message and terminate execution.
     */
    if (!OFStandard::dirExists(opt_outputDirectory))
    {
      dcmtkLogger::errorStream() << "specified output directory does not exist";
      return 1;
    }
  }

  /* check if the output directory is writeable */
  if (!OFStandard::isWriteable(opt_outputDirectory))
  {
    dcmtkLogger::errorStream() <<  "specified output directory is not writeable";
    return 1;
  }


  /* initialize network, i.e. create an instance of T_ASC_Network*. */
  OFCondition cond = ASC_initializeNetwork(NET_ACCEPTOR, OFstatic_cast(int, opt_port), opt_acse_timeout, &net);
  if (cond.bad())
  {
    dcmtkLogger::errorStream() << "cannot create network: " << DimseCondition::dump(temp_str, cond);
    return 1;
  }


#ifdef HAVE_GETUID
  /* return to normal uid so that we can't do too much damage in case
   * things go very wrong.   Only does someting if the program is setuid
   * root, and run by another user.  Running as root user may be
   * potentially disasterous if this program screws up badly.
   */
  setuid(getuid());
#endif



  while (cond.good() && (!m_stop))
  {
    /* receive an association and acknowledge or reject it. If the association was */
    /* acknowledged, offer corresponding services and invoke one or more if required. */
    cond = acceptAssociation(net, asccfg);

    // if running in inetd mode, we always terminate after one association
    if (opt_inetd_mode) break;

    // if running in multi-process mode, always terminate child after one association
    if (DUL_processIsForkedChild()) break;
  }

  /* drop the network, i.e. free memory of T_ASC_Network* structure. This call */
  /* is the counterpart of ASC_initializeNetwork(...) which was called above. */
  cond = ASC_dropNetwork(&net);
  if (cond.bad())
  {
    dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
    return 1;
  }
  
  // all good
  return 0;
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkStoreScp::acceptAssociation(T_ASC_Network *net, DcmAssociationConfiguration& asccfg)
{
  char buf[BUFSIZ];
  T_ASC_Association *assoc;
  OFCondition cond;
  OFString sprofile;
  OFString temp_str;


  const char* knownAbstractSyntaxes[] =
  {
    UID_VerificationSOPClass
  };

  const char* transferSyntaxes[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
  int numTransferSyntaxes = 0;

  // try to receive an association. Here we either want to use blocking or
  // non-blocking, depending on if the option --eostudy-timeout is set.
  if( opt_endOfStudyTimeout == -1 )
    cond = ASC_receiveAssociation(net, &assoc, opt_maxPDU, NULL, NULL, opt_secureConnection, DUL_NOBLOCK);
  else
    cond = ASC_receiveAssociation(net, &assoc, opt_maxPDU, NULL, NULL, opt_secureConnection, DUL_NOBLOCK, OFstatic_cast(int, opt_endOfStudyTimeout));

  if (cond.code() == DULC_FORKEDCHILD)
  {
    // dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
    goto cleanup;
  }

  // if some kind of error occured, take care of it
  if (cond.bad())
  {
    // check what kind of error occurred. If no association was
    // received, check if certain other conditions are met
    if( cond == DUL_NOASSOCIATIONREQUEST )
    {
      // If in addition to the fact that no association was received also option --eostudy-timeout is set
      // and if at the same time there is still a study which is considered to be open (i.e. we were actually
      // expecting to receive more objects that belong to this study) (this is the case if lastStudyInstanceUID
      // does not equal NULL), we have to consider that all objects for the current study have been received.
      // In such an "end-of-study" case, we might have to execute certain optional functions which were specified
      // by the user through command line options passed to storescp.
      if( opt_endOfStudyTimeout != -1 && !lastStudyInstanceUID.empty() )
      {
        // indicate that the end-of-study-event occured through a timeout event.
        // This knowledge will be necessary in function renameOnEndOFStudy().
        endOfStudyThroughTimeoutEvent = OFTrue;

        // before we actually execute those optional functions, we need to determine the path and name
        // of the subdirectory into which the DICOM files for the last study were written.
        lastStudySubdirectoryPathAndName = subdirectoryPathAndName;

        // now we can finally handle end-of-study events which might have to be executed
        executeEndOfStudyEvents();

        // also, we need to clear lastStudyInstanceUID to indicate
        // that the last study is not considered to be open any more.
        lastStudyInstanceUID.clear();

        // also, we need to clear subdirectoryPathAndName
        subdirectoryPathAndName.clear();

        // reset the endOfStudyThroughTimeoutEvent variable.
        endOfStudyThroughTimeoutEvent = OFFalse;
      }
    }
    // If something else was wrong we might have to dump an error message.
    else
    {
        dcmtkLogger::debugStream() << DimseCondition::dump(temp_str, cond);
    }

    // no matter what kind of error occurred, we need to do a cleanup
    goto cleanup;
  }

    dcmtkLogger::infoStream() << "Association Received";

  /* dump presentation contexts if required */
  if (opt_showPresentationContexts)
  {
    dcmtkLogger::infoStream() << "Parameters:";
        dcmtkLogger::infoStream() << ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_RQ);
  }
  else
  {
    dcmtkLogger::debugStream() << "Parameters:";
        dcmtkLogger::debugStream() << ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_RQ);
  }

  if (opt_refuseAssociation)
  {
    T_ASC_RejectParameters rej =
    {
      ASC_RESULT_REJECTEDPERMANENT,
      ASC_SOURCE_SERVICEUSER,
      ASC_REASON_SU_NOREASON
    };

    dcmtkLogger::infoStream() << "Refusing Association (forced via command line)";
    cond = ASC_rejectAssociation(assoc, &rej);
    if (cond.bad())
    {
      dcmtkLogger::errorStream() << "Association Reject Failed: " << DimseCondition::dump(temp_str, cond);
    }
    goto cleanup;
  }

  switch (opt_networkTransferSyntax)
  {
    case EXS_LittleEndianImplicit:
      /* we only support Little Endian Implicit */
      transferSyntaxes[0] = UID_LittleEndianImplicitTransferSyntax;
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
    case EXS_JPEGProcess14SV1TransferSyntax:
      /* we prefer JPEGLossless:Hierarchical-1stOrderPrediction (default lossless) */
      transferSyntaxes[0] = UID_JPEGProcess14SV1TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEGProcess1TransferSyntax:
      /* we prefer JPEGBaseline (default lossy for 8 bit images) */
      transferSyntaxes[0] = UID_JPEGProcess1TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEGProcess2_4TransferSyntax:
      /* we prefer JPEGExtended (default lossy for 12 bit images) */
      transferSyntaxes[0] = UID_JPEGProcess2_4TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEG2000LosslessOnly:
      /* we prefer JPEG2000 Lossless */
      transferSyntaxes[0] = UID_JPEG2000LosslessOnlyTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEG2000:
      /* we prefer JPEG2000 Lossy */
      transferSyntaxes[0] = UID_JPEG2000TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEGLSLossless:
      /* we prefer JPEG-LS Lossless */
      transferSyntaxes[0] = UID_JPEGLSLosslessTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEGLSLossy:
      /* we prefer JPEG-LS Lossy */
      transferSyntaxes[0] = UID_JPEGLSLossyTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_MPEG2MainProfileAtMainLevel:
      /* we prefer MPEG2 MP@ML */
      transferSyntaxes[0] = UID_MPEG2MainProfileAtMainLevelTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_RLELossless:
      /* we prefer RLE Lossless */
      transferSyntaxes[0] = UID_RLELosslessTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
#ifdef WITH_ZLIB
    case EXS_DeflatedLittleEndianExplicit:
      /* we prefer Deflated Explicit VR Little Endian */
      transferSyntaxes[0] = UID_DeflatedExplicitVRLittleEndianTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
#endif
    default:
      if (opt_acceptAllXfers)
      {
        /* we accept all supported transfer syntaxes
         * (similar to "AnyTransferSyntax" in "storescp.cfg")
         */
        transferSyntaxes[0] = UID_JPEG2000TransferSyntax;
        transferSyntaxes[1] = UID_JPEG2000LosslessOnlyTransferSyntax;
        transferSyntaxes[2] = UID_JPEGProcess2_4TransferSyntax;
        transferSyntaxes[3] = UID_JPEGProcess1TransferSyntax;
        transferSyntaxes[4] = UID_JPEGProcess14SV1TransferSyntax;
        transferSyntaxes[5] = UID_JPEGLSLossyTransferSyntax;
        transferSyntaxes[6] = UID_JPEGLSLosslessTransferSyntax;
        transferSyntaxes[7] = UID_RLELosslessTransferSyntax;
        transferSyntaxes[8] = UID_MPEG2MainProfileAtMainLevelTransferSyntax;
        transferSyntaxes[9] = UID_DeflatedExplicitVRLittleEndianTransferSyntax;
        if (gLocalByteOrder == EBO_LittleEndian)
        {
          transferSyntaxes[10] = UID_LittleEndianExplicitTransferSyntax;
          transferSyntaxes[11] = UID_BigEndianExplicitTransferSyntax;
        } else {
          transferSyntaxes[10] = UID_BigEndianExplicitTransferSyntax;
          transferSyntaxes[11] = UID_LittleEndianExplicitTransferSyntax;
        }
        transferSyntaxes[12] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 13;
      } else {
        /* We prefer explicit transfer syntaxes.
         * If we are running on a Little Endian machine we prefer
         * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
         */
        if (gLocalByteOrder == EBO_LittleEndian)  /* defined in dcxfer.h */
        {
          transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
          transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        }
        else
        {
          transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
          transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        }
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
      }
      break;
  }

  if (opt_profileName)
  {
    /* perform name mangling for config file key */
    const char *c = opt_profileName;
    while (*c)
    {
      if (!isspace(*c)) sprofile += OFstatic_cast(char, toupper(*c));
      ++c;
    }

    /* set presentation contexts as defined in config file */
    cond = asccfg.evaluateAssociationParameters(sprofile.c_str(), *assoc);
    if (cond.bad())
    {
      dcmtkLogger::debugStream() << DimseCondition::dump(temp_str, cond);
      goto cleanup;
    }
  }
  else
  {
    /* accept the Verification SOP Class if presented */
    cond = ASC_acceptContextsWithPreferredTransferSyntaxes( assoc->params, knownAbstractSyntaxes, DIM_OF(knownAbstractSyntaxes), transferSyntaxes, numTransferSyntaxes);
    if (cond.bad())
    {
      dcmtkLogger::debugStream() << DimseCondition::dump(temp_str, cond);
      goto cleanup;
    }

    /* the array of Storage SOP Class UIDs comes from dcuid.h */
    cond = ASC_acceptContextsWithPreferredTransferSyntaxes( assoc->params, dcmAllStorageSOPClassUIDs, numberOfAllDcmStorageSOPClassUIDs, transferSyntaxes, numTransferSyntaxes);
    if (cond.bad())
    {
      dcmtkLogger::debugStream() << DimseCondition::dump(temp_str, cond);
      goto cleanup;
    }

    if (opt_promiscuous)
    {
      /* accept everything not known not to be a storage SOP class */
      cond = acceptUnknownContextsWithPreferredTransferSyntaxes(
        assoc->params, transferSyntaxes, numTransferSyntaxes);
      if (cond.bad())
      {
        dcmtkLogger::debugStream() << DimseCondition::dump(temp_str, cond);
        goto cleanup;
      }
    }
  }

  /* set our app title */
  ASC_setAPTitles(assoc->params, NULL, NULL, opt_respondingAETitle);

  /* acknowledge or reject this association */
  cond = ASC_getApplicationContextName(assoc->params, buf);
  if ((cond.bad()) || strcmp(buf, UID_StandardApplicationContext) != 0)
  {
    /* reject: the application context name is not supported */
    T_ASC_RejectParameters rej =
    {
      ASC_RESULT_REJECTEDPERMANENT,
      ASC_SOURCE_SERVICEUSER,
      ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED
    };

    dcmtkLogger::infoStream() << "Association Rejected: Bad Application Context Name: " << buf;
    cond = ASC_rejectAssociation(assoc, &rej);
    if (cond.bad())
    {
      dcmtkLogger::debugStream() << DimseCondition::dump(temp_str, cond);
    }
    goto cleanup;

  }
  else if (opt_rejectWithoutImplementationUID && strlen(assoc->params->theirImplementationClassUID) == 0)
  {
    /* reject: the no implementation Class UID provided */
    T_ASC_RejectParameters rej =
    {
      ASC_RESULT_REJECTEDPERMANENT,
      ASC_SOURCE_SERVICEUSER,
      ASC_REASON_SU_NOREASON
    };

    dcmtkLogger::infoStream() << "Association Rejected: No Implementation Class UID provided";
    cond = ASC_rejectAssociation(assoc, &rej);
    if (cond.bad())
    {
      dcmtkLogger::debugStream() << DimseCondition::dump(temp_str, cond);
    }
    goto cleanup;
  }
  else
  {
    cond = ASC_acknowledgeAssociation(assoc);
    if (cond.bad())
    {
      dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
      goto cleanup;
    }
    dcmtkLogger::infoStream() << "Association Acknowledged (Max Send PDV: " << assoc->sendPDVLength << ")";
    if (ASC_countAcceptedPresentationContexts(assoc->params) == 0)
      dcmtkLogger::infoStream() << "    (but no valid presentation contexts)";
    /* dump the presentation contexts which have been accepted/refused */
    if (opt_showPresentationContexts)
      dcmtkLogger::infoStream() << ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_AC);
    else
      dcmtkLogger::debugStream() << ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_AC);
  }


  // store previous values for later use
  lastCallingAETitle = callingAETitle;
  lastCalledAETitle = calledAETitle;
  // store calling and called aetitle in global variables to enable
  // the --exec options using them. Enclose in quotation marks because
  // aetitles may contain space characters.
  DIC_AE callingTitle;
  DIC_AE calledTitle;
  if (ASC_getAPTitles(assoc->params, callingTitle, calledTitle, NULL).good())
  {
    callingAETitle = "\"";
    callingAETitle += callingTitle;
    callingAETitle += "\"";
    calledAETitle = "\"";
    calledAETitle += calledTitle;
    calledAETitle += "\"";
  }
  else
  {
    // should never happen
    callingAETitle.clear();
    calledAETitle.clear();
  }

  /* now do the real work, i.e. receive DIMSE commmands over the network connection */
  /* which was established and handle these commands correspondingly. In case of */
  /* storscp only C-ECHO-RQ and C-STORE-RQ commands can be processed. */
  cond = processCommands(assoc);

  if (cond == DUL_PEERREQUESTEDRELEASE)
  {
    dcmtkLogger::infoStream() << "Association Release";
    cond = ASC_acknowledgeRelease(assoc);
  }
  else if (cond == DUL_PEERABORTEDASSOCIATION)
  {
    dcmtkLogger::infoStream() << "Association Aborted";
  }
  else
  {
    dcmtkLogger::errorStream() << "DIMSE failure (aborting association): " << DimseCondition::dump(temp_str, cond);
    /* some kind of error so abort the association */
    cond = ASC_abortAssociation(assoc);
  }

// this is bad!
 cleanup:

  if (cond.code() == DULC_FORKEDCHILD) 
      return cond;

  cond = ASC_dropSCPAssociation(assoc);
  if (cond.bad())
  {
    dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
    exit(1);
  }
  cond = ASC_destroyAssociation(&assoc);
  if (cond.bad())
  {
    dcmtkLogger::errorStream() << DimseCondition::dump(temp_str, cond);
    exit(1);
  }

  return cond;
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkStoreScp::processCommands(T_ASC_Association * assoc)
{
  OFCondition cond = EC_Normal;
  T_DIMSE_Message msg;
  T_ASC_PresentationContextID presID = 0;
  DcmDataset *statusDetail = NULL;

  // start a loop to be able to receive more than one DIMSE command
  while( cond == EC_Normal || cond == DIMSE_NODATAAVAILABLE || cond == DIMSE_OUTOFRESOURCES )
  {

    // receive a DIMSE command over the network
    if( opt_endOfStudyTimeout == -1 )
      cond = DIMSE_receiveCommand(assoc, DIMSE_BLOCKING, 0, &presID, &msg, &statusDetail);
    else
      cond = DIMSE_receiveCommand(assoc, DIMSE_NONBLOCKING, OFstatic_cast(int, opt_endOfStudyTimeout), &presID, &msg, &statusDetail);

    // check what kind of error occurred. If no data was
    // received, check if certain other conditions are met
    if( cond == DIMSE_NODATAAVAILABLE )
    {
      // If in addition to the fact that no data was received also option --eostudy-timeout is set and
      // if at the same time there is still a study which is considered to be open (i.e. we were actually
      // expecting to receive more objects that belong to this study) (this is the case if lastStudyInstanceUID
      // does not equal NULL), we have to consider that all objects for the current study have been received.
      // In such an "end-of-study" case, we might have to execute certain optional functions which were specified
      // by the user through command line options passed to storescp.
      if( opt_endOfStudyTimeout != -1 && !lastStudyInstanceUID.empty() )
      {
        // indicate that the end-of-study-event occured through a timeout event.
        // This knowledge will be necessary in function renameOnEndOFStudy().
        endOfStudyThroughTimeoutEvent = OFTrue;

        // before we actually execute those optional functions, we need to determine the path and name
        // of the subdirectory into which the DICOM files for the last study were written.
        lastStudySubdirectoryPathAndName = subdirectoryPathAndName;

        // now we can finally handle end-of-study events which might have to be executed
        executeEndOfStudyEvents();

        // also, we need to clear lastStudyInstanceUID to indicate
        // that the last study is not considered to be open any more.
        lastStudyInstanceUID.clear();

        // also, we need to clear subdirectoryPathAndName
        subdirectoryPathAndName.clear();

        // reset the endOfStudyThroughTimeoutEvent variable.
        endOfStudyThroughTimeoutEvent = OFFalse;
      }
    }

    // if the command which was received has extra status
    // detail information, dump this information
    if (statusDetail != NULL)
    {
      dcmtkLogger::warningStream() << "Status Detail:";
        dcmtkLogger::warningStream() << DcmObject::PrintHelper(*statusDetail);
      delete statusDetail;
    }

    // check if peer did release or abort, or if we have a valid message
    if (cond == EC_Normal)
    {
      // in case we received a valid message, process this command
      // note that storescp can only process a C-ECHO-RQ and a C-STORE-RQ
      switch (msg.CommandField)
      {
        case DIMSE_C_ECHO_RQ:
          // process C-ECHO-Request
          cond = echoSCP(assoc, &msg, presID);
          break;
        case DIMSE_C_STORE_RQ:
          // process C-STORE-Request
          cond = storeSCP(assoc, &msg, presID);
          break;
        default:
          // we cannot handle this kind of message
          cond = DIMSE_BADCOMMANDTYPE;
          dcmtkLogger::errorStream() << "cannot handle command: 0x" << STD_NAMESPACE hex << OFstatic_cast(unsigned, msg.CommandField);
          break;
      }
    }
  }
  return cond;
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkStoreScp::echoSCP( T_ASC_Association * assoc, T_DIMSE_Message * msg, T_ASC_PresentationContextID presID)
{
  OFString temp_str;
  dcmtkLogger::infoStream() << "Received Echo Request";
  dcmtkLogger::debugStream() << DIMSE_dumpMessage(temp_str, msg->msg.CEchoRQ, DIMSE_INCOMING, NULL, presID);

  /* the echo succeeded !! */
  OFCondition cond = DIMSE_sendEchoResponse(assoc, presID, &msg->msg.CEchoRQ, STATUS_Success, NULL);
  if (cond.bad())
  {
    dcmtkLogger::errorStream() << "Echo SCP Failed: " << DimseCondition::dump(temp_str, cond);
  }
  return cond;
}

//---------------------------------------------------------------------------------------------

void dcmtkStoreScp::storeSCPCallback(void *callbackData, T_DIMSE_StoreProgress *progress, T_DIMSE_C_StoreRQ *req,
                             char * /*imageFileName*/, DcmDataset **imageDataSet, T_DIMSE_C_StoreRSP *rsp,
                             DcmDataset **statusDetail)
{
    if (!callbackData)
    return;

    // get context
    dcmtkStoreScp* scp = (dcmtkStoreScp*) callbackData;


    DIC_UI sopClass;
    DIC_UI sopInstance;

    // determine if the association shall be aborted
    if( (scp->opt_abortDuringStore && progress->state != DIMSE_StoreBegin) ||
        (scp->opt_abortAfterStore && progress->state == DIMSE_StoreEnd) )
    {
      dcmtkLogger::infoStream() << "ABORT initiated (due to command line options)";
      ASC_abortAssociation(scp->m_assoc);
      rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
      return;
    }

  // if opt_sleepAfter is set, the user requires that the application shall
  // sleep a certain amount of seconds after having received one PDU.
  if (scp->opt_sleepDuring > 0)
  {
    OFStandard::sleep(OFstatic_cast(unsigned int, scp->opt_sleepDuring));
  }

  // TODO find substitute here - mk
  /*
  // dump some information if required (depending on the progress state)
  // We can't use oflog for the pdu output, but we use a special logger for
  // generating this output. If it is set to level "INFO" we generate the
  // output, if it's set to "DEBUG" then we'll assume that there is debug output
  // generated for each PDU elsewhere.
  OFLogger progressLogger = OFLog::getLogger("MedInria storeScp.progress");
  if (progressLogger.getChainedLogLevel() == OFLogger::INFO_LOG_LEVEL)
  {
    switch (progress->state)
    {
      case DIMSE_StoreBegin:
        COUT << "RECV: ";
        break;
      case DIMSE_StoreEnd:
        COUT << OFendl;
        break;
      default:
        COUT << '.';
        break;
    }
    COUT.flush();
  }
  */

  // if this is the final call of this function, save the data which was received to a file
  // (note that we could also save the image somewhere else, put it in database, etc.)
  if (progress->state == DIMSE_StoreEnd)
  {
    OFString tmpStr;

    // do not send status detail information
    *statusDetail = NULL;

  
    // Concerning the following line: an appropriate status code is already set in the resp structure,
    // it need not be success. For example, if the caller has already detected an out of resources problem
    // then the status will reflect this.  The callback function is still called to allow cleanup.
    //rsp->DimseStatus = STATUS_Success;

    // we want to write the received information to a file only if this information
    // is present and the options opt_bitPreserving and opt_ignore are not set.
    if ((imageDataSet != NULL) && (*imageDataSet != NULL) && !scp->opt_bitPreserving && !scp->opt_ignore)
    {
      OFString fileName;

      // in case one of the --sort-xxx options is set, we need to perform some particular steps to
      // determine the actual name of the output file
      if (scp->opt_sortStudyMode != dcmtkStoreScp::ESM_None)
      {
        // determine the study instance UID in the (current) DICOM object that has just been received
        OFString currentStudyInstanceUID;
        if ((*imageDataSet)->findAndGetOFString(DCM_StudyInstanceUID, currentStudyInstanceUID).bad() || currentStudyInstanceUID.empty())
        {
          dcmtkLogger::errorStream() << "element StudyInstanceUID " << DCM_StudyInstanceUID << " absent or empty in data set";
          rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
          return;
        }

        // if --sort-on-patientname is active, we need to extract the
        // patients name (format: last_name^first_name)
        OFString currentPatientsName;
        if (scp->opt_sortStudyMode == dcmtkStoreScp::ESM_PatientsName)
        {
          OFString tmpName;
          if ((*imageDataSet)->findAndGetOFString(DcmTagKey(0x0010, 0x0010), tmpName).bad() || tmpName.empty())
          {
            // default if patient name is missing or empty
            tmpName = "ANONYMOUS";
            dcmtkLogger::warningStream() << "element PatientsName " << DcmTagKey(0x0010, 0x0010) << " absent or empty in data set, using '"
                 << tmpName << "' instead";
          }

          /* substitute non-ASCII characters in patient name to ASCII "equivalent" */
          const size_t length = tmpName.length();
          for (size_t i = 0; i < length; i++)
            scp->mapCharacterAndAppendToString(tmpName[i], currentPatientsName);
        }

        // if this is the first DICOM object that was received or if the study instance UID in the
        // current DICOM object does not equal the last object's study instance UID we need to create
        // a new subdirectory in which the current DICOM object will be stored
        if (scp->lastStudyInstanceUID.empty() || (scp->lastStudyInstanceUID != currentStudyInstanceUID))
        {
          // if lastStudyInstanceUID is non-empty, we have just completed receiving all objects for one
          // study. In such a case, we need to set a certain indicator variable (lastStudySubdirectoryPathAndName),
          // so that we know that executeOnEndOfStudy() might have to be executed later. In detail, this indicator
          // variable will contain the path and name of the last study's subdirectory, so that we can still remember
          // this directory, when we execute executeOnEndOfStudy(). The memory that is allocated for this variable
          // here will be freed after the execution of executeOnEndOfStudy().
          if (!scp->lastStudyInstanceUID.empty())
            scp->lastStudySubdirectoryPathAndName = scp->subdirectoryPathAndName;

          // create the new lastStudyInstanceUID value according to the value in the current DICOM object
          scp->lastStudyInstanceUID = currentStudyInstanceUID;

          // get the current time (needed for subdirectory name)
          OFDateTime dateTime;
          dateTime.setCurrentDateTime();

          // create a name for the new subdirectory.
          char timestamp[32];
          sprintf(timestamp, "%04u%02u%02u_%02u%02u%02u%03u",
            dateTime.getDate().getYear(), dateTime.getDate().getMonth(), dateTime.getDate().getDay(),
            dateTime.getTime().getHour(), dateTime.getTime().getMinute(), dateTime.getTime().getIntSecond(), dateTime.getTime().getMilliSecond());

          OFString subdirectoryName;
          switch (scp->opt_sortStudyMode)
          {
            case dcmtkStoreScp::ESM_Timestamp:
              // pattern: "[prefix]_[YYYYMMDD]_[HHMMSSMMM]"
              subdirectoryName = scp->opt_sortStudyDirPrefix;
              if (!subdirectoryName.empty())
                subdirectoryName += '_';
              subdirectoryName += timestamp;
              break;
            case dcmtkStoreScp::ESM_StudyInstanceUID:
              // pattern: "[prefix]_[Study Instance UID]"
              subdirectoryName = scp->opt_sortStudyDirPrefix;
              if (!subdirectoryName.empty())
                subdirectoryName += '_';
              subdirectoryName += currentStudyInstanceUID;
              break;
            case dcmtkStoreScp::ESM_PatientsName:
              // pattern: "[Patient's Name]_[YYYYMMDD]_[HHMMSSMMM]"
              subdirectoryName = currentPatientsName;
              subdirectoryName += '_';
              subdirectoryName += timestamp;
              break;
            case dcmtkStoreScp::ESM_None:
              break;
          }

          // create subdirectoryPathAndName (string with full path to new subdirectory)
          OFStandard::combineDirAndFilename(scp->subdirectoryPathAndName, OFStandard::getDirNameFromPath(tmpStr, scp->m_imageFileName), subdirectoryName);

          // check if the subdirectory already exists
          // if it already exists dump a warning
          if( OFStandard::dirExists(scp->subdirectoryPathAndName) )
            dcmtkLogger::warningStream() << "subdirectory for study already exists: " << scp->subdirectoryPathAndName;
          else
          {
            // if it does not exist create it
            dcmtkLogger::infoStream() << "creating new subdirectory for study: " << scp->subdirectoryPathAndName;
#ifdef HAVE_WINDOWS_H
            if( _mkdir( scp->subdirectoryPathAndName.c_str() ) == -1 )
#else
            if( mkdir( scp->subdirectoryPathAndName.c_str(), S_IRWXU | S_IRWXG | S_IRWXO ) == -1 )
#endif
            {
              dcmtkLogger::errorStream() << "could not create subdirectory for study: " << scp->subdirectoryPathAndName;
              rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
              return;
            }
            // all objects of a study have been received, so a new subdirectory is started.
            // ->timename counter can be reset, because the next filename can't cause a duplicate.
            // if no reset would be done, files of a new study (->new directory) would start with a counter in filename
            if (scp->opt_timeNames)
              scp->timeNameCounter = -1;
          }
        }

        // integrate subdirectory name into file name (note that cbdata->imageFileName currently contains both
        // path and file name; however, the path refers to the output directory captured in opt_outputDirectory)
        OFStandard::combineDirAndFilename(fileName, scp->subdirectoryPathAndName, OFStandard::getFilenameFromPath(tmpStr, scp->m_imageFileName));

        // update global variable outputFileNameArray
        // (might be used in executeOnReception() and renameOnEndOfStudy)
        scp->outputFileNameArray.push_back(tmpStr);
      }
      // if no --sort-xxx option is set, the determination of the output file name is simple
      else
      {
        fileName = scp->m_imageFileName;

        // update global variables outputFileNameArray
        // (might be used in executeOnReception() and renameOnEndOfStudy)
        scp->outputFileNameArray.push_back(OFStandard::getFilenameFromPath(tmpStr, fileName));
      }

      // determine the transfer syntax which shall be used to write the information to the file
      E_TransferSyntax xfer = scp->opt_writeTransferSyntax;
      if (xfer == EXS_Unknown) xfer = (*imageDataSet)->getOriginalXfer();

      // store file either with meta header or as pure dataset
      dcmtkLogger::infoStream() << "storing DICOM file: " << fileName;
      if (OFStandard::fileExists(fileName))
      {
        dcmtkLogger::warningStream() << "DICOM file already exists: " << fileName;
      }
      OFCondition cond = scp->m_dcmff->saveFile(fileName.c_str(), xfer, scp->opt_sequenceType, scp->opt_groupLength,
          scp->opt_paddingType, OFstatic_cast(Uint32, scp->opt_filepad), OFstatic_cast(Uint32, scp->opt_itempad),
          (scp->opt_useMetaheader) ? EWM_fileformat : EWM_dataset);

      if (cond.bad())
      {
        dcmtkLogger::errorStream() << "cannot write DICOM file: " << fileName << ": " << cond.text();
        rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
      }

      // check the image to make sure it is consistent, i.e. that its sopClass and sopInstance correspond
      // to those mentioned in the request. If not, set the status in the response message variable.
      if ((rsp->DimseStatus == STATUS_Success)&&(!scp->opt_ignore))
      {
        // which SOP class and SOP instance ?
        if (!DU_findSOPClassAndInstanceInDataSet(*imageDataSet, sopClass, sopInstance, scp->opt_correctUIDPadding))
        {
           dcmtkLogger::errorStream() << "bad DICOM file: " << fileName;
           rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
        }
        else if (strcmp(sopClass, req->AffectedSOPClassUID) != 0)
        {
          rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
        }
        else if (strcmp(sopInstance, req->AffectedSOPInstanceUID) != 0)
        {
          rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
        }
      }
    }

    // in case opt_bitPreserving is set, do some other things
    if( scp->opt_bitPreserving )
    {
      // we need to set outputFileNameArray and outputFileNameArrayCnt to be
      // able to perform the placeholder substitution in executeOnReception()
      scp->outputFileNameArray.push_back(OFStandard::getFilenameFromPath(tmpStr, scp->m_imageFileName));
    }
  }
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkStoreScp::storeSCP( T_ASC_Association *assoc, T_DIMSE_Message *msg, T_ASC_PresentationContextID presID)
{
  OFCondition cond = EC_Normal;
  T_DIMSE_C_StoreRQ *req;
  char imageFileName[2048];

  // assign the actual information of the C-STORE-RQ command to a local variable
  req = &msg->msg.CStoreRQ;

  // if opt_ignore is set, the user requires that the data shall be received but not
  // stored. in this case, we want to create a corresponding temporary filename for
  // a file in which the data shall be stored temporarily. If this is not the case,
  // create a real filename (consisting of path and filename) for a real file.
  if (opt_ignore)
  {
#ifdef _WIN32
    tmpnam(imageFileName);
#else
    strcpy(imageFileName, NULL_DEVICE_NAME);
#endif
  }
  else
  {
    // 3 possibilities: create unique filenames (fn), create timestamp fn, create fn from SOP Instance UIDs
    if (opt_uniqueFilenames)
    {
      // create unique filename by generating a temporary UID and using ".X." as an infix
      char buf[70];
      dcmGenerateUniqueIdentifier(buf);
      sprintf(imageFileName, "%s%c%s.X.%s%s", opt_outputDirectory.c_str(), PATH_SEPARATOR, dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "UNKNOWN"),
        buf, opt_fileNameExtension.c_str());
    }
    else if (opt_timeNames)
    {
      // create a name for the new file. pattern: "[YYYYMMDDHHMMSSMMM]_[NUMBER].MODALITY[EXTENSION]" (use current datetime)
      // get the current time (needed for file name)
      OFDateTime dateTime;
      dateTime.setCurrentDateTime();
      // used to hold prospective filename
      char cmpFileName[2048];
      // next if/else block generates prospective filename, that is compared to last written filename
      if (timeNameCounter == -1)
      {
        // timeNameCounter not set -> last written filename has to be without "serial number"
        sprintf(cmpFileName, "%04u%02u%02u%02u%02u%02u%03u.%s%s",
          dateTime.getDate().getYear(), dateTime.getDate().getMonth(), dateTime.getDate().getDay(),
          dateTime.getTime().getHour(), dateTime.getTime().getMinute(), dateTime.getTime().getIntSecond(), dateTime.getTime().getMilliSecond(),
          dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "UNKNOWN"), opt_fileNameExtension.c_str());
      }
      else
      {
        // counter was active before, so generate filename with "serial number" for comparison
        sprintf(cmpFileName, "%04u%02u%02u%02u%02u%02u%03u_%04u.%s%s", // millisecond version
          dateTime.getDate().getYear(), dateTime.getDate().getMonth(), dateTime.getDate().getDay(),
          dateTime.getTime().getHour(), dateTime.getTime().getMinute(), dateTime.getTime().getIntSecond(), dateTime.getTime().getMilliSecond(),
          timeNameCounter, dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "UNKNOWN"), opt_fileNameExtension.c_str());
      }
      if ( (outputFileNameArray.size()!=0) && (outputFileNameArray.back() == cmpFileName) )
      {
        // if this is not the first run and the prospective filename is equal to the last written filename
        // generate one with a serial number (incremented by 1)
        timeNameCounter++;
        sprintf(imageFileName, "%s%c%04u%02u%02u%02u%02u%02u%03u_%04u.%s%s", opt_outputDirectory.c_str(), PATH_SEPARATOR, // millisecond version
          dateTime.getDate().getYear(), dateTime.getDate().getMonth(), dateTime.getDate().getDay(),
          dateTime.getTime().getHour(), dateTime.getTime().getMinute(), dateTime.getTime().getIntSecond(), dateTime.getTime().getMilliSecond(),
          timeNameCounter, dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "UNKNOWN"), opt_fileNameExtension.c_str());
      }
      else
      {
        // first run or filenames are different: create filename without serial number
        sprintf(imageFileName, "%s%c%04u%02u%02u%02u%02u%02u%03u.%s%s", opt_outputDirectory.c_str(), PATH_SEPARATOR, // millisecond version
          dateTime.getDate().getYear(), dateTime.getDate().getMonth(), dateTime.getDate().getDay(),
          dateTime.getTime().getHour(), dateTime.getTime().getMinute(),dateTime.getTime().getIntSecond(), dateTime.getTime().getMilliSecond(),
          dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "UNKNOWN"), opt_fileNameExtension.c_str());
        // reset counter, because timestamp and therefore filename has changed
        timeNameCounter = -1;
      }
    }
    else
    {
      // don't create new UID, use the study instance UID as found in object
      sprintf(imageFileName, "%s%c%s.%s%s", opt_outputDirectory.c_str(), PATH_SEPARATOR, dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "UNKNOWN"),
        req->AffectedSOPInstanceUID, opt_fileNameExtension.c_str());
    }
  }

  // dump some information if required
  OFString str;
  dcmtkLogger::infoStream() << "Received Store Request: MsgID " << req->MessageID << ", ("
    << dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "OT") << ")";
  dcmtkLogger::debugStream() << DIMSE_dumpMessage(str, *req, DIMSE_INCOMING, NULL, presID);

  // intialize some variables
  this->m_assoc = assoc;
  this->m_imageFileName = imageFileName;
  DcmFileFormat dcmff;
  this->m_dcmff = &dcmff;

  // store SourceApplicationEntityTitle in metaheader
  if (assoc && assoc->params)
  {
    const char *aet = assoc->params->DULparams.callingAPTitle;
    if (aet) dcmff.getMetaInfo()->putAndInsertString(DCM_SourceApplicationEntityTitle, aet);
  }

  // define an address where the information which will be received over the network will be stored
  DcmDataset *dset = dcmff.getDataset();

  // if opt_bitPreserving is set, the user requires that the data shall be
  // written exactly as it was received. Depending on this option, function
  // DIMSE_storeProvider must be called with certain parameters.
  if (opt_bitPreserving)
  {
    cond = DIMSE_storeProvider(assoc, presID, req, imageFileName, opt_useMetaheader, NULL,
      storeSCPCallback, (void*) this, opt_blockMode, opt_dimse_timeout);
  }
  else
  {
    cond = DIMSE_storeProvider(assoc, presID, req, NULL, opt_useMetaheader, &dset,
      storeSCPCallback, (void*) this, opt_blockMode, opt_dimse_timeout);
  }

  // if some error occured, dump corresponding information and remove the outfile if necessary
  if (cond.bad())
  {
    OFString temp_str;
    dcmtkLogger::errorStream() << "Store SCP Failed: " << DimseCondition::dump(temp_str, cond);
    // remove file
    if (!opt_ignore)
    {
      if (strcmp(imageFileName, NULL_DEVICE_NAME) != 0)
        OFStandard::deleteFile(imageFileName);
    }
  }
#ifdef _WIN32
  else if (opt_ignore)
  {
    if (strcmp(imageFileName, NULL_DEVICE_NAME) != 0)
      OFStandard::deleteFile(imageFileName); // delete the temporary file
  }
#endif

  // if everything was successful so far and option --exec-on-reception is set,
  // we want to execute a certain command which was passed to the application
  if( cond.good() && opt_execOnReception != NULL )
    executeOnReception();

  // if everything was successful so far, go ahead and handle possible end-of-study events
  if( cond.good() )
    executeEndOfStudyEvents();

  // if opt_sleepAfter is set, the user requires that the application shall
  // sleep a certain amount of seconds after storing the instance data.
  if (opt_sleepAfter > 0)
  {
    OFStandard::sleep(OFstatic_cast(unsigned int, opt_sleepAfter));
  }

  // return return value
  return cond;
}

//---------------------------------------------------------------------------------------------

void dcmtkStoreScp::executeEndOfStudyEvents()
{
  // if option --rename-on-eostudy is set and variable lastStudySubdirectoryPathAndName
  // does not equal NULL (i.e. we received all objects that belong to one study, or - in
  // other words - it is the end of one study) we want to rename the output files that
  // belong to the last study. (Note that these files are captured in outputFileNameArray)
  if( opt_renameOnEndOfStudy && !lastStudySubdirectoryPathAndName.empty() )
    renameOnEndOfStudy();

  // if option --exec-on-eostudy is set and variable lastStudySubdirectoryPathAndName does
  // not equal NULL (i.e. we received all objects that belong to one study, or - in other
  // words - it is the end of one study) we want to execute a certain command which was
  // passed to the application
  //if( opt_execOnEndOfStudy != NULL && !lastStudySubdirectoryPathAndName.empty() )
  if(!lastStudySubdirectoryPathAndName.empty())
    executeOnEndOfStudy();
 
  lastStudySubdirectoryPathAndName.clear();
}

//---------------------------------------------------------------------------------------------

void dcmtkStoreScp::executeOnReception()
{
  OFString cmd = opt_execOnReception;

  // in case a file was actually written
  if( !opt_ignore )
  {
    // perform substitution for placeholder #p (depending on presence of any --sort-xxx option)
    OFString dir = (opt_sortStudyMode == ESM_None) ? opt_outputDirectory : subdirectoryPathAndName;
    cmd = replaceChars( cmd, OFString(PATH_PLACEHOLDER), dir );

    // perform substitution for placeholder #f; note that outputFileNameArray.back()
    // always contains the name of the file (without path) which was written last.
    OFString outputFileName = outputFileNameArray.back();
    cmd = replaceChars( cmd, OFString(FILENAME_PLACEHOLDER), outputFileName );
  }

  // perform substitution for placeholder #a
  cmd = replaceChars( cmd, OFString(CALLING_AETITLE_PLACEHOLDER), callingAETitle );

  // perform substitution for placeholder #c
  cmd = replaceChars( cmd, OFString(CALLED_AETITLE_PLACEHOLDER), calledAETitle );

  // Execute command in a new process
 // executeCommand( cmd );
}

//---------------------------------------------------------------------------------------------

void dcmtkStoreScp::renameOnEndOfStudy()
{
  int counter = 1;

  OFListIterator(OFString) first = outputFileNameArray.begin();
  OFListIterator(OFString) last = outputFileNameArray.end();

  // before we deal with all the filenames which are included in the array, we need to distinguish
  // two different cases: If endOfStudyThroughTimeoutEvent is not true, the last filename in the array
  // refers to a file that belongs to a new study of which the first object was just received. (In this
  // case there are at least two filenames in the array). Then, this last filename is - at the end of the
  // following loop - not supposed to be deleted from the array. If endOfStudyThroughTimeoutEvent is true,
  // all filenames that are captured in the array, refer to files that belong to the same study. Hence,
  // all of these files shall be renamed and all of the filenames within the array shall be deleted.
  if( !endOfStudyThroughTimeoutEvent ) --last;

  // rename all files that belong to the last study
  while (first != last)
  {
    // determine the new file name: The first two characters of the old file name make up the [modality-prefix].
    // The value for [consecutive-numbering] will be determined using the counter variable.
    char modalityId[3];
    char newFileName[9];
    if (opt_timeNames)
    {
      // modality prefix are the first 2 characters after serial number (if present)
      size_t serialPos = (*first).find("_");
      if (serialPos != OFString_npos)
      {
        //serial present: copy modality prefix (skip serial: 1 digit "_" + 4 digits serial + 1 digit ".")
        OFStandard::strlcpy( modalityId, (*first).substr(serialPos+6, 2).c_str(), 3 );
      }
      else
      {
        //serial not present, copy starts directly after first "." (skip 17 for timestamp, one for ".")
        OFStandard::strlcpy( modalityId, (*first).substr(18, 2).c_str(), 3 );
      }
    }
    else
    {
      OFStandard::strlcpy( modalityId, (*first).c_str(), 3 );
    }
    sprintf( newFileName, "%s%06d", modalityId, counter );

    // create two strings containing path and file name for
    // the current filename and the future filename
    OFString oldPathAndFileName;
    oldPathAndFileName = lastStudySubdirectoryPathAndName;
    oldPathAndFileName += PATH_SEPARATOR;
    oldPathAndFileName += *first;

    OFString newPathAndFileName;
    newPathAndFileName = lastStudySubdirectoryPathAndName;
    newPathAndFileName += PATH_SEPARATOR;
    newPathAndFileName += newFileName;

    // rename file
    if( rename( oldPathAndFileName.c_str(), newPathAndFileName.c_str() ) != 0 )
      dcmtkLogger::warningStream() << "cannot rename file '" << oldPathAndFileName << "' to '" << newPathAndFileName << "'";

    // remove entry from list
    first = outputFileNameArray.erase(first);

    // increase counter
    counter++;
  }
}

//---------------------------------------------------------------------------------------------

void dcmtkStoreScp::executeOnEndOfStudy()
{
  OFString cmd = opt_execOnEndOfStudy;

  // perform substitution for placeholder #p; #p will be substituted by lastStudySubdirectoryPathAndName
  cmd = replaceChars( cmd, OFString(PATH_PLACEHOLDER), lastStudySubdirectoryPathAndName );

  // perform substitution for placeholder #a
  cmd = replaceChars( cmd, OFString(CALLING_AETITLE_PLACEHOLDER), (endOfStudyThroughTimeoutEvent) ? callingAETitle : lastCallingAETitle );

  // perform substitution for placeholder #c
  cmd = replaceChars( cmd, OFString(CALLED_AETITLE_PLACEHOLDER), (endOfStudyThroughTimeoutEvent) ? calledAETitle : lastCalledAETitle );

  std::cout << "STUDY WRITTEN TO: " << lastStudySubdirectoryPathAndName << std::endl;

  emit endOfStudy (QString(lastStudySubdirectoryPathAndName.c_str()));
}

//---------------------------------------------------------------------------------------------

OFString dcmtkStoreScp::replaceChars( const OFString &srcstr, const OFString &pattern, const OFString &substitute )
{
  OFString result = srcstr;
  size_t pos = 0;

  while( pos != OFString_npos )
  {
    pos = result.find( pattern, pos );

    if( pos != OFString_npos )
    {
      result.replace( pos, pattern.size(), substitute );
      pos += substitute.size();
    }
  }

  return( result );
}

//---------------------------------------------------------------------------------------------

DUL_PRESENTATIONCONTEXT* dcmtkStoreScp::findPresentationContextID(LST_HEAD * head,
                                T_ASC_PresentationContextID presentationContextID)
{
  DUL_PRESENTATIONCONTEXT *pc;
  LST_HEAD **l;
  OFBool found = OFFalse;

  if (head == NULL)
    return NULL;

  l = &head;
  if (*l == NULL)
    return NULL;

  pc = OFstatic_cast(DUL_PRESENTATIONCONTEXT *, LST_Head(l));
  (void)LST_Position(l, OFstatic_cast(LST_NODE *, pc));

  while (pc && !found) {
    if (pc->presentationContextID == presentationContextID) {
      found = OFTrue;
    } else {
      pc = OFstatic_cast(DUL_PRESENTATIONCONTEXT *, LST_Next(l));
    }
  }
  return pc;
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkStoreScp::acceptUnknownContextsWithTransferSyntax(T_ASC_Parameters * params,
                                                                   const char* transferSyntax,
                                                                   T_ASC_SC_ROLE acceptedRole)
{
  OFCondition cond = EC_Normal;
  int n, i, k;
  DUL_PRESENTATIONCONTEXT *dpc;
  T_ASC_PresentationContext pc;
  OFBool accepted = OFFalse;
  OFBool abstractOK = OFFalse;

  n = ASC_countPresentationContexts(params);
  for (i = 0; i < n; i++)
  {
    cond = ASC_getPresentationContext(params, i, &pc);
    if (cond.bad()) return cond;
    abstractOK = OFFalse;
    accepted = OFFalse;

    if (dcmFindNameOfUID(pc.abstractSyntax) == NULL)
    {
      abstractOK = OFTrue;

      /* check the transfer syntax */
      for (k = 0; (k < OFstatic_cast(int, pc.transferSyntaxCount)) && !accepted; k++)
      {
        if (strcmp(pc.proposedTransferSyntaxes[k], transferSyntax) == 0)
        {
          accepted = OFTrue;
        }
      }
    }

    if (accepted)
    {
      cond = ASC_acceptPresentationContext(
        params, pc.presentationContextID,
        transferSyntax, acceptedRole);
      if (cond.bad()) return cond;
    } else {
      T_ASC_P_ResultReason reason;

      /* do not refuse if already accepted */
      dpc = findPresentationContextID(params->DULparams.acceptedPresentationContext,
                                      pc.presentationContextID);
      if ((dpc == NULL) || ((dpc != NULL) && (dpc->result != ASC_P_ACCEPTANCE)))
      {

        if (abstractOK) {
          reason = ASC_P_TRANSFERSYNTAXESNOTSUPPORTED;
        } else {
          reason = ASC_P_ABSTRACTSYNTAXNOTSUPPORTED;
        }
        /*
         * If previously this presentation context was refused
         * because of bad transfer syntax let it stay that way.
         */
        if ((dpc != NULL) && (dpc->result == ASC_P_TRANSFERSYNTAXESNOTSUPPORTED))
          reason = ASC_P_TRANSFERSYNTAXESNOTSUPPORTED;

        cond = ASC_refusePresentationContext(params, pc.presentationContextID, reason);
        if (cond.bad()) return cond;
      }
    }
  }
  return EC_Normal;
}

//---------------------------------------------------------------------------------------------

OFCondition dcmtkStoreScp::acceptUnknownContextsWithPreferredTransferSyntaxes(T_ASC_Parameters * params,
                                                                              const char* transferSyntaxes[], 
                                                                              int transferSyntaxCount,
                                                                              T_ASC_SC_ROLE acceptedRole)
{
  OFCondition cond = EC_Normal;
  /*
  ** Accept in the order "least wanted" to "most wanted" transfer
  ** syntax.  Accepting a transfer syntax will override previously
  ** accepted transfer syntaxes.
  */
  for (int i = transferSyntaxCount - 1; i >= 0; i--)
  {
    cond = acceptUnknownContextsWithTransferSyntax(params, transferSyntaxes[i], acceptedRole);
    if (cond.bad()) return cond;
  }
  return cond;
}

//---------------------------------------------------------------------------------------------

void dcmtkStoreScp::mapCharacterAndAppendToString(Uint8 c, OFString &output)
{
    static const char *latin1_table[] =
    {
        "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", // Codes 0-15
        "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", // Codes 16-31
        "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", // Codes 32-47
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "_", "_", "_", "_", "_", "_", // Codes 48-63
        "_", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", // Codes 64-79
        "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "_", "_", "_", "_", "_", // Codes 80-95
        "_", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", // Codes 96-111
        "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "_", "_", "_", "_", "_", // Codes 112-127
        "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", // Codes 128-143
        "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", // Codes 144-159
        "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", // Codes 160-175
        "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", // Codes 176-191
        "A", "A", "A", "A", "Ae","A", "A", "C", "E", "E", "E", "E", "I", "I", "I", "I", // Codes 192-107
        "D", "N", "O", "O", "O", "O", "Oe","_", "O", "U", "U", "U", "Ue","Y", "_", "ss",// Codes 108-123
        "a", "a", "a", "a", "ae","a", "a", "c", "e", "e", "e", "e", "i", "i", "i", "i", // Codes 124-141
        "d", "n", "o", "o", "o", "o", "oe","_", "o", "u", "u", "u", "ue","y", "_", "y"  // Codes 142-157
    };
    output += latin1_table[c];
}

//---------------------------------------------------------------------------------------------

bool dcmtkStoreScp::setStorageDirectory(const char* directory)
{
    opt_outputDirectory = directory;
    
    // Todo check if directory is valid
    return true;
}

//---------------------------------------------------------------------------------------------

void dcmtkStoreScp::stopService()
{
    m_stop = true;
}

//---------------------------------------------------------------------------------------------

void dcmtkStoreScp::run()
{
    startService();
}

//---------------------------------------------------------------------------------------------
