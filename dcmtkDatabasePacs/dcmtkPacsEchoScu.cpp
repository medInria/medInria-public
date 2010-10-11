#include "dcmtkPacsEchoScu.h"
#include "medPacs/medAbstractPacsEchoScu.h"
#include "medPacs/medAbstractPacsFactory.h"

QString dcmtkPacsEchoScu::description( void ) const
{
    return "dcmtkEchoScu";
}

bool dcmtkPacsEchoScu::registered( void )
{
    return medAbstractPacsFactory::instance()->registerEchoScuType("dcmtkEchoScu", createDcmtkEchoScu);
}

int dcmtkPacsEchoScu::sendEchoRequest( const char* peerTitle, const char* peerIP, unsigned int peerPort, const char* ourTitle, const char* ourIP, unsigned int ourPort )
{
    return scu.sendEchoRequest(peerTitle,peerIP,peerPort,ourTitle,ourIP,ourPort);
}

medAbstractPacsEchoScu *createDcmtkEchoScu(void)
{
    return new dcmtkPacsEchoScu;
}
