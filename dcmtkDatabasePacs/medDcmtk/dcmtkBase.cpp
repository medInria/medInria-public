#include "dcmtkBase.h"

#include "dcmtk/dcmdata/dcdict.h"

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING

#include "dcmtkLogger.h"

static OFLogger dummyLogger = OFLog::getLogger("inria" "inria");

//---------------------------------------------------------------------------------------------

dcmtkBase::dcmtkBase()
{
#ifdef HAVE_GUSI_H
    /* needed for Macintosh */
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    /* we need at least version 1.1 */
    WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        dcmtkLogger::warningStream() << "no data dictionary loaded, check environment variable: " 
             << DCM_DICT_ENVIRONMENT_VARIABLE;
    }

}

//---------------------------------------------------------------------------------------------

dcmtkBase::~dcmtkBase()
{
#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif
}

//---------------------------------------------------------------------------------------------
