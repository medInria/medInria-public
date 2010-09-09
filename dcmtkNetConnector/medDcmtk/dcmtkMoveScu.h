    /**
    * This software was developed using DCMTK from OFFIS e.V.
    *
    * Kuratorium OFFIS e.V.
    * Healthcare Information and Communication Systems
    * Escherweg 2
    * D-26121 Oldenburg, Germany
    */

#ifndef DCMTKMOVESCU_H
#define DCMTKMOVESCU_H

#ifndef WIN32
#define HAVE_CONFIG_H
#endif


#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */
#include "dcmtk/dcmdata/dcdicent.h"
#include "dcmtk/dcmdata/dcostrmz.h"   /* for dcmZlibCompressionLevel */

class DcmFileFormat;

#include "dcmtkBaseScu.h"


    /**
    * @class dcmtkMoveScu
    * @author Michael Knopke
    * @brief class to perform C-MOVE requests on the remote node.
    * The common usage is to replace C-GET and redirect the move to yourself.
    * Use it to request and receive DICOM data.
    * The result will be stored in resultData that can be used to acquire the data.
    */
class dcmtkMoveScu : public dcmtkBaseScu
{
public:

    /**
    * Default Constructor
    */
    dcmtkMoveScu();

    /**
    * Performs the C-MOVE request on the peer.
    * You can use this function to move dicom data directly to another AET, 
    * but its purpose is to replace C-GET and move to itself.
    * @param peerTitle The AE of the peer
    * @param peerIP The IP of the peer that performs the C-MOVE.
    * @param peerPort The port number of the peer.
    * @param targetTitel The move target (thats us) AE
    * @param targetIP The IP of the move target.
    * @param targetPort The port number of the move target.
    */
    int sendMoveRequest(const char* peerTitle, const char* peerIP, int peerPort, 
                        const char* targetTitle, const char* targetIP, int targetPort);

    /**
    * Overloaded for convenience. Performs the C-MOVE request on the peer.
    * Uses the internal connection parameters. 
    * @see setConnectionParams(const char*, const char*, unsigned short,const char*, const char*, unsigned short)
    * @return 0 for success or errorcode
    */
    int sendMoveRequest();

    /**
    * The example comes with a simple STORE-SCP and ECHO-SCP, 
    * that is listening for commands only while requesting a C-MOVE.
    * Usually the application should have a STORE-SCP that is running somewhere else.
    * @param flag Set this flag to true if you want to try out the C-MOVE, default is false
    */
    void useBuildInStoreSCP(bool flag);



protected:

    /**
    * Process the attribute to be used in queries.
    * Uses a dcmDataset to process the search keys.
    * @return Success if key was recognized.
    */
    virtual bool processQueryAttribute(const char* key);


    /**
    * Method that calls moveSCU in a loop (in case of connection problems)
    * @param assoc The association
    * @param fname Function name, can be NULL
    * @returns condition status
    */
    OFCondition cmove(T_ASC_Association * assoc, const char *fname);

    /**
    * Method that calls DIMSE_moveUser with a callback (moveCallback)
    * @param assoc The association
    * @param fname Function name, can be NULL
    * @returns condition status
    */
    OFCondition moveSCU(T_ASC_Association * assoc, const char *fname);

    /**
    * Sample echoSCP implementation, better use standalone SCP. Default: not used
    * @see useBuildInStoreSCP(bool)
    */
    static OFCondition echoSCP(T_ASC_Association * assoc,
                               T_DIMSE_Message * msg,
                               T_ASC_PresentationContextID presID);

    /**
    * Sample storeSCP implementation, better use standalone SCP. Default: not used
    * @see useBuildInStoreSCP(bool)
    */
    static OFCondition storeSCP( T_ASC_Association *assoc,
                                 T_DIMSE_Message *msg,
                                 T_ASC_PresentationContextID presID,
                                 void* subOpCallbackData);

    /**
    * Receives and handles DIMSE commands (C-ECHO and C-STORE only). Helper for echoSCP and storeSCP
    * @see useBuildInStoreSCP(bool)
    */
    static OFCondition subOpSCP(T_ASC_Association **subAssoc, void * subOpCallbackData);

    /**
    * Handles TransferSyntax and accepts associations (echo, store only)
    */
    static OFCondition acceptSubAssoc(T_ASC_Network * aNet, T_ASC_Association ** assoc);

    /**
    * Callback for storeSCU method that handles (e.g.writes) the received data
    */
    static void storeSCPCallback(/* in */
                                 void *callbackData,
                                 T_DIMSE_StoreProgress *progress,                /* progress state */
                                 T_DIMSE_C_StoreRQ *req,                         /* original store request */
                                 char *imageFileName, DcmDataset **imageDataSet, /* being received into */
                                 /* out */
                                 T_DIMSE_C_StoreRSP *rsp,                        /* final store response */
                                 DcmDataset **statusDetail);

    /**
    * Callback for supOPSCU method that prepares negotiation
    */
    static void subOpCallback(void * /*subOpCallbackData*/ ,T_ASC_Network *aNet, T_ASC_Association **subAssoc);

    /**
    * Callback for moveSCU method that handles the c-move-rsp
    */
    static void moveCallback(void *callbackData, T_DIMSE_C_MoveRQ *request,
                             int responseCount, T_DIMSE_C_MoveRSP *response);

    /**
    * Helper to substitute keys with dcmdatasets
    */
    void substituteOverrideKeys(DcmDataset *dset);

private:


    typedef enum {
         QMPatientRoot = 0,
         QMStudyRoot = 1,
         QMPatientStudyOnly = 2
         } QueryModel;

    typedef struct {
         const char *findSyntax;
         const char *moveSyntax;
         } QuerySyntax;

    typedef struct {
         T_ASC_Association *assoc;
         T_ASC_PresentationContextID presId;
         } MyCallbackInfo;


    OFCmdUnsignedInt  opt_maxPDU;
    OFBool            opt_useMetaheader;
    OFBool            opt_acceptAllXfers;
    E_TransferSyntax  opt_writeTransferSyntax;
    E_GrpLenEncoding  opt_groupLength;
    E_EncodingType    opt_sequenceType;
    E_PaddingEncoding opt_paddingType;
    OFCmdUnsignedInt  opt_filepad;
    OFCmdUnsignedInt  opt_itempad;
    OFCmdUnsignedInt  opt_compressionLevel;
    OFBool            opt_bitPreserving;
    OFBool            opt_ignore;
    OFBool            opt_abortDuringStore;
    OFBool            opt_abortAfterStore;
    OFBool            opt_correctUIDPadding;
    E_TransferSyntax  opt_in_networkTransferSyntax;
    E_TransferSyntax  opt_out_networkTransferSyntax;
    OFBool            opt_abortAssociation;
    const char *      opt_moveDestination;
    OFCmdSignedInt    opt_cancelAfterNResponses;
    QueryModel        opt_queryModel;
    OFBool            opt_ignorePendingDatasets;
    OFString          opt_outputDirectory;
    bool              m_useBuildInStoreScp;


    DcmDataset          *overrideKeys;  
    char*               m_imageFileName;
    DcmFileFormat*      m_dcmff;
    T_ASC_Association*  m_assoc;

};

#endif DCMTKMOVESCU_H
