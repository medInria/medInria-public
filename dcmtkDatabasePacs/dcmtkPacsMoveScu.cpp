#include "dcmtkPacsMoveScu.h"
#include "medPacs/medAbstractPacsMoveScu.h"
#include "medPacs/medAbstractPacsFactory.h"

QString dcmtkPacsMoveScu::description( void ) const
{
    return "dcmtkMoveScu";
}

bool dcmtkPacsMoveScu::registered( void )
{
    return medAbstractPacsFactory::instance()->registerMoveScuType("dcmtkMoveScu", createDcmtkMoveScu);
}

int dcmtkPacsMoveScu::sendMoveRequest( const char* peerTitle, const char* peerIP, unsigned int peerPort, 
                                       const char* ourTitle, const char* ourIP, unsigned int ourPort )
{
    return scu.sendMoveRequest(peerTitle, peerIP, peerPort, ourTitle, ourIP, ourPort);
}

bool dcmtkPacsMoveScu::addQueryAttribute( int group, int elem, const char* value )
{
    return scu.addQueryAttribute(group, elem, value);
}

void dcmtkPacsMoveScu::setQueryLevel( eQueryLevel level )
{
    scu.setQueryLevel(static_cast<dcmtkBaseScu::eQueryLevel>(level));
}

void dcmtkPacsMoveScu::clearAllQueryAttributes()
{
    scu.clearAllQueryAttributes();
}

void dcmtkPacsMoveScu::useBuildInStoreSCP( bool flag )
{
    scu.useBuildInStoreSCP(flag);
}

bool dcmtkPacsMoveScu::setStorageDirectory( const char* directory )
{
    return scu.setStorageDirectory(directory);
}

dcmtkPacsMoveScu::dcmtkPacsMoveScu()
{
    connect(&scu, SIGNAL(progressed(int)), this,SIGNAL(progressed(int)) );
}

medAbstractPacsMoveScu * createDcmtkMoveScu( void )
{
    return new dcmtkPacsMoveScu;
}
