#include "dcmtkPacsStoreScp.h"

#include "medPacs/medAbstractPacsFactory.h"
#include "medPacs/medAbstractPacsStoreScp.h"

QString dcmtkPacsStoreScp::description( void ) const
{
    return "dcmtkStoreScp";
}

bool dcmtkPacsStoreScp::registered( void )
{
    return medAbstractPacsFactory::instance()->registerStoreScpType("dcmtkStoreScp", createDcmtkStoreScp);
}

int dcmtkPacsStoreScp::start( const char* ourTitle, const char* ourIP, unsigned int ourPort )
{
    scp.start(ourTitle, ourIP, ourPort);
    return 1;
}

bool dcmtkPacsStoreScp::setStorageDirectory( const char* directory )
{
    return scp.setStorageDirectory(directory);
}

medAbstractPacsStoreScp * createDcmtkStoreScp( void )
{
    return new dcmtkPacsStoreScp;
}