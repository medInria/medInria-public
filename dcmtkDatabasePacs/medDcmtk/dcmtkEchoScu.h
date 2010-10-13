   /**
    * This software was developed using DCMTK from OFFIS e.V.
    *
    * Kuratorium OFFIS e.V.
    * Healthcare Information and Communication Systems
    * Escherweg 2
    * D-26121 Oldenburg, Germany
    */

#ifndef DCMTKECHOSCU_H
#define DCMTKECHOSCU_H

#include "dcmtkBaseScu.h"

/**
 * @class dcmtkEchoScu
 * @author Michael Knopke
 * @brief class to send C-ECHO to the remote node.
 * Use it to test if the DICOM connection is set up correctly (one way only). 
 * It's not to be mistaken with a common ping command.
 * The echo might fail if the remote node doesn't know the client (us)
 */
class dcmtkEchoScu : public  dcmtkBaseScu
{
public:

   /**
    * Send the C-ECHO request to the peer.
    * @param peerTitle The AE of the peer
    * @param peerIP The IP of the peer that performs the C-FIND.
    * @param peerPort The port number of the peer.
    * @param ourTitel The AE of the caller
    * @param ourIP The IP of the caller.
    * @param ourPort The port number of the caller.
    * @return 0 for success, otherwise errorcode
    */
    int sendEchoRequest(const char* peerTitle, const char* peerIP, int peerPort, 
                        const char* ourTitle, const char* ourIP, int ourPort);

   /**
    * Overloaded for convenience. Performs the C-ECHO request on the peer.
    * Uses the internal connection parameters. 
    * @see setConnectionParams(const char*, const char*, unsigned short,const char*, const char*, unsigned short)
    * @return 0 for success or errorcode
    */
    int sendEchoRequest();

protected:

   /*
    * This function will send a C-ECHO-RQ over the network to another DICOM application
    * and handle the response.
    * @param assoc - [in] The association (network connection to another DICOM application).
    * @param opt_blockMode The DIMSE blocking mode
    * @param opt_dimse_timeout DIMSE timeout value
    */
    OFCondition echoSCU(T_ASC_Association * assoc, T_DIMSE_BlockingMode opt_blockMode, int opt_dimse_timeout);

    /*
     * This function will send num_repeat C-ECHO-RQ messages to the DICOM application
     * this application is connected with and handle corresponding C-ECHO-RSP messages.
     * @param assoc      - [in] The association (network connection to another DICOM application).
     * @param num_repeat - [in] The amount of C-ECHO-RQ messages which shall be sent.
     * @param opt_blockMode The DIMSE blocking mode
     * @param opt_dimse_timeout DIMSE timeout value
     */
    OFCondition cecho(T_ASC_Association * assoc, unsigned long num_repeat, T_DIMSE_BlockingMode opt_blockMode, int opt_dimse_timeout);


};

#endif //DCMTKECHOSCU_H
