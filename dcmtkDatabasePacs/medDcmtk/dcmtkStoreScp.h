    /**
    * This software was developed using DCMTK from OFFIS e.V.
    *
    * Kuratorium OFFIS e.V.
    * Healthcare Information and Communication Systems
    * Escherweg 2
    * D-26121 Oldenburg, Germany
    */


#ifndef DCMTKSTORESCP_H
#define DCMTKSTPRESCP_H

// fwd
class DcmAssociationConfiguration;
class DcmFileFormat;

// includes
#include "dcmtkBaseScp.h"
#include <iostream>

/**
 * @class dcmtkStoreScp
 * @author Michael Knopke
 * @brief Class to implement Server functionality (C-Store only)
 * Should be running in a threaded environment to be able to listen to commands at all times.
 */
class dcmtkStoreScp : public dcmtkBaseScp
{
    Q_OBJECT

public:

    /**
    * Constructor
    */
    dcmtkStoreScp();

    virtual void run();

    /*
    * Main function to start the store SCP server. Should be called in a threaded env.
    * @param ourTitel The AE of this store SCP 
    * @param ourIP The IP of the store SCP
    * @param ourPort The port number of to listen.
    * @return 0 for success, otherwise errorcode
    */
    int startService(const char* ourTitle, const char* ourIP, unsigned int ourPort);

   /**
    * Overloaded for convenience. starts tge store SCP server.
    * Uses the internal connection parameters. 
    * @see setConnectionParams(const char* , const char* , unsigned int );
    * @return 0 for success or errorcode
    */
    int startService();

   /**
    * Stop the storescp loop and exit normally
    */
    void stopService();

    /**
    * Set a directory where the store-scp should store its incoming data.
    * The default storage directory is '.'
    * @param directory valid directory
    * @return True if directory is valid, false otherwise.
    */
    bool setStorageDirectory(const char* directory);

signals:
    void endOfStudy(QString);

protected:

    /*
    * This function receives DIMSE commmands over the network connection
    * and handles these commands correspondingly. Note that in case of
    * storscp only C-ECHO-RQ and C-STORE-RQ commands can be processed.
    * @param assoc - [in] The association (network connection to another DICOM application).
    */
    OFCondition processCommands(T_ASC_Association *assoc);

    OFCondition acceptAssociation(T_ASC_Network *net, DcmAssociationConfiguration& asccfg);

    /*
    * This function processes a DIMSE C-ECHO-RQ commmand that was
    * received over the network connection.
    * @params assoc  - [in] The association (network connection to another DICOM application).
    * @params msg    - [in] The DIMSE C-ECHO-RQ message that was received.
    * @params presID - [in] The ID of the presentation context which was specified in the PDV which contained
    *                       the DIMSE command.
    */
    OFCondition echoSCP(T_ASC_Association * assoc, T_DIMSE_Message * msg, T_ASC_PresentationContextID presID);

    /*
    * This function processes a DIMSE C-STORE-RQ commmand that was
    * received over the network connection.
    * @params assoc  - [in] The association (network connection to another DICOM application).
    * @params msg    - [in] The DIMSE C-STORE-RQ message that was received.
    * @params presID - [in] The ID of the presentation context which was specified in the PDV which contained
    *                       the DIMSE command.
    */
    OFCondition storeSCP(T_ASC_Association * assoc, T_DIMSE_Message * msg, T_ASC_PresentationContextID presID);

