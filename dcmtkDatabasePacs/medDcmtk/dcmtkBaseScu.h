   /**
    * This software was developed using DCMTK from OFFIS e.V.
    *
    * Kuratorium OFFIS e.V.
    * Healthcare Information and Communication Systems
    * Escherweg 2
    * D-26121 Oldenburg, Germany
    */

#ifndef DCMTKBASESCU_H
#define DCMTKBASESCU_H

// inc
#include "dcmtkBase.h"
#include "dcmtkContainer.h"

// fwd
class dcmtkKey;
class dcmtkNode;
class dcmtkKeyContainer;

/**
 * @class dcmtkBaseScu
 * @author Michael Knopke
 * @brief base class for all SCU implementations
 */
class dcmtkBaseScu : public dcmtkBase
{
    Q_OBJECT
public:

   /**
    * Default Constructor
    */
    dcmtkBaseScu();

   /**
    * Default Destructor
    */
    ~dcmtkBaseScu();

   /**
    * Struct defining the query level (search hierachy)
    */
    enum eQueryLevel{
            PATIENT,
            STUDY,
            SERIES,
            IMAGE};

   /**
    * Set a new query level.
    * @param level The query level
    * @see dcmtkBaseScu
    */
    virtual void setQueryLevel(eQueryLevel level);

   /**
    * Set the default connection parameters to simplify queries.
    * @param peerTitle The AE of the peer
    * @param peerIP The IP of the peer that performs the C-FIND.
    * @param peerPort The port number of the peer.
    * @param ourTitel The AE of the caller
    * @param ourIP The IP of the caller.
    * @param ourPort The port number of the caller.    
    */
    virtual void setConnectionParams(const char* peerTitle, const char* peerIP, unsigned short peerPort, 
                                     const char* ourTitle, const char* ourIP, unsigned short ourPort);

   /**
    * Override all params with the default values.
    * Subclasses should set their specific params here too
    */
    virtual void resetDefaultParams();

   /**
    * Set one or more attributes to look for
    * You have to set at least one attribute.
    * This could be for example the name of the patient using Attribute: (0010,0010) = MyPatient.
    * @param group The group number
    * @param elem The element number
    * @param value The value for this attribute
    * @return Success if key was recognized.
    * @see addQueryAttribute(const char* key)
    */
    virtual bool addQueryAttribute(int group, int elem, const char* value);

   /**
    * Removes all added query attributes.
    * abstract please override
    */
    virtual void clearAllQueryAttributes();

   /**
    * Returns the result container that contains all datasets that belong to nodes
    */
    dcmtkContainer<dcmtkNode*>* getNodeContainer();

    /**
    * Set this to true if you want to release the association manually. Default: False
    * @params: bool flag if True association will be created only if not previously released
    * @return   void
    */
    void keepAssociationOpen(bool flag);

    /**
    * ReleaseAssociation from outside use last condition. Should only be used if keepAssociationOpen is set to true
    * @return   void
    */
    void releaseAssociation();

public slots:

    /**
    * sendCancelRequest - Stop the current request on the PACS
    * @return   void
    */
    virtual void sendCancelRequest();

signals:
    void progressed(int);
    void finished();

protected:

    typedef enum {
        QMPatientRoot = 0,
        QMStudyRoot = 1,
        QMPatientStudyOnly = 2
    } QueryModel;

    typedef struct {
        const char *findSyntax;
        const char *moveSyntax;
    } QuerySyntax;


    /**
    * Overloaded Helper
    * @param group The group number
    * @param elem The element number
    * @param value The value for this attribute
    * @return Success if key was recognized.
    * @see addQueryAttribute(const char* key)
    */
    virtual bool addQueryAttribute(const char* group, const char* elem, const char* value);


   /**
    * Overloaded Helper
    * Use either key format: "group,element=value" or dictionary name (see dcmtk/dcuid)
    * @param key The group and element number including the value
    * @return Success if key was recognized.
    */
    virtual bool addQueryAttribute(const char* key);
    
   /**
    * We prefer to use Explicitly encoded transfer syntaxes.
    * If we are running on a Little Endian machine we prefer
    * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
    * Some SCP implementations will just select the first transfer
    * syntax they support (this is not part of the standard) so
    * organise the proposed transfer syntaxes to take advantage
    * of such behaviour.
    *
    * The presentation contexts proposed here are only used for
    * C-FIND and C-MOVE, so there is no need to support compressed
    * transmission.
    */
    OFCondition addPresentationContext(T_ASC_Parameters *params,
                                       T_ASC_PresentationContextID pid,
                                       const char* abstractSyntax,
                                       E_TransferSyntax preferredTransferSyntax);

    
    /**
    * CreateAssociation (currently only cmove uses this)
    * work in progress
    * @return   OFCondition
    */
    OFCondition createAssociation();

    
    /**
    * ReleaseAssociation
    * @params: OFCondition & cond
    * @return   int
    */
    int releaseAssociation(OFCondition &cond);


   /**
    * Process the attribute to be used in queries.
    * virtual and abstract, override it if needed!
    * @return Success if key was recognized.
    */
    virtual bool processQueryAttribute(const char* key){return false;};


    // common variables

    T_ASC_Network*        net;              /* the global DICOM network */
    T_ASC_Parameters*     params;
    T_ASC_Association*    assoc;


    const char*           opt_ourTitle;
    const char*           opt_peerTitle;
    const char*           opt_peer;
    const char*           opt_ourIP; 
    OFCmdUnsignedInt      opt_port;         /* peerPort */
    OFCmdUnsignedInt      opt_retrievePort; /* ourPort */

    bool                  m_keepAssocOpen;
    bool                  m_assocExists;
    OFCondition           m_lastCondition;
    QueryModel            opt_queryModel;
    OFCmdUnsignedInt      opt_maxPDU;
    int                   opt_acse_timeout;
    T_DIMSE_BlockingMode  opt_blockMode;
    int                   opt_dimse_timeout;
    OFCmdUnsignedInt      opt_maxReceivePDULength;
    OFCmdUnsignedInt      opt_maxSendPDULength;
    E_TransferSyntax      opt_networkTransferSyntax;
    OFCmdUnsignedInt      opt_repeatCount;
    OFBool                opt_secureConnection; 
    OFBool                opt_abortAssociation;
    OFCmdSignedInt        opt_cancelAfterNResponses;
    OFBool                opt_extractResponsesToFile;
    const char*           opt_abstractSyntax;
    OFCmdUnsignedInt      opt_sleepAfter;
    OFCmdUnsignedInt      opt_sleepDuring;


    OFString              temp_str;

    eQueryLevel                  m_currentQueryLevel;
    dcmtkContainer<dcmtkNode*>   m_resContainer;
    dcmtkContainer<dcmtkKey*>    m_keyContainer;

    bool                         m_cancelRqst;
    std::string                  szPeerTitle;
    std::string                  szOurTitle;
    std::string                  szPeerIP;
    std::string                  szOurIP;

};

#endif //DCMTKBASESCU_H
