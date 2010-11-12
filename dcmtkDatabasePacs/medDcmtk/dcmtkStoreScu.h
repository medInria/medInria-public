    /**
    * This software was developed using DCMTK from OFFIS e.V.
    *
    * Kuratorium OFFIS e.V.
    * Healthcare Information and Communication Systems
    * Escherweg 2
    * D-26121 Oldenburg, Germany
    */

#ifndef DCMTKSTORESCU_H
#define DCMTKSTORESCU_H

// inc
#include "dcmtkBaseScu.h"

/**
 * @class dcmtkStoreScu
 * @author Michael Knopke
 * @brief class to send C-STORE to the remote node.
 * Used to send DICOM data to another modality.
 */
class dcmtkStoreScu : public  dcmtkBaseScu
{
public:

    /**
    * Default constructor
    */
    dcmtkStoreScu();


    /**
    * Default Destructor
    */
    ~dcmtkStoreScu();

    /**
    * run - thread method
    * @return   void
    */
    virtual void run();

    /**
    * Directory to look for DICOM files. Default ist: current directory
    * @param dir path to dicom source files
    * see setScanPattern(const char* pattern) for file exclusion
    */
    bool setScanDirectory(const char* dir);

    /*
    * Add a scan pattern. Default is all files and types: *.*
    */
    bool setScanPattern(const char* pattern){};

    /**
    * Performs the C-STORE request on the peer.
    * @param peerTitle The AE of the peer
    * @param peerIP The IP of the peer that performs the C-FIND.
    * @param peerPort The port number of the peer.
    * @param ourTitel The AE of the caller
    * @param ourIP The IP of the caller.
    * @param ourPort The port number of the caller.
    * @return 0 for success, otherwise errorcode
    */
    int  sendStoreRequest(const char* peerTitle, const char* peerIP, int peerPort, 
                          const char* ourTitle, const char* ourIP, int ourPort);

    /**
    * Overloaded for convenience. Performs the C-STORE request on the peer.
    * Uses the internal connection parameters. 
    * @see setConnectionParams(const char*, const char*, unsigned short,const char*, const char*, unsigned short)
    * @return 0 for success or errorcode
    */
    int sendStoreRequest();

protected:

    /**
    * This function will read all the information from the given file,
    * figure out a corresponding presentation context which will be used
    * to transmit the information over the network to the SCP, and it
    * will finally initiate the transmission of all data to the SCP.
    * @param assoc - [in] The association (network connection to another DICOM application).
    * @param fname - [in] Name of the file which shall be processed.
    */
    OFCondition storeSCU(T_ASC_Association *assoc, const char *fname);

    /**
    * This function will process the given file as often as is specified by opt_repeatCount.
    * "Process" in this case means "read file, send C-STORE-RQ, receive C-STORE-RSP".
    * @param assoc - [in] The association (network connection to another DICOM application).
    * @param fname - [in] Name of the file which shall be processed.
    */
    OFCondition cstore(T_ASC_Association *assoc, const OFString &fname);

    OFBool findSOPClassAndInstanceInFile(const char *fname, char *sopClass, char *sopInstance);

    OFBool isaListMember(OFList<OFString> &lst, OFString &s);

    OFCondition addPresentationContext(T_ASC_Parameters *params, int presentationContextId,
                                                  const OFString &abstractSyntax, const OFString &transferSyntax,
                                                  T_ASC_SC_ROLE proposedRole = ASC_SC_ROLE_DEFAULT);

    OFCondition addPresentationContext(T_ASC_Parameters *params, int presentationContextId,
                                                  const OFString &abstractSyntax, const OFList<OFString> &transferSyntaxList,
                                                  T_ASC_SC_ROLE proposedRole = ASC_SC_ROLE_DEFAULT);

    OFCondition addStoragePresentationContexts(T_ASC_Parameters *params, OFList<OFString> &sopClasses);

    /**
    * SSL stuff, not used at the moment
    */
    OFCondition configureUserIdentityRequest(T_ASC_Parameters *params);
    
    /**
    * SSL stuff, not used at the moment
    */
    OFCondition checkUserIdentityResponse(T_ASC_Parameters *params);

    static void progressCallback(void * /*callbackData*/, T_DIMSE_StoreProgress *progress,
                          T_DIMSE_C_StoreRQ * req);

    OFBool updateStringAttributeValue(DcmItem *dataset, const DcmTagKey &key, OFString &value);

    void replaceSOPInstanceInformation(DcmDataset *dataset);


private:


    // helper

    int secondsSince1970();

    OFString intToString(int i);

    OFString makeUID(OFString basePrefix, int counter);


    // variables
    OFList<OFString>                    inputFiles;
    OFString                            opt_directory;
    OFBool                              opt_showPresentationContexts;
    OFBool                              opt_abortAssociation;
    E_FileReadMode                      opt_readMode;

    OFBool                              opt_scanDir;
    OFBool                              opt_recurse;
    const char*                         opt_scanPattern;

    OFBool                              opt_haltOnUnsuccessfulStore;
    OFBool                              unsuccessfulStoreEncountered;
    int                                 lastStatusCode;

    OFBool                              opt_proposeOnlyRequiredPresentationContexts;
    OFBool                              opt_combineProposedTransferSyntaxes;

    T_ASC_UserIdentityNegotiationMode   opt_identMode;
    OFString                            opt_user;
    OFString                            opt_password;
    OFString                            opt_identFile;
    OFBool                              opt_identResponse;

    
    OFCmdUnsignedInt opt_inventPatientCount;
    OFCmdUnsignedInt opt_inventStudyCount;
    OFCmdUnsignedInt opt_inventSeriesCount;
    OFBool opt_inventSOPInstanceInformation;
    OFBool opt_correctUIDPadding;
    OFString patientNamePrefix;         // PatientName is PN (maximum 16 chars)
    OFString patientIDPrefix;           // PatientID is LO (maximum 64 chars)
    OFString studyIDPrefix;             // StudyID is SH (maximum 16 chars)
    OFString accessionNumberPrefix;     // AccessionNumber is SH (maximum 16 chars)
    
    OFBool opt_secureConnection; 
    const char *opt_configFile;
    const char *opt_profileName;


};

#endif //DCMTKSTORESCU_H