    /*
    * This function.is used to indicate progress when storescp receives instance data over the
    * network. On the final call to this function (identified by progress->state == DIMSE_StoreEnd)
    * this function will store the data set which was received over the network to a file.
    * Earlier calls to this function will simply cause some information to be dumped to stdout.
    * @param callbackData  - [in] data for this callback function
    * @param progress      - [in] The state of progress. (identifies if this is the initial or final call
    *                             to this function, or a call in between these two calls.
    * @param req           - [in] The original store request message.
    * @param imageFileName - [in] The path to and name of the file the information shall be written to.
    * @param imageDataSet  - [in] The data set which shall be stored in the image file
    * @param rsp           - [inout] the C-STORE-RSP message (will be sent after the call to this function)
    * @param statusDetail  - [inout] This variable can be used to capture detailed information with regard to
    *                                the status information which is captured in the status element (0000,0900). Note
    *                                that this function does specify any such information, the pointer will be set to NULL.
    */
    static void storeSCPCallback(void *callbackData, T_DIMSE_StoreProgress *progress, T_DIMSE_C_StoreRQ *req,
                                 char * /*imageFileName*/, DcmDataset **imageDataSet, T_DIMSE_C_StoreRSP *rsp,
                                 DcmDataset **statusDetail);

    /*
    * This function deals with the execution of end-of-study-events. In detail,
    * events that need to take place are sepcified by the user through certain
    * command line options. The options that define these end-of-study-events
    * are "--rename-on-eostudy" and "--exec-on-eostudy".
    */
    void executeEndOfStudyEvents();

    /*
    * This function deals with the execution of the command line which was passed
    * to option --exec-on-reception of the storescp. This command line is captured
    * in opt_execOnReception. Note that the command line can contain the placeholders
    * PATH_PLACEHOLDER and FILENAME_PLACEHOLDER which need to be substituted before the command line is actually
    * executed. PATH_PLACEHOLDER will be substituted by the path to the output directory into which
    * the last file was written; FILENAME_PLACEHOLDER will be substituted by the filename of the last
    * file which was written.
    */
    void executeOnReception();

    /*
    * This function deals with renaming the last study's output files. In detail, these file's
    * current filenames will be changed to a filename that corresponds to the pattern [modality-
    * prefix][consecutive-numbering]. The current filenames of all files that belong to the study
    * are captured in outputFileNameArray. The new filenames will be calculated whithin this
    * function: The [modality-prefix] will be taken from the old filename,
    * [consecutive-numbering] is a consecutively numbered, 6 digit number which will be calculated
    * starting from 000001.
    */
    void renameOnEndOfStudy();

    /*
    * This function deals with the execution of the command line which was passed
    * to option --exec-on-eostudy of the storescp. This command line is captured
    * in opt_execOnEndOfStudy. Note that the command line can contain the placeholder
    * PATH_PLACEHOLDER which needs to be substituted before the command line is actually executed.
    * In detail, PATH_PLACEHOLDER will be substituted by the path to the output directory into which
    * the files of the last study were written.
    */
    void executeOnEndOfStudy();
    
    
    /*
    * This function replaces all occurrences of pattern in srcstr with substitute and returns
    * the result as a new OFString variable. Note that srcstr itself will not be changed,
    * @param srcstr     - [in] The source string.
    * @param pattern    - [in] The pattern string which shall be substituted.
    * @param substitute - [in] The substitute for pattern in srcstr.
    */
    OFString replaceChars( const OFString &srcstr, const OFString &pattern, const OFString &substitute );


    DUL_PRESENTATIONCONTEXT* findPresentationContextID(LST_HEAD * head, T_ASC_PresentationContextID presentationContextID);

    /* accept all presenstation contexts for unknown SOP classes,
    *  i.e. UIDs appearing in the list of abstract syntaxes
    *  where no corresponding name is defined in the UID dictionary.
    *  @param params pointer to association parameters structure
    *  @param transferSyntax transfer syntax to accept
    *  @param acceptedRole SCU/SCP role to accept
    */
    OFCondition acceptUnknownContextsWithTransferSyntax(T_ASC_Parameters * params, const char* transferSyntax,
                                                        T_ASC_SC_ROLE acceptedRole);

