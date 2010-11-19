    /**
    * This software was developed using DCMTK from OFFIS e.V.
    *
    * Kuratorium OFFIS e.V.
    * Healthcare Information and Communication Systems
    * Escherweg 2
    * D-26121 Oldenburg, Germany
    */

#ifndef DCMTKFINDSCU_H
#define DCMTKFINDSCU_H

#include "dcmtkBaseScu.h"

#include "dcmtk/ofstd/ofstring.h" /* for class OFString */
#include "dcmtk/ofstd/oflist.h"

#include <iostream>
#include <vector>

// fwd
class dcmtkResultDataset;
class DcmFindSCUCallback;
class dcmtkFindScuCallback;

/**
 * @class dcmtkFindScu
 * @author Michael Knopke
 * @brief class to send C-FIND to the remote node.
 * Slight wrapper for DcmFindSCU class, uses custom callback (dcmtkFindCallback) to aquire data
 * The result will be stored in resultData that can be used to acquire the data.
 */
class dcmtkFindScu : public dcmtkBaseScu
{
    Q_OBJECT
public:

   /*
    * Constructor
    */
    dcmtkFindScu();

   /*
    * Destructor
    */
    ~dcmtkFindScu();

    /**
    * run - thread method
    * @return   void
    */
    virtual void run();

   /**
    * Performs the C-FIND request on the peer.
    * @param peerTitle The AE of the peer
    * @param peerIP The IP of the peer that performs the C-FIND.
    * @param peerPort The port number of the peer.
    * @param ourTitel The AE of the caller
    * @param ourIP The IP of the caller.
    * @param ourPort The port number of the caller.
    * @return 0 for success or errorcode
    */
    int sendFindRequest(const char* peerTitle, const char* peerIP, unsigned int peerPort, 
                        const char* ourTitle, const char* ourIP, unsigned int ourPort);

   /**
    * Overloaded for convenience. Performs the C-FIND request on the peer.
    * Uses the internal connection parameters. 
    * @see setConnectionParams(const char*, const char*, unsigned short,const char*, const char*, unsigned short)
    * @return 0 for success or errorcode
    */
    int sendFindRequest();

   /**
    * Clear all previously set attributes.
    */
    void clearAllQueryAttributes();

public slots:

        /**
        * sendCancelRequest - Stop the current request on the PACS
        * @return   void
        */
        virtual void sendCancelRequest();

protected:

  /** main worker method that negotiates an association, executes one or more 
   *  C-FIND-RQ transactions, processes the responses and closes the association 
   *  once everything is finished (or an error has occured).
   *  @param peer hostname or IP address of peer SCP host
   *  @param port TCP port number of peer SCP host
   *  @param ourTitle calling AE title
   *  @param peerTitle called AE title
   *  @param abstractSyntax SOP Class UID or Meta SOP Class UID of service
   *  @param preferredTransferSyntax. May be Unknown, Implicit Little Endian, or any of the
   *    two uncompressed explicit VR transfer syntaxes. By default (unknown), local endian
   *    explicit VR is proposed first, followed by opposite endian explicit VR, followed by
   *    implicit VR. This behaviour can be modified by explicitly specifying the preferred
   *    explicit VR transfer syntax. With Little Endian Implicit, only Implicit VR is proposed.
   *  @param blockMode DIMSE blocking mode
   *  @param dimse_timeout timeout for DIMSE operations (in seconds)
   *  @param maxReceivePDULength limit the maximum PDU size for incoming PDUs to the given value.
   *    This value should be less than or equal to ASC_DEFAULTMAXPDU, and is usually identical
   *    to ASC_DEFAULTMAXPDU (other values are only useful for debugging purposes).
   *  @param secureConnection this flag, if true, requests a secure TLS connection to be used
   *    instead of a normal connection. This will only work if DCMTK has been compiled with
   *    OpenSSL support (WITH_OPENSSL) and if a transport layer object supporting secure
   *    connections has been set with setTransportLayer() prior to this call.
   *  @param abortAssociation abort association instead of releasing it (for debugging purposes)
   *  @param repeatCount number of times this query should be repeated
   *    (for debugging purposes, works only with default callback)
   *  @param extractResponsesToFile if true, extract incoming response messages to file
   *    (works only with default callback)
   *  @param cancelAfterNResponses issue C-FIND-CANCEL after given number of responses
   *    (works only with default callback)
   *  @param overrideKeys list of keys/paths that override those in the query files, if any.
   *    Either the list of query files or override keys or both should be non-empty, because the query
   *    dataset will be empty otherwise. For path syntax see DcmPath.
   *  @param callback user-provided non-default callback handler object. 
   *    For default callback, pass NULL.
   *  @param fileNameList list of query files. Each file is expected to be a DICOM file
   *    containing a dataset that is used as a query, possibly modified by override keys, if any.
   *    This parameter, if non-NULL, points to a list of filenames (paths).
   *  @return EC_Normal if successful, an error code otherwise
   */  
  OFCondition performQuery( const char *peer,
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
                            DcmFindSCUCallback *callback = NULL,
                            OFList<OFString> *fileNameList = NULL);


  /** perform a single C-FIND transaction on association that is already open.
   *  @param assoc DIMSE association
   *  @param fname file name for the query file to be used with this query. May be NULL.
   *  @param repeatCount number of times this query should be repeated
   *    (for debugging purposes, works only with default callback)
   *  @param abstractSyntax SOP Class UID or Meta SOP Class UID of service
   *  @param blockMode DIMSE blocking mode
   *  @param dimse_timeout timeout for DIMSE operations (in seconds)
   *  @param extractResponsesToFile if true, extract incoming response messages to file
   *    (works only with default callback)
   *  @param cancelAfterNResponses issue C-FIND-CANCEL after given number of responses
   *    (works only with default callback)
   *  @param overrideKeys dataset with keys that override those in the query file, if any.
   *    Either query file or override keys or both should be non-empty, because the query
   *    dataset will be empty otherwise.
   *  @param callback user-provided non-default callback handler object. 
   *    For default callback, pass NULL.
   *  @return EC_Normal if successful, an error code otherwise
   */
    OFCondition findSCU(T_ASC_Association * assoc, 
                        const char *fname, 
                        int repeatCount,  
                        const char *abstractSyntax,
                        T_DIMSE_BlockingMode blockMode, 
                        int dimse_timeout, 
                        OFBool extractResponsesToFile,
                        int cancelAfterNResponses,
                        OFList<OFString> *overrideKeys,
                        DcmFindSCUCallback *callback = NULL) const;


    static void progressCallback(void *callbackData,
                                T_DIMSE_C_FindRQ *request,
                                int responseCount,
                                T_DIMSE_C_FindRSP *rsp,
                                DcmDataset *responseIdentifiers);

    /**
    * Process the attribute to be used in queries.
    * Uses OFList to store the attributes and values.
    * @return Success if key was recognized.
    */
    virtual bool processQueryAttribute(const char* key);

 
    OFList<OFString>                        overrideKeys;
    OFList<OFString>                        fileNameList;

 
    dcmtkFindScuCallback    *findCallback;
 
};

#endif
