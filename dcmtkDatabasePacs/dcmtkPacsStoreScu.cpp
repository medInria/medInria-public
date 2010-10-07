#include "dcmtkPacsStoreScu.h"
#include "medPacs/medAbstractPacsStoreScu.h"
#include "medPacs/medAbstractPacsFactory.h"

bool dcmtkPacsStoreScu::registered( void )
{
    return medAbstractPacsFactory::instance()->registerStoreScuType("dcmtkStoreScu", createDcmtkStoreScu);

}

QString dcmtkPacsStoreScu::description( void ) const
{
    return "dcmtkStoreScu";
}



int dcmtkPacsStoreScu::sendStoreRequest( const char* peerTitle, const char* peerIP, unsigned int peerPort, 
                                         const char* ourTitle, const char* ourIP, unsigned int ourPort )
{
    return scu->sendStoreRequest(peerTitle, peerIP, peerPort, ourTitle, ourIP, ourPort);
}

medAbstractPacsStoreScu * createDcmtkStoreScu( void )
{
    return new dcmtkPacsStoreScu;
}