    /* accept all presenstation contexts for unknown SOP classes,
    *  i.e. UIDs appearing in the list of abstract syntaxes
    *  where no corresponding name is defined in the UID dictionary.
    *  This method is passed a list of "preferred" transfer syntaxes.
    *  @param params pointer to association parameters structure
    *  @param transferSyntax transfer syntax to accept
    *  @param acceptedRole SCU/SCP role to accept
    */
    OFCondition acceptUnknownContextsWithPreferredTransferSyntaxes(T_ASC_Parameters * params,
                                                                   const char* transferSyntaxes[],
                                                                   int transferSyntaxCount,
                                                                   T_ASC_SC_ROLE acceptedRole = ASC_SC_ROLE_DEFAULT);

    /*
    * substitute non-ASCII characters with ASCII "equivalents"
    * @param c Character to substitute
    * @param output Output string
    */
    void mapCharacterAndAppendToString(Uint8 c, OFString &output);



private:

    /* sort study mode */
    enum E_SortStudyMode
        {
          ESM_None,
          ESM_Timestamp,
          ESM_StudyInstanceUID,
          ESM_PatientsName
        };


    OFBool             opt_showPresentationContexts;
    OFBool             opt_uniqueFilenames;
    OFString           opt_fileNameExtension;
    OFBool             opt_timeNames;
    int                timeNameCounter;   // "serial number" to differentiate between files with same timestamp
    OFBool             opt_refuseAssociation;
    OFBool             opt_rejectWithoutImplementationUID;
    OFCmdUnsignedInt   opt_sleepAfter;
    OFCmdUnsignedInt   opt_sleepDuring;
    OFCmdUnsignedInt   opt_maxPDU;
    OFBool             opt_useMetaheader;
    OFBool             opt_acceptAllXfers;
    E_TransferSyntax   opt_networkTransferSyntax;
    E_TransferSyntax   opt_writeTransferSyntax;
    E_GrpLenEncoding   opt_groupLength;
    E_EncodingType     opt_sequenceType;
    E_PaddingEncoding  opt_paddingType;
    OFCmdUnsignedInt   opt_filepad;
    OFCmdUnsignedInt   opt_itempad;
    OFCmdUnsignedInt   opt_compressionLevel;
    OFBool             opt_bitPreserving;
    OFBool             opt_ignore;
    OFBool             opt_abortDuringStore;
    OFBool             opt_abortAfterStore;
    OFBool             opt_promiscuous;
    OFBool             opt_correctUIDPadding;
    OFBool             opt_inetd_mode;
    OFString           callingAETitle;       // calling application entity title will be stored here
    OFString           lastCallingAETitle;
    OFString           calledAETitle;        // called application entity title will be stored here
    OFString           lastCalledAETitle;
    OFBool             opt_secureConnection;    // default: no secure connection
    OFString           opt_outputDirectory;         // default: output directory equals "."
    E_SortStudyMode    opt_sortStudyMode;      // default: no sorting
    const char*        opt_sortStudyDirPrefix;     // default: no directory prefix
    OFString           lastStudyInstanceUID;
    OFString           subdirectoryPathAndName;
    OFList<OFString>   outputFileNameArray;
    const char*        opt_execOnReception;        // default: don't execute anything on reception
    const char*        opt_execOnEndOfStudy;       // default: don't execute anything on end of study

    OFString           lastStudySubdirectoryPathAndName;
    OFBool             opt_renameOnEndOfStudy;  // default: don't rename any files on end of study
    long               opt_endOfStudyTimeout;        // default: no end of study timeout
    OFBool             endOfStudyThroughTimeoutEvent;
    const char*        opt_configFile;
    const char*        opt_profileName;
    T_DIMSE_BlockingMode opt_blockMode;
    int                opt_dimse_timeout;
    int                opt_acse_timeout;

    OFString temp_str;

    char*              m_imageFileName;
    DcmFileFormat*     m_dcmff;
    T_ASC_Association* m_assoc;
    bool               m_stop;

};

#endif //DCMTKSTPRESCP_H
