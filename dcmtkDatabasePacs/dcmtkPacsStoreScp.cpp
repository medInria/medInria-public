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
    scp.startService(ourTitle, ourIP, ourPort);
    return 1;
}

bool dcmtkPacsStoreScp::setStorageDirectory( const char* directory )
{
    return scp.setStorageDirectory(directory);
}

dcmtkPacsStoreScp::dcmtkPacsStoreScp()
{
    // forward signal
    connect(&scp, SIGNAL(endOfStudy(QString)), this,SIGNAL(endOfStudy(QString)) );
}

void dcmtkPacsStoreScp::stop()
{
    scp.stopService();
}

medAbstractPacsStoreScp * createDcmtkStoreScp( void )
{
    return new dcmtkPacsStoreScp;
}