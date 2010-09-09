   /**
    * This software was developed using DCMTK from OFFIS e.V.
    *
    * Kuratorium OFFIS e.V.
    * Healthcare Information and Communication Systems
    * Escherweg 2
    * D-26121 Oldenburg, Germany
    */

#ifndef DCMTKBASESCP_H
#define DCMTKBASESCP_H

#include "dcmtkBase.h"

/**
 * @class dcmtkBase
 * @author Michael Knopke
 * @brief base class for all SCP base implementations.
 */
class dcmtkBaseScp : public dcmtkBase
{
public:

   /*
    * Set the default connection parameters to simplify start/stop
    * @param ourTitel The AE of this store SCP 
    * @param ourIP The IP of the store SCP (not used)
    * @param ourPort The port number of to listen.
    * @return 0 for success, otherwise errorcode
    */
    void setConnectionParams(const char* ourTitle, const char* ourIP, unsigned short ourPort);

protected:

     const char*        opt_respondingAETitle;
     OFCmdUnsignedInt   opt_port;
 
};

#endif DCMTKBASESCP_H