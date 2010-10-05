
#include "medAbstractPacsFindScu.h"

#include <dtkCore/dtkGlobal.h>

medAbstractPacsFindScu::medAbstractPacsFindScu(void)
{

}

medAbstractPacsFindScu::~medAbstractPacsFindScu(void)
{

}
    
int medAbstractPacsFindScu::sendFindRequest(const char* peerTitle, const char* peerIP, unsigned int peerPort, const char* ourTitle, const char* ourIP, unsigned int ourPort)
{
    DTK_UNUSED(peerTitle);
    DTK_UNUSED(peerIP);
    DTK_UNUSED(peerPort);
    DTK_UNUSED(ourTitle);
    DTK_UNUSED(ourIP);
    DTK_UNUSED(ourPort);
    
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}




