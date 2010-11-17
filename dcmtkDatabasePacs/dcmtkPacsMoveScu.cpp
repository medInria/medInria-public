#include "dcmtkPacsMoveScu.h"
#include "medPacs/medAbstractPacsMoveScu.h"
#include "medPacs/medAbstractPacsFactory.h"
#include "medPacs/medAbstractPacsNode.h"
#include "dcmtkNode.h"

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

bool dcmtkPacsMoveScu::addRequestToQueue( int group, int elem, const char* query, medAbstractPacsNode& moveSource, medAbstractPacsNode& moveTarget)
{
    dcmtkNode source;
    source.setTitle(moveSource.title().toStdString());
    source.setIp(moveSource.ip().toStdString());
    source.setPort(moveSource.port());

    dcmtkNode target;
    target.setTitle(moveTarget.title().toStdString());
    target.setIp(moveTarget.ip().toStdString());
    target.setPort(moveTarget.port());

    return scu.addRequestToQueue(group, elem, query,source,target);
}

int dcmtkPacsMoveScu::performQueuedMoveRequests()
{
    return scu.performQueuedMoveRequests();
}

void dcmtkPacsMoveScu::sendCancelRequest()
{
    scu.sendCancelRequest();
}

medAbstractPacsMoveScu * createDcmtkMoveScu( void )
{
    return new dcmtkPacsMoveScu;
}
